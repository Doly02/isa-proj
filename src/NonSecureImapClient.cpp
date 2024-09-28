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
    outputDir(outDirectory) {}


bool NonSecureImapClient::ConnectImapServer(const std::string& serverAddress, const std::string& username, const std::string& password)
{
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
            return false;
        }
        if (0 > connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) 
        {
            std::cerr << "ERR: Unable To Connect To The IMAP Server on IPv4 Protocol.\n";
            close(sockfd);
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
            return false;
        }

        if (0 > connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) 
        {
            std::cerr << "ERR: Unable to connect to the IMAP server (IPv6).\n";
            close(sockfd);
            return false;
        }
    }

    /* Send Login Command To IMAP Server */
    LoginClient(username, password);

    return true; 

}

int NonSecureImapClient::SendData(const std::string& data)
{
    ssize_t bytes_tx = 0;
    std::string message = data + "\r\n";

    bytes_tx = send(sockfd, message.c_str(), message.length(), 0);
    if (0 > bytes_tx)
    {
        std::cerr << "ERR: Failed to Send Data to IMAP Server.\n";
        return TRANSMIT_DATA_FAILED;
    }
    /* Check If All Data Was Transmitted */
    if (static_cast<size_t>(bytes_tx) != message.length())
    {
        std::cerr << "ERR: Some of The Data Were Not Transmitted!\n";
        return TRANSMIT_DATA_FAILED;
    }
    return SUCCESS;

}

int NonSecureImapClient::LoginClient(std::string username, std::string password)
{
    std::string tag = generateTag();
    std::string log_cmd = tag + " LOGIN " + username + " " + password;
    if (SUCCESS != SendData(log_cmd))
    {
        return TRANSMIT_DATA_FAILED;
    }
    return SUCCESS;
}

int NonSecureImapClient::LogoutClient()
{
    return -1;
}