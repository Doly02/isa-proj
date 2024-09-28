/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      utilities.cpp
 *  Author:         Tomas Dolak
 *  Date:           27.09.2024
 *  Description:    Includes Utilities That Are Not Specifically Connected To Any Class.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           utilities.cpp
 *  @author         Tomas Dolak
 *  @date           27.09.2024
 *  @details        Includes Utilities That Are Not Specifically Connected To Any Class.
 * ****************************/

/************************************************/
/*                  Libraries                   */
/************************************************/
#include "../include/utilities.hpp"


/************************************************/
/*                  Code                        */
/************************************************/
void PrintHelp(void)
{
    printf("Usage: ...\n");
    printf("TODO\n");
}

bool fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

bool IsIp4Address(std::string& str)
{
    struct sockaddr_in sa;
    return (0 != inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)));    
}

bool isIpv6Address(const std::string& str) 
{
    struct sockaddr_in6 sa;  // Structure of IPv6 Address
    return (0 != inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr)));
}
