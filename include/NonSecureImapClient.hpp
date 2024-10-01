/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      NonSecureImapClient.hpp
 *  Author:         Tomas Dolak
 *  Date:           28.09.2024
 *  Description:    Implements Non-Secure IMAPv4 Client That Communicates Just Thru TCP/IP With Port 143.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           NonSecureImapClient.hpp
 *  @author         Tomas Dolak
 *  @date           28.09.2024
 *  @brief          Implements Non-Secure IMAPv4 Client That Communicates Just Thru TCP/IP With Port 143.
 * ****************************/

#ifndef NON_SECURE_IMAP_CLIENT_H
#define NON_SECURE_IMAP_CLIENT_H
/************************************************/
/*                  Libraries                   */
/************************************************/
#include "utilities.hpp"
#include "BaseImapClient.hpp"
/************************************************/
/*                  Class Definition            */
/************************************************/
/**
 * @brief Derived Class from BaseImapClient, That Implements Non-Secure Version of IMAP Client.
 */
class NonSecureImapClient : public BaseImapClient
{
    private:
        /* IMAP Server Informations */
        std::string mailbox;        //!< Mailbox to fetch emails from
        std::string outputDir;      //!< Directory to store the emails
        bool        headersOnly;
        bool        newOnly;
    public:

        /**
        * @brief        Class Constructor.
        * @param[in]    mailbox The Name of The Mailbox To Fetch Emails From.
        * @param[in]    outputDir The Directory To Save The Downloaded Emails.
        */
        NonSecureImapClient(const std::string& MailBox, const std::string& OutDirectory, bool HeadersOnly, bool NewOnly);
        
        /**
        * @brief    Connects to the IMAP server.
        * @param[in] serverAddress IMAP server address.
        * @param[in] username IMAP account username.
        * @param[in] password IMAP account password.
        * @retval   SUCCESS If Client Has Connected Successfully To IMAP Server.
        */
        int ConnectImapServer(const std::string& serverAddress, const std::string& username, const std::string& password);

        int SendData(const std::string& data);

        std::string ReceiveData(void);

        int LoginClient(std::string username, std::string password);

        int LogoutClient(void);
        
        int ParseUIDs(std::string response);

        int FetchUIDs(void);
        /**
        * @brief    Fetches Emails From The Connected Mailbox.
        * @details  Downloads Emails From The Specified Mailbox and Saves Them To The Output Directory.
        * @retval   True If Emails Were Successfully Fetched, False Otherwise.
        */
        int FetchEmails(void);

 
        std::string FetchEmailByUID(int uid, bool mode);

        std::string ParseEmail(int uid, std::string email, bool just_headers);

        int SetMailBox(void);

        /**
        * @brief    Disconnects From The IMAP Server.
        * @retval   SUCCESS If Client Was Safely Dissconnected.
        */
        int DisconnectImapServer(void);

        int Run(const std::string& serverAddress, const std::string& username, const std::string& password);
};


#endif /* NON_SECURE_IMAP_CLIENT_H */
