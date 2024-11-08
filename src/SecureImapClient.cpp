/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      SecureImapClient.hpp
 *  Author:         Tomas Dolak
 *  Login:          xdolak09
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
      certDir(CertDirectory),
	  uidValidity(0)
{
    /* SSL/TLS Lib. Initialization */
    SSL_library_init();
    SSL_load_error_strings();
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
}

int SecureImapClient::ConnectImapServer(const std::string& serverAddress, const std::string& username, const std::string& password, int port)
{
    int ret_val = -1;

    /* Create SSL Context Only Once */
    ctx = SSL_CTX_new(TLS_client_method()); /* SSL Context For TLS Client */
    if (!ctx)
    {
        std::cerr << "ERR: Unable to Create SSL Context.\n";
        return CREATE_CONNECTION_FAILED;
    }

    /* Setup of File With Certificate */
    if (false == certFile.empty()) 
    {
        if (!SSL_CTX_load_verify_locations(ctx, certFile.c_str(), nullptr)) 
        {
            std::cerr << "ERR: Failed to Load Certificate From File: " << certFile << "\n";
            SSL_CTX_free(ctx);
            return SSL_CERT_VERIFICATION_FAILED;
        }
    }

    /* Setup of Directory With Certificates */
    if (false == certDir.empty()) 
    {
        if (!SSL_CTX_load_verify_locations(ctx, nullptr, certDir.c_str())) 
        {
            std::cerr << "ERR: Failed to Load Certificates From Directory: " << certDir << "\n";
            SSL_CTX_free(ctx);
            return SSL_CERT_VERIFICATION_FAILED;
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
            return CREATE_CONNECTION_FAILED;
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
            return CREATE_CONNECTION_FAILED;
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons((uint16_t)port); 
        if (0 >= inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr))
        {
            std::cerr << "ERR: Invalid IPv4 Address Format.\n";
            close(sockfd);
            sockfd = -1;
            SSL_CTX_free(ctx);
            return CREATE_CONNECTION_FAILED;
        }

        /* Connection to IMAP Server */
        if (0 > connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
        {
            std::cerr << "ERR: Unable To Connect To The IMAP Server on IPv4 Protocol.\n";
            close(sockfd);
            sockfd = -1;
            SSL_CTX_free(ctx);
            return CREATE_CONNECTION_FAILED;
        }
    }
    else
    {
        std::cerr << "ERR: Server Address Is Not Valid IPv4.\n";
        SSL_CTX_free(ctx);
        return CREATE_CONNECTION_FAILED;
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
        sockfd = -1;
        return CREATE_CONNECTION_FAILED;
    }
    /* Set LOGIN State */
    ret_val = LoginClient(username, password);
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }
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
        return BAD_RESPONSE;
    }

    time.tv_sec = TIMEOUT_SECURE;   /* Timeout in Secs */
    time.tv_usec = 0;               /* None Microsecs */
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time)) < 0)
    {
        return BAD_RESPONSE;
    }

    while (0 < (bytes_rx = SSL_read(ssl, rx_buffer, RX_BUFFER_SIZE - 1)))
    {
        rx_buffer[bytes_rx] = '\0';
        rx_data += rx_buffer;

        /* Check For The End of The Response */
        ret_val = BaseImapClient::FindEndOfResponse(std::string(rx_data)); 
        if (SUCCESS == ret_val)
        {
            break;
        }
        else if (TRANSMIT_DATA_FAILED == ret_val)
        {
            return BAD_RESPONSE;
        }
    }

    /* Clear The Socket */
    ClearSSLBuffer(ssl);

    /* Handle Errors During Transmission */
    if (0 > bytes_rx)
    {
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
        return PARSE_BY_REGEX_FAILED;
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

int SecureImapClient::FetchUIDs()
{
    std::string recv_data = EMPTY_STR;
    curr_state = SEARCH;

    std::string tag = GenerateTag();
    std::string fetch_uids_cmd = tag + " UID SEARCH";
    
    if (false == newOnly)
        fetch_uids_cmd += " ALL";
    else
        fetch_uids_cmd += " UNSEEN";

    
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
        return NON_UIDS_RECEIVED;
    }

    curr_state = DEFAULT;
    return SUCCESS;    
}

int SecureImapClient::GetUIDValidity()
{
    curr_state = SELECT;
    std::string tag = GenerateTag();
    std::string select_cmd = tag + " SELECT " + mailbox;  // UIDVALIDITY in The Select of The Mailbox
    std::string recv_data = EMPTY_STR;

    if (SUCCESS != SendData(select_cmd)) 
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

    /* Regular Expression to Find Value of UID Validity */
    std::regex uidvalidity_regex("\\*\\s+OK\\s+\\[UIDVALIDITY\\s+(\\d+)\\]");
    std::smatch match;

    if (std::regex_search(recv_data, match, uidvalidity_regex)) {
        if (1 < match.size()) 
        {
            std::string uidvalidity_str = match.str(1);

            try 
            {
                uidValidity = std::stoi(uidvalidity_str);
                curr_state = DEFAULT; /* Clear The State */
                return SUCCESS;  
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "ERR: Invalid UIDVALIDITY value format.\n";
                return UID_VALIDITY_ERROR_IN_RECV;
            }
            catch (const std::out_of_range& e) {
                std::cerr << "ERR: UIDVALIDITY value out of range.\n";
                return UID_VALIDITY_ERROR_IN_RECV;
            }
        }
    }

    std::cerr << "ERR: UIDVALIDITY Not Found in the Response.\n";
    return UID_VALIDITY_ERROR_IN_RECV;
}

int SecureImapClient::CheckUIDValidity()
{
    int ret_val = -1;
    std::string uidvalidity_file = GeneratePathToFile(outputDir, UIDVALIDITY_FILE);

    ret_val = GetUIDValidity();
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }

    ret_val = ReadUIDVALIDITYFile(uidvalidity_file, mailbox);
    if ((UIDVALIDITY_FILE_NOT_FOUND != ret_val) && (0 > ret_val))
    {
        /**
         * ReadUIDVALIDITYFile Return UIDVALIDITY Values Stored In Local (Output) Directory,
         * If The Return Values Is < 0, It Means That Some Error With Read of The Value.
         */
        return ret_val;
    }
    
    if (ret_val != uidValidity)
    {
        /* Program Will Remove Email Files From Folder And Then Downloaded Them Again. */
        
        /* Remove Email Files From Output Directory */
        std::string prefix = "MSG_" + mailbox + "_";
        ret_val = RemoveFilesMatchingPattern(outputDir, prefix, OUTPUT_FILE_FORMAT);
        if (SUCCESS != ret_val)
        {
            return ret_val;
        }
        /* Store Current Value of UIDVALIDITY */
        StoreUIDVALIDITY(uidValidity, mailbox, outputDir);

        /* Emails Are Removed, From Now Client Can Operate as Usual */
    }
    else
    {
        ; /* Program Will Run As Normal */
    }
    return SUCCESS;
}
int SecureImapClient::FetchEmails()
{
    int ret_val = NON_UIDS_RECEIVED;
    std::string email = EMPTY_STR;
    std::string path = EMPTY_STR;
    int num_of_uids = 0;

    ret_val = FetchUIDs();
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }


    for (int id : this->vec_uids)
    {
        /* 1401, 1402, 1556, 1554 */

        /* Assembly Path To File */
        path = GenerateFilename(id, mailbox, headersOnly, newOnly);
        path = GeneratePathToFile(outputDir, path);

        /**
         * If File Exists, Email Does Not Have To Be Downloaded Again.
         * If .uidvalidity File Does Not Match, Emails Will Be Removed Before This Function.
         */
        if (false == FileExists(path))
        {
            email = EMPTY_STR;
            email = FetchEmailByUID(id, WHOLE_MESSAGE);
            if (EMPTY_STR == email)
            {
                return FETCH_EMAIL_FAILED;   
            }
            email = ParseEmail(id, email, false);
            if (EMPTY_STR == email)
            {
                return FETCH_EMAIL_FAILED;   
            }
            StoreEmail(email, path);
            num_of_uids++;
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
        std::cerr << "ERR: Failed to Receive FETCH Response for UID: " << uid << "\n";
        return recv_data;
    }

    curr_state = DEFAULT;
    return recv_data;
}

std::string SecureImapClient::ParseEmail(int uid, std::string email, bool just_headers)
{
    std::string email_content;
    std::string email_body;
    std::string tag = EMPTY_STR;

    if (false == just_headers)
    {
        email_body = FetchEmailByUID(uid, JUST_HEADER);
        tag = GetTag();
        email_body = ParseEmailBody(email_body, tag);
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
    return SUCCESS;
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

    ret_val = CheckUIDValidity();
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }

    ret_val = FetchEmails();
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }

    ret_val = LogoutClient();
    if (SUCCESS != ret_val)
    {
        return ret_val;
    }    
    return SUCCESS;
}

/**
 * NOTES:
 * Je pojisteno ze se vsechny zdroje uvolni?
 */