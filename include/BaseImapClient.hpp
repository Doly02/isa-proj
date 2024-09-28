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

class BaseImapClient {

    private:
        unsigned long long mCurrentTagValue;

    public:

        /**
        * @brief Class Constructor.
        */
        BaseImapClient();

        /**
        * @brief       Resolves The Hostname of the IMAP Server to an IPv4 Address.
        * @details     Correctness of This Method Can Be Tested By Command 'nslookup'.
        * @param[in]   hostname The IMAP Server Hostname to Resolve.
        * @retval      A String Containing the IPv4 Address or an Empty String in Case of Failure.
        */
        std::string resolveHostnameToIPv4(const std::string& hostname);

        /**
        * @brief       Generates Tags For Client Requests.
        * @details     Since The Tag is Client-Defined, The Tag Will Look Like 'Axxxxxxxxxxxx', 
        *              Where 'A' is a Fixed Character and The Sequence 'x' is a Ten-Character Hexadecimal Number, 
        *              Incrementing With Each Client's Request.
        * @retval      11-Byte Long String.
        */
        std::string generateTag(void);
};

#endif /* BASE_IMAP_CLIENT_H */