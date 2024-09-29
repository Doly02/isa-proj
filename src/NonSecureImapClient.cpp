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
NonSecureImapClient::NonSecureImapClient(const std::string& mailBox, const std::string& outDirectory)
    : mailbox(mailBox), 
    outputDir(outDirectory),
    rx_data(EMPTY_STR) {}


bool NonSecureImapClient::ConnectImapServer(const std::string& serverAddress, const std::string& username, const std::string& password)
{
    /* Set Current State of The Communication With IMAP Server */
    curr_state = LOGIN;

    std::string server_ip = serverAddress;
    bool is_ipv4_addr = IsIPv4Address(serverAddress);
    bool is_ipv6_addr = isIPv6Address(serverAddress);

    if (false == is_ipv4_addr && false == is_ipv6_addr)
    {
        server_ip = ResolveHostnameToIP(serverAddress,"143");
        if (server_ip.empty())
        {
            std::cerr << "ERR: Unable to Resolve Hostname To IP Address.\n";
        }
    }

    is_ipv4_addr = IsIPv4Address(server_ip);
    is_ipv6_addr = isIPv6Address(server_ip);

    /* Create a Socket For Either IPv4 or IPv6 */
    if (true == is_ipv4_addr)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (0 > sockfd)
        {
            std::cerr << "ERR: Unable To Create IPv4 Socket.\n";
            return false;
        }

        /* Preparation of IPv4 Server Addr. Struct */
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons((uint16_t)PORT_NON_SECURE); // IMAP Standard Port - 143 (Non-Secure)
        if (0 >= inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr)) 
        {
            std::cerr << "ERR: Invalid IPv4 Address Format.\n";
            close(sockfd);
            sockfd = -1;
            return false;
        }
        if (0 > connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) 
        {
            std::cerr << "ERR: Unable To Connect To The IMAP Server on IPv4 Protocol.\n";
            close(sockfd);
            sockfd = -1;
            return false;
        }
    }
    else if (true == is_ipv6_addr)
    {
        sockfd = socket(AF_INET6, SOCK_STREAM, 0);
        if (0 > sockfd) 
        {
            std::cerr << "ERR: Unable To Create IPv6 Socket.\n";
            return false;
        }

        struct sockaddr_in6 server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin6_family = AF_INET6;
        server_addr.sin6_port = htons((uint16_t)PORT_NON_SECURE);
        if (0 >= inet_pton(AF_INET6, server_ip.c_str(), &server_addr.sin6_addr)) 
        {
            std::cerr << "ERR: Invalid IPv6 Address Format.\n";
            close(sockfd);
            sockfd = -1;
            return false;
        }

        if (0 > connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) 
        {
            std::cerr << "ERR: Unable to connect to the IMAP server (IPv6).\n";
            close(sockfd);
            sockfd = -1;
            return false;
        }
    }

    /* Send Login Command To IMAP Server */
    LoginClient(username, password);
    if (SUCCESS != ReceiveData())
    {
        return false;
    }
    // TODO: Print To The User That Login Was Successful.

    return true; 

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

int NonSecureImapClient::ReceiveData()
{
    char        rx_buffer[RX_BUFFER_SIZE];
    ssize_t     bytes_rx;            //<! Num. of Received Bytes

    while(0 < (bytes_rx = recv(sockfd, rx_buffer, RX_BUFFER_SIZE-1, 0)))
    {
        rx_buffer[bytes_rx] = '\0';
        rx_data += rx_buffer;
        if (SUCCESS == BaseImapClient::FindEndOfResponse(std::string(rx_buffer)))
        {
            break;
        }
    }

    /* Handle Error If Occured During Transmission */
    if (0 > bytes_rx){
        return RECEIVE_DATA_FAILED;
    }
    return SUCCESS;

}

int NonSecureImapClient::LoginClient(std::string username, std::string password)
{
    std::string tag = GenerateTag();
    std::string log_cmd = tag + " LOGIN " + username + " " + password;
    if (SUCCESS != SendData(log_cmd))
    {
        std::cerr << "ERR: Failed to Login to IMAP Server.\n";
        return TRANSMIT_DATA_FAILED;
    }
    // TODO: Receive Response From The Server & Eval.

    return SUCCESS;
}

int NonSecureImapClient::LogoutClient()
{
    curr_state = LOGOUT;

    std::string tag = GenerateTag();  
    std::string logout_cmd = tag + " LOGOUT"; 
    if (SUCCESS != SendData(logout_cmd)) 
    {
        std::cerr << "ERR: Failed to Send LOGOUT Command to IMAP Server.\n";
        return TRANSMIT_DATA_FAILED;
    }

    if (SUCCESS != ReceiveData()) {
        std::cerr << "ERR: Failed to Receive LOGOUT Response from IMAP Server.\n";
        return RECEIVE_DATA_FAILED;
    }

    return SUCCESS;
}

int NonSecureImapClient::ParseUIDs()
{
    std::string uid_response = rx_data;
    std::string deleted_part;
    int uid = 0;
    std::string::size_type found;

    try
    {
        std::regex reg_expression("(\\r\\n.*(?=OK SEARCH completed)[.|\\s\\S]*)");
        std::smatch match;
        if (std::regex_search(uid_response, match, reg_expression) && (1 < match.size()))
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
    uid_response = uid_response.substr(0, uid_response.size() - deleted_part.size());
    deleted_part = "* SEARCH ";
    
    found = uid_response.find(deleted_part);
    if (std::string::npos != found)
    {
        uid_response.erase(found, deleted_part.length()); /* Erase The Prefix "* SEARCH" */
    }

    if (uid_response.empty())
    {
        /* ERR: No UIDs Found in Response */
        return NON_UIDS_RECEIVED;
    }

    std::istringstream parse_uids(uid_response);
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
    curr_state = SEARCH;

    std::string tag = GenerateTag();
    std::string fetch_uids_cmd = tag + " UID SEARCH ALL"; /* TODO: Here Will Be Update (Right Now -n Is not Avalaible)*/
    if (SUCCESS != SendData(fetch_uids_cmd)) 
    {
        std::cerr << "ERR: Failed to Fetch UIDs From IMAP Server.\n";
        return TRANSMIT_DATA_FAILED;
    } 
    if (SUCCESS != ReceiveData()) {
        std::cerr << "ERR: Failed to Receive UIDs From IMAP Server.\n";
        return RECEIVE_DATA_FAILED;
    }

    return SUCCESS;    
}


int NonSecureImapClient::SetMailBox()
{
    curr_state = SEARCH;

    std::string tag = GenerateTag();  
    std::string set_mailbox_cmd = tag + " SELECT " + mailbox;
    if (SUCCESS != SendData(set_mailbox_cmd)) 
    {
        std::cerr << "ERR: Failed to Set Mailbox on IMAP Server.\n";
        return TRANSMIT_DATA_FAILED;
    }    
    if (SUCCESS != ReceiveData()) {
        std::cerr << "ERR: Failed to Receive Response For Setup of Mailbox on IMAP Server.\n";
        return RECEIVE_DATA_FAILED;
    }
    return SUCCESS;
}