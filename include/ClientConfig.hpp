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
        
        bool            mode;           //<! IMAP Client Mode (Secure/Non-Secure).
        int             port;           //<! Server's Port (Only Two Options - 143/993).
        std::string     server;         //<! IPv4 Address or Hostname of IMAPv4 Server
        std::string     certF;          //<! Specifies File In Which Cert. is Stored.
        std::string     certD;          //<! Specifies Dir In Which Cert. is Stored.
        std::string     authF;          //<! Defines Authentication File.
        std::string     mailboxD;       //<! Specifies Mailbox Directory.
        std::string     outputD;
        bool            onlyNew;        //<! Only New Emails Will Be Read.
        bool            onlyHeaders;    //<! Just Header Will Be Downloaded.


    public:
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

        /**
         * @brief       Retrieves The Mode Of The IMAP Client (Secure/Non-Secure).
         * 
         * @retval The Mode (true For Secure, false For Non-Secure).
         */
        bool GetClientMode(void);

        /**
         * @brief       Retrieves The Server Address Set In The Configuration.
         * 
         * @retval The Server Address As A String.
         */
        std::string GetServerAddress(void);

        /**
         * @brief       Retrieves The Output Directory Set In The Configuration.
         * 
         * @retval The Output Directory As A String.
         */
        std::string GetOutputDirectory(void);

        /**
         * @brief       Retrieves The Mailbox Set In The Configuration.
         * 
         * @retval      The Mailbox Name As A String.
         */
        std::string GetMailbox(void);

        /**
         * @brief       Retrieves The Certificate File For SSL/TLS Verification.
         * 
         * @retval The Certificate File Path As A String.
         */
        std::string GetCertFile(void);

        /**
         * @brief       Retrieves The Directory Containing SSL/TLS Certificates.
         * 
         * @retval The Certificate Directory Path As A String.
         */
        std::string GetCertDirectory(void);

        /**
         * @brief       Checks Whether Only New Emails Should Be Fetched.
         * 
         * @retval true If Only New Emails Should Be Fetched, false Otherwise.
         */
        bool GetOnlyNew(void);

        /**
         * @brief       Checks Whether Only Email Headers Should Be Fetched.
         * 
         * @retval true If Only Email Headers Should Be Fetched, false Otherwise.
         */
        bool GetOnlyHeaders(void);

        /**
         * @brief       Retrieves The Port Set In The Configuration.
         * 
         * @retval The Port Number As An Integer.
         */
        int GetPort(void);

        /**
         * @brief       Processes Command-Line Arguments And Extracts Authentication Data.
         * 
         * @details     This Method First Parses The Command-Line Arguments Using The 
         * `ParseArguments()` Method. If The Arguments Are Valid, It Then
         * Proceeds To Extract The Authentication Data From The Specified 
         * File Using `ExtractAuthData()`. If Either Step Fails, An Error Code
         * Is Returned.
         * 
         * @param[in]   argc Number Of Command-Line Arguments.
         * @param[in]   argv Array Of Command-Line Arguments.
         * 
         * @retval SUCCESS If The Arguments Are Successfully Parsed And The Authentication Data Is Extracted.
         * @retval PARSE_ARGUMENTS_FAILED If The Argument Parsing Fails.
         * @retval PARSE_CREDENTIALS_FAILED If The Extraction Of Authentication Data Fails.
         */
        int ProcessArguments(int argc, char* argv[]);

        /**
         * @brief       Parses Command Line Arguments.
         * @param[in]   argc Number of Arguments
         * @param[in]   argv Array of Arguments
         * 
         * @details Parsed Arguments Are Stored Into Class Attributes.
         * @return Arguments Are Not Valid, Exits With Error Code 1, Otherwise Returns 0.
        */         
        int ParseArguments(int argc, char* argv[]);   

        /**
        * @brief        Extracts Username and Password From an Authentication File.
        * 
        * @details      This Function Attempts to Extract The Credentials (Username and Password)
        * From The Specified Auth. file. It Uses Regular Expressions To Search For The Format:
        * 
        * @code
        * username = <username>
        * password = <password>
        * @endcode
        * 
        * User Has to Provide Full Path to The File With Credentials By Parameter '-p'.
        * 
        * @throw        std::runtime_error If The Auth. File Cannot  
        * Be Opened or If Valid Credentials (Username and Password) Are Not Found in 
        * Are Not Found in The Expected Format. TODO: Update Comment!
        */
        int ExtractAuthData(void);
};

#endif /* CLIENTCONFIG_HPP */