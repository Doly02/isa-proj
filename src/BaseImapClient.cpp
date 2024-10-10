/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      BaseImapClient.cpp
 *  Author:         Tomas Dolak
 *  Date:           24.09.2024
 *  Description:    Implements The Base Class For IMAPv4 Client.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           BaseImapClient.cpp
 *  @author         Tomas Dolak
 *  @date           24.09.2024
 *  @brief          Implements The Base Class For IMAPv4 Client.
 * ****************************/

/************************************************/
/*                  Libraries                   */
/************************************************/
#include "../include/BaseImapClient.hpp"
#include <sstream>      //!< For Tag Creation.
#include <iomanip>      //!< For Tag Creation.

#include <unistd.h>     //!< Hostname Relevant.
/************************************************/
/*             Class Implementation             */
/************************************************/
BaseImapClient::BaseImapClient() : mCurrentTagValue(0), 
                                    curr_state(LOGIN){}

BaseImapClient::~BaseImapClient(){
    if (sockfd >= 0) {
        if (close(sockfd) < 0) {
            std::cerr << "DEBUG: Failed to Close Socket in Destructor.\n";
        }
        sockfd = -1;
    }
}


std::string BaseImapClient::GenerateTag(void)
{
    std::stringstream tag_stream;

    tag_stream << 'A';

    // Add Incremented Hexadecimal Value
    tag_stream << std::setw(10) << std::setfill('0') << std::hex << ++mCurrentTagValue;
    return tag_stream.str();
}  

std::string BaseImapClient::GetTag()
{
    std::stringstream tag_stream;
    tag_stream << 'A';
    tag_stream << std::setw(10) << std::setfill('0') << std::hex << mCurrentTagValue;
    return tag_stream.str();
}

/**
 * @brief Resolves the hostname of the IMAP server to an IP address.
 */
std::string BaseImapClient::ResolveHostnameToIP(const std::string& hostname, const std::string& port) {
    struct addrinfo hints, *res, *p;
    int status;
    char ip_str[INET6_ADDRSTRLEN]; // Enough to store either IPv4 or IPv6 address

    // Set hints to try both IPv4 and IPv6
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    // Get address info
    if ((status = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res)) != 0) {
        std::cerr << "ERR: Unable to Resolve Hostname to IP Address: " << gai_strerror(status) << "\n";
        return EMPTY_STR;
    }

    // Loop through the results and process the first valid address (prefer IPv4)
    for (p = res; p != nullptr; p = p->ai_next) {
        void *addr;
        std::string ipVersion;

        // Check if it's IPv4 or IPv6
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipVersion = "IPv4";
        } else if (p->ai_family == AF_INET6 && 0) { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipVersion = "IPv6";
        } else {
            continue;
        }

        // Convert the IP to a readable string
        inet_ntop(p->ai_family, addr, ip_str, sizeof ip_str);
        std::cout << "DEBUG: Info: Resolved " << hostname << " To " << ipVersion << " Address: " << ip_str << "\n";

        // Clean up and return the first resolved IP
        freeaddrinfo(res);
        return std::string(ip_str);
    }

    // No address found
    freeaddrinfo(res);
    return EMPTY_STR;
}

void BaseImapClient::PrintNumberOfMessages(int num, bool new_only, bool headers_only)
{
    if (false == new_only && false == headers_only)
        printf("Stored %d Messages\n", num);
    else if (true == new_only && false == headers_only)
        printf("Stored %d New Messages\n", num);
    else if (false == new_only && true == headers_only)
        printf("Stored %d Message Headers\n", num);
    else
        printf("Stored %d New Message Headers\n", num);
}


int BaseImapClient::FindEndOfResponse(std::string buff)
{
    std::string current_tag = GetTag();
    switch(curr_state)
    {   
        case LOGIN:
            /* login completed, now in authenticated state */
            if (std::string::npos != buff.find(current_tag + " OK "))
                return SUCCESS; 
            /* Greeting from IMAP Server FIXME */
            else if (std::string::npos != buff.find(current_tag + "* OK "))
                return TRANSMIT_DATA_FAILED; 
            /* login failure: user name or password rejected */
            else if (std::string::npos != buff.find(current_tag + " NO "))
                return TRANSMIT_DATA_FAILED; 
            /* command unknown or arguments invalid */
            else if (std::string::npos != buff.find(current_tag + " BAD "))
                return TRANSMIT_DATA_FAILED; 
            else
                return CONTINUE_IN_RECEIVING;
            break;
        case LOGOUT:
            if (std::string::npos != buff.find(current_tag + " OK "))
                return SUCCESS;
            else if (std::string::npos != buff.find(current_tag + " BAD "))
                return TRANSMIT_DATA_FAILED; 
            else
                return CONTINUE_IN_RECEIVING;
            break;
        case SEARCH:
            /* OK - search completed */
            if (std::string::npos != buff.find(current_tag + " OK "))
                return SUCCESS;
            /* NO - search error: can't search that [CHARSET] or criteria*/
            else if (std::string::npos != buff.find(current_tag + " NO "))
                return TRANSMIT_DATA_FAILED;
            /* BAD - command unknown or arguments invalid */
            else if (std::string::npos != buff.find(current_tag + " BAD "))
                return TRANSMIT_DATA_FAILED;
            else
                return CONTINUE_IN_RECEIVING;
            break;
        case FETCH:
            /* OK - fetch completed */
            if (std::string::npos != buff.find(current_tag + " OK "))
                return SUCCESS;
            /* NO - fetch error: can't fetch that data */
            else if (std::string::npos != buff.find(current_tag + " NO "))
                return TRANSMIT_DATA_FAILED;
            /* BAD - command unknown or arguments invalid */
            else if (std::string::npos != buff.find(current_tag + " BAD "))
                return TRANSMIT_DATA_FAILED;
            else
                return CONTINUE_IN_RECEIVING;
            break;
        case SELECT:
            /* OK [READ-WRITE] - select completed, now in selected state -> Client is Able To Modify Mailbox */
            /* OK [READ-ONLY] The Client is Not Permitted To Modify The Mailbox But is Permitted Read Access, 
               The Mailbox is Selected as Read-Only*/
            if (std::string::npos != buff.find(current_tag + " OK "))
                return SUCCESS;
            /* NO - select failure, now in authenticated state: no such mailbox, can't access mailbox */
            else if (std::string::npos != buff.find(current_tag + " NO "))
                return TRANSMIT_DATA_FAILED;
            /* BAD - command unknown or arguments invalid */
            else if (std::string::npos != buff.find(current_tag + " BAD "))
                return TRANSMIT_DATA_FAILED;
            else
                return CONTINUE_IN_RECEIVING;
        default:
            return UNDEFINED_STATE;
            
    }
    return UNDEFINED_STATE;
}

/* TODO: BYE Command! */
/**
 * Zpravy jsou poznaceny UID (unikatni pro kazdou zpravu) -> Tak poznas ze zpravu jsi jiz stahoval.
 *
 *
 * SELECT Command: 
 * 1) RFC:              A142 OK [READ-WRITE] SELECT completed
 * 2) Real Server:      A0000000002 OK [READ-WRITE] Select completed
 *
 * TODO: Check If Files Are Correcly Stored.
 */