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
            std::cerr << "ERR: Failed to Close Socket in Destructor.\n";
        }
        sockfd = -1;  // Nastav deskriptor na neplatnou hodnotu
    }
}


std::string BaseImapClient::GenerateTag(void)
{
    std::stringstream tag_stream;

    tag_stream << 'A';

    // Add Incremented Hexadecimal Value
    tag_stream << std::setw(5) << std::setfill('0') << std::hex << ++mCurrentTagValue;
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
        std::cerr << "ERR: Unable to resolve hostname to IP address: " << gai_strerror(status) << "\n";
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
        std::cout << "INFO: Resolved " << hostname << " to " << ipVersion << " address: " << ip_str << "\n";

        // Clean up and return the first resolved IP
        freeaddrinfo(res);
        return std::string(ip_str);
    }

    // No address found
    freeaddrinfo(res);
    return EMPTY_STR;
}

int BaseImapClient::FindEndOfResponse(std::string buff)
{
    std::string current_tag = GetTag();
    switch(curr_state)
    {   /*TODO: Update Return Value For All TRANSMIT_DATA_FAILED */
        case LOGIN:
            /* login completed, now in authenticated state */
            if (std::string::npos != buff.find(current_tag + " OK LOGIN completed"))
                return SUCCESS;
            /* login failure: user name or password rejected */
            else if (std::string::npos != buff.find(current_tag + " NO LOGIN completed"))
                return TRANSMIT_DATA_FAILED; 
            /* command unknown or arguments invalid */
            else if (std::string::npos != buff.find(current_tag + " BAD LOGIN completed"))
                return TRANSMIT_DATA_FAILED; 
            else
                return CONTINUE_IN_RECEIVING;
            break;
        case LOGOUT:
            if (std::string::npos != buff.find(current_tag + " OK LOGOUT completed"))
                return SUCCESS;
            else if (std::string::npos != buff.find(current_tag + " BAD LOGOUT completed"))
                return TRANSMIT_DATA_FAILED; 
            else
                return CONTINUE_IN_RECEIVING;
            break;
        case SEARCH:
            /* OK - search completed */
            if (std::string::npos != buff.find(current_tag + " OK SEARCH completed"))
                return SUCCESS;
            /* NO - search error: can't search that [CHARSET] or criteria*/
            else if (std::string::npos != buff.find(current_tag + " NO SEARCH completed"))
                return TRANSMIT_DATA_FAILED;
            /* BAD - command unknown or arguments invalid */
            else if (std::string::npos != buff.find(current_tag + " BAD SEARCH completed"))
                return TRANSMIT_DATA_FAILED;
            else
                return CONTINUE_IN_RECEIVING;
            break;
        case FETCH:
            /* OK - fetch completed */
            if (std::string::npos != buff.find(current_tag + " OK FETCH completed"))
                return SUCCESS;
            /* NO - fetch error: can't fetch that data */
            else if (std::string::npos != buff.find(current_tag + " NO FETCH completed"))
                return TRANSMIT_DATA_FAILED;
            /* BAD - command unknown or arguments invalid */
            else if (std::string::npos != buff.find(current_tag + " BAD FETCH completed"))
                return TRANSMIT_DATA_FAILED;
            else
                return CONTINUE_IN_RECEIVING;
            break;
        default:
            return UNDEFINED_STATE;
            
    }
    return UNDEFINED_STATE;
}

/* TODO: BYE Command! */
/**
 * Zpravy jsou poznaceny UID (unikatni pro kazdou zpravu) -> Tak poznas ze zpravu jsi jiz stahoval.
 */