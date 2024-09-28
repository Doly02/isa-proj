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
#include <iostream>     //!< For 'cerr' Output.

#include <unistd.h>     //!< Hostname Relevant.
#include <arpa/inet.h>  //!< Hostname Relevant.
#include <netdb.h>      //!< Hostname Relevant.
#include <sys/socket.h> //!< Hostname Relevant.
/************************************************/
/*             Class Implementation             */
/************************************************/
BaseImapClient::BaseImapClient() : mCurrentTagValue(0) {}

std::string BaseImapClient::generateTag(void)
{
    std::stringstream tag_stream;

    tag_stream << 'A';

    // Add Incremented Hexadecimal Value
    tag_stream << std::setw(10) << std::setfill('0') << std::hex << mCurrentTagValue++;
    return tag_stream.str();
}   

/**
 * @brief Resolves the hostname of the IMAP server to an IPv4 address.
 */
std::string BaseImapClient::resolveHostnameToIPv4(const std::string& hostname) {
    
    struct addrinfo hints, *pResults, *pCurrent;
    char ipstr[INET_ADDRSTRLEN];
    int status;

    // Set hints For IPv4 Addresses.
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;          // To Work Just Only With IPv4 Addresses (TODO: Maybe I Would Like To Work With IPv6 Addresses Too :D )
    hints.ai_socktype = SOCK_STREAM;    // Decision That TCP Connection Will Be Used

    // Get Address With The Help of getaddrinfo
    if ((status = getaddrinfo(hostname.c_str(), nullptr, &hints, &pResults)) != 0) 
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return "";
    }

    /**
     * It's Importatnt To Go Thru The Results Because One Hostname Can Be Associated With More Than One IP Address.
     */
    for (pCurrent = pResults; pCurrent != nullptr; pCurrent = pCurrent->ai_next) {
        
        /* Convert Binary IP Address To Readeable String */
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)pCurrent->ai_addr;
        void *addr = &(ipv4->sin_addr);
        inet_ntop(pCurrent->ai_family, addr, ipstr, sizeof ipstr);

        freeaddrinfo(pResults);  
        return std::string(ipstr);
    }

    /* None Address Found */
    freeaddrinfo(pResults);
    return "";
}