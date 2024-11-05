/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      BaseImapClient.cpp
 *  Author:         Tomas Dolak
 *  Login:          xdolak09
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
        unsigned long long  mCurrentTagValue;
    
    public:
        State_t             curr_state;
        int                 sockfd;         //!< Socket File Descriptor for IMAP Connection.
        std::vector<int>    vec_uids;
        /**
        * @brief Class Constructor.
        */
        BaseImapClient();
        
        /**
        * @brief Class Destructor.
        */
        ~BaseImapClient();
        
        /**
        * @brief       Generates Tags For Client Requests.
        * @details     Since The Tag is Client-Defined, The Tag Will Look Like 'Axxxxxxxxxxxx', 
        * Where 'A' is a Fixed Character and The Sequence 'x' is a Ten-Character Hexadecimal Number, 
        * Incrementing With Each Client's Request.
        * 
        * @retval      11-Byte Long String.
        */
        std::string GenerateTag(void);

        /**
         * @brief       Generates A Unique Tag For Each IMAP Command.
         * 
         * @details     This Method Generates A Unique Tag Using A Hexadecimal Value For Each Command Sent To The IMAP Server.
         * It Uses The Member Variable `mCurrentTagValue` To Create The Tag And Increments It After Each Use.
         * 
         * @return      A String Representing The Tag In The Format "Axxxxxxxxxx" Where 'x' Is A Hexadecimal Digit.
         */
        std::string GetTag(void);
        /**
        * @brief       Resolves the Hostname of The Server to an IP Address (tries IPv4 First, Then IPv6).
        * @details     The Function Uses getaddrinfo() To Resolve The Hostname To An IP Address. 
        * It First Attempts To Resolve The Address To an IPv4 Address. 
        * If No IPv4 Address is Found, Method Tries To Resolve an IPv6 Address.
        * The Correctness of This Method Can Be Tested Using The Command 'nslookup' or 'dig'.
        * The Method Supports Both - IPv4 and IPv6 Addresses.
        * 
        * @param[in]   hostname The Server Hostname to Resolve.
        * @param[in]   port The Port Number as a String (e.g., "143" for IMAP).
        * @retval      A String Containing the Resolved IP Address (Either IPv4 or IPv6), or An Empty String in Case of Failure.
        */
        std::string ResolveHostnameToIP(const std::string& hostname, const std::string& port);


        /**
         * @brief       Prints The Number Of Stored Messages.
         * 
         * @details     This Method Prints Information About The Number Of Stored Messages, Taking Into Account Whether The Client
         * Is Downloading Only New Messages Or Only Headers. The Output Differs Based On The Combination Of Flags.
         * 
         * @param[in]   num The Number Of Stored Messages.
         * @param[in]   new_only If True, Only New Messages Are Stored.
         * @param[in]   headers_only If True, Only Headers Are Stored.
         */
        void PrintNumberOfMessages(int num, bool new_only, bool headers_only);

        /**
         * @brief       Determines If The End Of The IMAP Server's Response Has Been Reached.
         * 
         * @details     This Method Analyzes The Buffer To Check For The End Of The IMAP Server's Response. It Does So By Checking
         * For A Specific Tag And The Resulting Status (OK, NO, or BAD). The Current Client State (LOGIN, LOGOUT, SEARCH, etc.)
         * Determines The Expected Ending Of The Response.
         * 
         * @param[in]   buff The Received Buffer Containing The Server's Response.
         * 
         * @retval SUCCESS If The End Of The Response Is Found And The Command Was Successful (OK).
         * @retval TRANSMIT_DATA_FAILED If The Response Indicates A Failure (NO or BAD).
         * @retval CONTINUE_IN_RECEIVING If The End Of The Response Has Not Been Reached Yet.
         * @retval UNDEFINED_STATE If The Current State Is Undefined.
         */
        int FindEndOfResponse(std::string buff);
};  

#endif /* BASE_IMAP_CLIENT_H */