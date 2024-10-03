/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      NonSecureImapClient.cpp
 *  Author:         Tomas Dolak
 *  Date:           28.09.2024
 *  Description:    Implements Non-Secure IMAPv4 Client That Communicates Just Thru TCP/IP With Port 143.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           NonSecureImapClient.cpp
 *  @author         Tomas Dolak
 *  @date           28.09.2024
 *  @brief          Implements Non-Secure IMAPv4 Client That Communicates Just Thru TCP/IP With Port 143.
 * ****************************/
/************************************************/
/*                  Libraries                   */
/************************************************/
#include "../include/NonSecureImapClient.hpp"

/************************************************/
/*             Class Implementation             */
/************************************************/
NonSecureImapClient::NonSecureImapClient(const std::string& MailBox, const std::string& OutDirectory, bool HeadersOnly, bool NewOnly)
    : mailbox(MailBox), 
    outputDir(OutDirectory),
    headersOnly(HeadersOnly),
    newOnly(NewOnly){}

int NonSecureImapClient::ConnectImapServer(const std::string& serverAddress, const std::string& username, const std::string& password, int port)
{
    std::string server_ip = serverAddress;
    bool is_ipv4_addr = IsIPv4Address(serverAddress);

    /* Set Current State of The Communication With IMAP Server */
    curr_state = LOGIN;

    if (false == is_ipv4_addr)
    {
        server_ip = ResolveHostnameToIP(serverAddress, std::to_string(port));
        if (server_ip.empty())
        {
            std::cerr << "ERR: Unable to Resolve Hostname To IP Address.\n";
        }
    }

    is_ipv4_addr = IsIPv4Address(server_ip);

    /* Create a Socket For Either IPv4 or IPv6 */
    if (true == is_ipv4_addr)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (0 > sockfd)
        {
            std::cerr << "ERR: Unable To Create IPv4 Socket.\n";
            return -3;
        }

        /* Preparation of IPv4 Server Addr. Struct */
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons((uint16_t)port); // IMAP Standard Port - 143 (Non-Secure)
        if (0 >= inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr)) 
        {
            std::cerr << "ERR: Invalid IPv4 Address Format.\n";
            close(sockfd);
            sockfd = -1;
            return -3;
        }
        if (0 > connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) 
        {
            std::cerr << "ERR: Unable To Connect To The IMAP Server on IPv4 Protocol.\n";
            close(sockfd);
            sockfd = -1;
            return -3;
        }
    }
    curr_state = LOGIN;
    /* Send Login Command To IMAP Server */
    LoginClient(username, password);

    // TODO: Print To The User That Login Was Successful?
    curr_state = DEFAULT;
    return SUCCESS; 

}

int NonSecureImapClient::SendData(const std::string& data)
{
    ssize_t bytes_tx = 0;
    std::string message = data + "\r\n";
    bytes_tx = send(sockfd, message.c_str(), message.length(), 0);
    if (0 > bytes_tx)
    {
        return TRANSMIT_DATA_FAILED;
    }

    /* Check If All Data Was Transmitted */
    if (static_cast<size_t>(bytes_tx) != message.length())
    {
        return TRANSMIT_DATA_FAILED;
    }
    return SUCCESS;

}

std::string NonSecureImapClient::ReceiveData()
{

    char            rx_buffer[RX_BUFFER_SIZE] = { 0 };  //<! Buffer That Interacts With recv()
    ssize_t         bytes_rx = 0;                       //<! Num. of Received Bytes
    std::string     rx_data = EMPTY_STR;                //<! Buffer For Server Response
    int             ret_val = -1;
    struct timeval  time;


    /* Timeout Setup*/
    time.tv_sec = TIMEOUT;
    time.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time)) < 0)  //<! Setup of Socket Timeout
    {   
        std::cerr << "Error Setting Timeout For recv() Function." << std::endl;
        return BAD_RESPONSE;
    }

    while(0 < (bytes_rx = recv(sockfd, rx_buffer, RX_BUFFER_SIZE - 1, 0)))
    {

        rx_buffer[bytes_rx] = '\0';
        rx_data += rx_buffer;
        ret_val = BaseImapClient::FindEndOfResponse(std::string(rx_buffer));
        if (SUCCESS == ret_val)
        {
            break;
        }
        else if (TRANSMIT_DATA_FAILED == ret_val)
        {
            return BAD_RESPONSE;
        }
    }

    /* Handle Error If Ocurred During Transmission */
    if (0 > bytes_rx) 
    {
        if (EAGAIN == errno || EWOULDBLOCK == errno) 
        {    
            std::cerr << "Error: Timeout Overrun While Receiving Data - recv() Timeout." << std::endl;
        } else 
        {        
            std::cerr << "Error Receiving Data: " << strerror(errno) << std::endl;
        }
        return EMPTY_STR;
    }
    return rx_data;
}

int NonSecureImapClient::LoginClient(std::string username, std::string password)
{
    std::string tag = GenerateTag();
    std::string log_cmd = tag + " LOGIN " + username + " " + password;
    std::string recv_data   = EMPTY_STR;

    if (SUCCESS != SendData(log_cmd))
    {
        std::cerr << "ERR: Failed to Login to IMAP Server.\n";
        return TRANSMIT_DATA_FAILED;
    }

    /* Receive Response From The Server & Eval. */
    recv_data = ReceiveData();
    if (EMPTY_STR == recv_data || BAD_RESPONSE == recv_data) 
    {
        std::cerr << "ERR: Failed to Receive LOGIN Response from IMAP Server.\n";
        return RECEIVE_DATA_FAILED;
    }
    return SUCCESS;
}

int NonSecureImapClient::LogoutClient()
{
    curr_state              = LOGOUT;
    std::string recv_data   = EMPTY_STR;

    std::string tag = GenerateTag();  
    std::string logout_cmd = tag + " LOGOUT"; 
    if (SUCCESS != SendData(logout_cmd)) 
    {
        std::cerr << "ERR: Failed to Send LOGOUT Command to IMAP Server.\n";
        return TRANSMIT_DATA_FAILED;
    }

    recv_data = ReceiveData();
    if (EMPTY_STR == recv_data || BAD_RESPONSE == recv_data) 
    {
        std::cerr << "ERR: Failed to Receive LOGOUT Response from IMAP Server.\n";
        return RECEIVE_DATA_FAILED;
    }
    
    curr_state = DEFAULT;
    return SUCCESS;
}

int NonSecureImapClient::ParseUIDs(std::string response)
{
    std::string deleted_part;
    int uid = 0;
    std::string::size_type found;

    try
    {
        std::regex reg_expression("(\\r\\n.*(?=OK SEARCH completed)[.|\\s\\S]*)");
        std::smatch match;
        if (std::regex_search(response, match, reg_expression) && (1 < match.size()))
        {
            deleted_part = match.str(1);
        }
        else
        {
            deleted_part = EMPTY_STR;
        }
    }
    catch (std::regex_error& e)
    {
        /* ERR: Regex Error While Parsing UIDs */
        return PARSE_REGEX_FAILED;
    }
    response = response.substr(0, response.size() - deleted_part.size());
    deleted_part = "* SEARCH ";
    
    found = response.find(deleted_part);
    if (std::string::npos != found)
    {
        response.erase(found, deleted_part.length()); /* Erase The Prefix "* SEARCH" */
    }

    if (response.empty())
    {
        /* ERR: No UIDs Found in Response */
        return NON_UIDS_RECEIVED;
    }

    std::istringstream parse_uids(response);
    for (std::string tok; std::getline(parse_uids, tok, ' '); )
    {
        uid = std::atoi(tok.c_str());
        if (0  < uid)
            vec_uids.push_back(uid);
    }

    return SUCCESS;
}

int NonSecureImapClient::FetchUIDs()
{
    std::string recv_data = EMPTY_STR;
    curr_state = SEARCH;

    std::string tag = GenerateTag();
    std::string fetch_uids_cmd = tag + " UID SEARCH";
    
    printf("value_true: %s\n", newOnly ? "true" : "false");
    if (false == newOnly)
        fetch_uids_cmd += " ALL";
    else
        fetch_uids_cmd += " UNSEEN"; /*TODO: Check If Requirements Are Satisfide */

    
    if (SUCCESS != SendData(fetch_uids_cmd)) 
    {
        std::cerr << "ERR: Failed to Fetch UIDs From IMAP Server.\n";
        return TRANSMIT_DATA_FAILED;
    }
    recv_data = ReceiveData();
    if (EMPTY_STR == recv_data || BAD_RESPONSE == recv_data) 
    {
        std::cerr << "ERR: Failed to Receive UIDs From IMAP Server.\n";
        return RECEIVE_DATA_FAILED;
    }
    if (SUCCESS != ParseUIDs(recv_data)) 
    {
        std::cerr << "ERR: Failed to Parse UIDs.\n";
        return NON_UIDS_RECEIVED;                       /* TODO: Update Logic of RetVal! */
    }

    curr_state = DEFAULT;

    return SUCCESS;    
}

int NonSecureImapClient::FetchEmails()
{
    int ret_val = NON_UIDS_RECEIVED;
    std::string email = EMPTY_STR;
    std::string path = EMPTY_STR;
    int num_of_uids = int(vec_uids.size());

    ret_val = FetchUIDs();
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }


    for (int id : this->vec_uids)
    {
        /*if (id >= 36 && id <= 40)
        {*/
        email = EMPTY_STR;
        email = FetchEmailByUID(id, WHOLE_MESSAGE);
        if (EMPTY_STR == email)
        {
            return PARSE_EMAIL_FAILED;   
        }
        /* Assembly Path To File */
        path = GenerateFilename(id);
        path = GeneratePathToFile(outputDir, path);
        email = ParseEmail(id, email, false);
        StoreEmail(email, path);
        /*}*/

    }
    PrintNumberOfMessages(num_of_uids, newOnly, headersOnly);
    return num_of_uids;
}

std::string NonSecureImapClient::FetchEmailByUID(int uid, bool mode)
{
    curr_state = FETCH;
    std::string recv_data = EMPTY_STR;
    std::string fetch_cmd = EMPTY_STR;
    std::string tag = GenerateTag();

    if (WHOLE_MESSAGE == mode)
    {
        fetch_cmd = tag + " UID FETCH " + std::to_string(uid) + " BODY[HEADER]";
    }
    else if (JUST_HEADER == mode)
    {
        fetch_cmd = tag + " UID FETCH " + std::to_string(uid) + " BODY[TEXT]"; 
    }

    if (SUCCESS != SendData(fetch_cmd))
    {
        std::cerr << "ERR: Failed to Send FETCH Command for UID: " << uid << "\n";
        return EMPTY_STR;
    }

    recv_data = ReceiveData();
    if (EMPTY_STR == recv_data || BAD_RESPONSE == recv_data) 
    {
        std::cerr << "ERR: Failed to Receive Data for UID: " << uid << "\n";
        return recv_data;
    }

    curr_state = DEFAULT;
    return recv_data;
}

std::string NonSecureImapClient::ParseEmail(int uid, std::string email, bool just_headers)
{
    std::string email_content;
    std::string email_body;

    if (false == just_headers)
    {
        email_body = FetchEmailByUID(uid, JUST_HEADER);
        email_body = ParseEmailBody(email_body);
    }
    email_content = ParseEmailHeader(email);

    if (false == just_headers)
    {
        email_content = email_content.append(email_body);
    }
    return email_content;
}

int NonSecureImapClient::SetMailBox()
{
    std::string recv_data = EMPTY_STR;
    curr_state = SELECT;

    std::string tag = GenerateTag();  
    std::string set_mailbox_cmd = tag + " SELECT " + mailbox;
    if (SUCCESS != SendData(set_mailbox_cmd)) 
    {
        std::cerr << "ERR: Failed to Set Mailbox on IMAP Server.\n";
        return TRANSMIT_DATA_FAILED;
    }
    recv_data = ReceiveData();
    if (EMPTY_STR == recv_data || BAD_RESPONSE == recv_data) {
        std::cerr << "ERR: Failed to Receive Response For Setup of Mailbox on IMAP Server.\n";
        return RECEIVE_DATA_FAILED;
    }

    curr_state = DEFAULT;
    return SUCCESS;
}

int NonSecureImapClient::DisconnectImapServer(void)
{
    if (0 < sockfd)
    {
        curr_state = LOGOUT;
        std::string tag = GenerateTag();
        std::string logout_cmd = tag + " LOGOUT";
        std::string recv_data = EMPTY_STR;
        
        if (SUCCESS != SendData(logout_cmd))
        {
            std::cerr << "ERR: Failed to Login to IMAP Server.\n";
            return TRANSMIT_DATA_FAILED;
        }
        recv_data = ReceiveData();
        if (EMPTY_STR == recv_data || BAD_RESPONSE == recv_data) 
        {
            std::cerr << "ERR: Failed to Receive LOGOUT Response.\n";
            return false;
        }
        return SUCCESS;
    }
    return SUCCESS; /* TODO: Already Closed */
}

int NonSecureImapClient::Run(const std::string& serverAddress, int server_port, const std::string& username, const std::string& password)
{
    int ret_val = -4;
    ret_val = ConnectImapServer(serverAddress, username, password, server_port);
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }

    /* Set MailBox */
    ret_val = SetMailBox();
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }

    ret_val = FetchEmails();
    
    printf("Fetched New: %d Emails!", ret_val);

    ret_val = LogoutClient();
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }    
    return SUCCESS;
}

/**
 * Pokud dojde k nejake blbosti a klient chce skoncit nemel by se odhlasit ze serveru? (slusne se odhlasit)
 */