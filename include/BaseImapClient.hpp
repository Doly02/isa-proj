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

#ifndef BASE_IMAP_CLIENT_H
#define BASE_IMAP_CLIENT_H

/************************************************/
/*                  Libraries                   */
/************************************************/
#include <string>
#include <cstring>
#include <unistd.h>             /*!< Hostname Relevant, close() */
#include "definitions.hpp"

class BaseImapClient {

    private:
        unsigned long long mCurrentTagValue;

    public:

        /**
        * @brief Class Constructor.
        */
        BaseImapClient();

        /**
        * @brief       Resolves the Hostname of The Server to an IP Address (tries IPv4 First, Then IPv6).
        * @details     The Function Uses getaddrinfo() To Resolve The Hostname To An IP Address. 
        *              It First Attempts To Resolve The Address To an IPv4 Address. 
        *              If No IPv4 Address is Found, Method Tries To Resolve an IPv6 Address.
        *              The Correctness of This Method Can Be Tested Using The Command 'nslookup' or 'dig'.
        *              The Method Supports Both - IPv4 and IPv6 Addresses.
        * @param[in]   hostname The Server Hostname to Resolve.
        * @param[in]   port The Port Number as a String (e.g., "143" for IMAP).
        * @retval      A String Containing the Resolved IP Address (Either IPv4 or IPv6), or An Empty String in Case of Failure.
        */
        std::string ResolveHostnameToIP(const std::string& hostname, const std::string& port);

        /**
        * @brief       Generates Tags For Client Requests.
        * @details     Since The Tag is Client-Defined, The Tag Will Look Like 'Axxxxxxxxxxxx', 
        *              Where 'A' is a Fixed Character and The Sequence 'x' is a Ten-Character Hexadecimal Number, 
        *              Incrementing With Each Client's Request.
        * @retval      11-Byte Long String.
        */
        std::string generateTag(void);

        int EvaluateServerResponse(Response_t type, std::string response);

};

#endif /* BASE_IMAP_CLIENT_H */