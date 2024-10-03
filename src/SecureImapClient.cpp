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
SecureImapClient::SecureImapClient(const std::string& MailBox, const std::string& OutDirectory, bool HeadersOnly, bool NewOnly)
    : mailbox(MailBox), 
      outputDir(OutDirectory),
      headersOnly(HeadersOnly),
      newOnly(NewOnly),
      ssl(nullptr),             /* SSL Pointer      */
      ctx(nullptr)              /* SSL_CTX Pointer  */ 
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
    curr_state = LOGIN;
    LoginClient(username, password);
    curr_state = DEFAULT;
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

    time.tv_sec = TIMEOUT;   /* Timeout in Secs */
    time.tv_usec = 0;        /* None Microsecs */
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
}

int SecureImapClient::LoginClient(std::string username, std::string password)
{
    (void)username;
    (void)password;
    return SUCCESS;
}



/**
 * TODO: Je pojisteno ze se vsechny zdroje uvolni?
 */