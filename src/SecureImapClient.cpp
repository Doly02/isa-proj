/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      SecureImapClient.hpp
 *  Author:         Tomas Dolak
 *  Date:           02.10.2024
 *  Description:    Implements Secure IMAP4rev1 Client That Communicates Thru TCP/IP and SSL/TLS.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           SecureImapClient.hpp
 *  @author         Tomas Dolak
 *  @date           02.10.2024
 *  @brief          Implements Secure IMAP4rev1 Client That Communicates Thru TCP/IP and SSL/TLS.
 * ****************************/
/************************************************/
/*                  Libraries                   */
/************************************************/
#include "../include/SecureImapClient.hpp"

/************************************************/
/*             Class Implementation             */
/************************************************/
SecureImapClient::SecureImapClient(const std::string& MailBox, 
                                   const std::string& OutDirectory, 
                                   bool HeadersOnly, 
                                   bool NewOnly,
                                   const std::string& CertFile,
                                   const std::string& CertDirectory)
    : mailbox(MailBox), 
      outputDir(OutDirectory),
      headersOnly(HeadersOnly),
      newOnly(NewOnly),
      ssl(nullptr),             /* SSL Pointer      */
      ctx(nullptr),             /* SSL_CTX Pointer  */ 
      certFile(CertFile),
      certDir(CertDirectory)
{
    /* SSL/TLS Lib. Initialization */
    SSL_library_init();
    SSL_load_error_strings();
    /* ERR_load_BIO_strings();  TODO: Maybe Deprecated Function of SSL Library */
    OpenSSL_add_all_algorithms();

}

SecureImapClient::~SecureImapClient()
{
    if (nullptr != ssl)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }

    if (nullptr != ctx)
    {
        SSL_CTX_free(ctx);
    }

    if (0 <= sockfd)
    {
        close(sockfd);
    }
}

int SecureImapClient::ConnectImapServer(const std::string& serverAddress, const std::string& username, const std::string& password, int port)
{
    /* Create SSL Context Only Once */
    ctx = SSL_CTX_new(TLS_client_method()); /* SSL Context For TLS Client */
    if (!ctx)
    {
        std::cerr << "ERR: Unable to Create SSL Context.\n";
        return -3;
    }

    /* Setup of File With Certificate */
    if (false == certFile.empty()) 
    {
        if (!SSL_CTX_load_verify_locations(ctx, certFile.c_str(), nullptr)) 
        {
            std::cerr << "ERR: Failed to Load Certificate From File: " << certFile << "\n";
            SSL_CTX_free(ctx);
            return -3;
        }
    }

    /* Setup of Directory With Certificates */
    if (false == certDir.empty()) 
    {
        if (!SSL_CTX_load_verify_locations(ctx, nullptr, certDir.c_str())) 
        {
            std::cerr << "ERR: Failed to Load Certificates From Directory: " << certDir << "\n";
            SSL_CTX_free(ctx);
            return -3;
        }
    }

    /* Resolve IPv4 Address And Socket Creation */
    std::string server_ip = serverAddress;
    bool is_ipv4_addr = IsIPv4Address(serverAddress);

    if (false == is_ipv4_addr)
    {
        server_ip = ResolveHostnameToIP(serverAddress, std::to_string(port));
        if (server_ip.empty())
        {
            std::cerr << "ERR: Unable to Resolve Hostname To IP Address.\n";
            SSL_CTX_free(ctx);
            return -3;
        }
    }

    is_ipv4_addr = IsIPv4Address(server_ip);
    if (true == is_ipv4_addr)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (0 > sockfd)
        {
            std::cerr << "ERR: Unable To Create IPv4 Socket.\n";
            SSL_CTX_free(ctx);
            return -3;
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons((uint16_t)port); 
        if (0 >= inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr))
        {
            std::cerr << "ERR: Invalid IPv4 Address Format.\n";
            close(sockfd);      // TODO: Set Socket To -1 (Invalid Value)
            SSL_CTX_free(ctx);
            return -3;
        }

        /* Connection to IMAP Server */
        if (0 > connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
        {
            std::cerr << "ERR: Unable To Connect To The IMAP Server on IPv4 Protocol.\n";
            close(sockfd);
            SSL_CTX_free(ctx);
            return -3;
        }
    }
    else
    {
        std::cerr << "ERR: Server Address Is Not Valid IPv4.\n";
        SSL_CTX_free(ctx);
        return -3;
    }

    /* Creation of SSL/TLS Connection */
    ssl = SSL_new(ctx);                     /* Uses the SSL_CTX created once */
    SSL_set_fd(ssl, sockfd);                /* Sets the socket */

    /* SSL Handshake */
    if (0 >= SSL_connect(ssl))
    {
        std::cerr << "ERR: SSL Handshake Failed.\n";
        SSL_free(ssl);
        ssl = nullptr;
        SSL_CTX_free(ctx);
        ctx = nullptr;
        close(sockfd);
        return -3;
    }
    /* Set LOGIN State */
    LoginClient(username, password);
    return SUCCESS;
}


int SecureImapClient::SendData(const std::string& data)
{
    ssize_t bytes_tx = 0;
    std::string message = data + "\r\n";

    if (nullptr == ssl)
    {
        std::cerr << "ERR: SSL Object Is Not Initialized.\n";
        return TRANSMIT_DATA_FAILED;
    }

    /* Sends Data Thru SLL Connection */
    bytes_tx = SSL_write(ssl, message.c_str(), message.length());
    if (0 > bytes_tx)
    {
        std::cerr << "ERR: Failed to Send Data Over SSL Connection.\n";
        return TRANSMIT_DATA_FAILED;
    }

    if (static_cast<size_t>(bytes_tx) != message.length())
    {
        std::cerr << "ERR: Not All Data Was Transmitted Over SSL Connection.\n";
        return TRANSMIT_DATA_FAILED;
    }
    return SUCCESS;
}

std::string SecureImapClient::ReceiveData(void)
{
    char rx_buffer[RX_BUFFER_SIZE]  = { 0 };        /* Buffer For Data Reception    */
    ssize_t bytes_rx                = 0;            /* Number of Received Bytes     */
    std::string rx_data             = EMPTY_STR;    /* Buffer For Server Response   */
    int ret_val                     = -1;
    struct timeval time;

    if (nullptr == ssl)
    {
        std::cerr << "ERR: SSL object is not initialized.\n";
        return BAD_RESPONSE;
    }

    time.tv_sec = TIMEOUT_SECURE;   /* Timeout in Secs */
    time.tv_usec = 0;               /* None Microsecs */
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time)) < 0)
    {
        std::cerr << "Error Setting Timeout for SSL_read() function.\n";
        return BAD_RESPONSE;
    }

    while (0 < (bytes_rx = SSL_read(ssl, rx_buffer, RX_BUFFER_SIZE - 1)))
    {
        rx_buffer[bytes_rx] = '\0';
        rx_data += rx_buffer;

        /* Check For The End of The Response */
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
#if 0
    printf("Received Data:\n");
    printf("%s", rx_data.c_str());
#endif

    /* Disabled Timeout */
    time.tv_sec = 0;
    time.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time)) < 0)
    {
        std::cerr << "Error Resetting Timeout for SSL_read() function.\n";
        return BAD_RESPONSE;
    }


    /* Handle Errors During Transmission */
    if (0 > bytes_rx)
    {
        int ssl_error = SSL_get_error(ssl, bytes_rx);
        if (SSL_ERROR_WANT_READ == ssl_error || SSL_ERROR_WANT_WRITE == ssl_error)
        {
            std::cerr << "Error: Timeout while receiving data with SSL_read().\n";
        }
        else
        {
            std::cerr << "Error: Failed to receive data over SSL - " << strerror(errno) << std::endl;
        }
        return EMPTY_STR;
    }

    return rx_data;
}


int SecureImapClient::LoginClient(std::string username, std::string password)
{
    std::string tag = GenerateTag();
    std::string log_cmd = tag + " LOGIN " + username + " " + password;
    std::string recv_data   = EMPTY_STR;
    curr_state = LOGIN;

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

    curr_state = DEFAULT;
    return SUCCESS;
}

int SecureImapClient::LogoutClient()
{
    curr_state              = LOGOUT;
    std::string recv_data   = EMPTY_STR;
    std::string tag         = GenerateTag();  
    std::string logout_cmd  = tag + " LOGOUT"; 
    
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

int SecureImapClient::ParseUIDs(std::string response)
{
    std::string deleted_part;
    int uid = 0;
    std::string::size_type found;
    std::string tag = GetTag(); 

    try
    {
        std::regex reg_expression("(\\r\\n" + tag + "\\s.*)");
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
    deleted_part = "* SEARCH "; //FIXME:
    
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

int SecureImapClient::FetchUIDs()
{
    std::string recv_data = EMPTY_STR;
    curr_state = SEARCH;

    std::string tag = GenerateTag();
    std::string fetch_uids_cmd = tag + " UID SEARCH";
    
    printf("value_true: %s\n", newOnly ? "true" : "false");
    if (false == newOnly)
        fetch_uids_cmd += " ALL";
    else
        fetch_uids_cmd += " UNSEEN"; /*TODO: Check If Requirements Are Satisfied */

    
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

int SecureImapClient::FetchEmails()
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
        if (id >= 51 && id <= 55)
        {
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
        }

    }
    PrintNumberOfMessages(num_of_uids, newOnly, headersOnly);
    return SUCCESS;
}

std::string SecureImapClient::FetchEmailByUID(int uid, bool mode)
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

std::string SecureImapClient::ParseEmail(int uid, std::string email, bool just_headers)
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

int SecureImapClient::SetMailBox()
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

int SecureImapClient::DisconnectImapServer(void)
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

        curr_state = DEFAULT;
        return SUCCESS;
    }
    return SUCCESS; /* TODO: Already Closed */
}

int SecureImapClient::Run(const std::string& serverAddress, int server_port, const std::string& username, const std::string& password)
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
    
    ret_val = LogoutClient();
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }    
    return SUCCESS;
}



/**
 * TODO: Je pojisteno ze se vsechny zdroje uvolni?
 */