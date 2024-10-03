/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      ClientConfig.hpp
 *  Author:         Tomas Dolak
 *  Date:           22.09.2024
 *  Description:    Implements The Client Configuration Settings Based On The Parameters With Which The Program Was Launched.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           ClientConfig.hpp
 *  @author         Tomas Dolak
 *  @date           22.09.2024
 *  @brief          Implements The Client Configuration Settings Based On The Parameters With Which The Program Was Launched.
 * ****************************/

#ifndef CLIENTCONFIG_HPP
#define CLIENTCONFIG_HPP
/************************************************/
/*                  Libraries                   */
/************************************************/
#include <getopt.h>
#include <string>
#include "definitions.hpp"
#include "utilities.hpp"
/************************************************/
/*                  Class Definition            */
/************************************************/
class ImapClientConfig
{
    private:
        /*TODO Add Private Properieties. */
        bool            mode;           //<! IMAP Client Mode (Secure/Non-Secure).
        int             port;           //<! Server's Port (Only Two Options - 143/993).
        std::string     server;         //<! IPv4 Address or Hostname of IMAPv4 Server (FIXME: IMAP Server Can Also Have Just IPv6 Address).
        std::string     certF;          //<! Specifies File In Which Cert. is Stored.
        std::string     certD;          //<! Specifies Dir In Which Cert. is Stored.
        std::string     authF;          //<! Defines Authentication File.
        std::string     mailboxD;       //<! Specifies Mailbox Directory.
        std::string     outputD;
        bool            onlyNew;        //<! Only New Emails Will Be Read.
        bool            onlyHeaders;    //<! Just Header Will Be Downloaded.


    public:
        /*TODO Add Public Properieties. */
        Credentials_t   authData;

        ImapClientConfig();
        /**
         * @brief       Constructor.
         * @param       argc Number of Arguments
         * @param       argv Array of Arguments
         * 
         * Constructor For Arguments Class
        */
        ImapClientConfig(int argc, char* argv[]);   

        bool GetClientMode(void);

        std::string GetServerAddress(void);

        std::string GetOutputDirectory(void);

        std::string GetMailbox(void);

        std::string GetCertFile(void);

        std::string GetCertDirectory(void);

        bool GetOnlyNew(void);

        bool GetOnlyHeaders(void);

        int GetPort(void);

        int ProcessArguments(int argc, char* argv[]);

        /**
         * @brief       Parses Command Line Arguments.
         * @param[in]   argc Number of Arguments
         * @param[in]   argv Array of Arguments
         * 
         * @details     Parsed Arguments Are Stored Into Class Attributes.
         * @return      Arguments Are Not Valid, Exits With Error Code 1, Otherwise Returns 0.
        */         
        int ParseArguments(int argc, char* argv[]);   

        /**
        * @brief        Extracts Username and Password From an Authentication File.
        * 
        * @details      This Function Attempts to Extract The Credentials (Username and Password)
        *               From The Specified Auth. file. It Uses Regular Expressions To Search For The Format:
        * 
        * @code
        * username = <username>
        * password = <password>
        * @endcode
        * 
        * User Has to Provide Full Path to The File With Credentials By Parameter '-p'.
        * @throw        std::runtime_error If The Auth. File Cannot Be Opened or 
        *               If Valid Credentials (Username and Password) Are Not Found in 
        *               The Expected Format. TODO: Update Comment!
        */
        int ExtractAuthData(void);
};

#endif /* CLIENTCONFIG_HPP */