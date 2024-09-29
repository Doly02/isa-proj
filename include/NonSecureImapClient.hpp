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
        
        std::string rx_data;        //<! Buffer For Server Response

    public:

        /**
        * @brief        Class Constructor.
        * @param[in]    mailbox The Name of The Mailbox To Fetch Emails From.
        * @param[in]    outputDir The Directory To Save The Downloaded Emails.
        */
        NonSecureImapClient(const std::string& mailbox, const std::string& outputDir);
        
        /**
        * @brief    Connects to the IMAP server.
        * @param[in] serverAddress IMAP server address.
        * @param[in] username IMAP account username.
        * @param[in] password IMAP account password.
        * @retval   True on successful connection, False otherwise.
        */
        bool ConnectImapServer(const std::string& serverAddress, const std::string& username, const std::string& password);

        int SendData(const std::string& data);

        int ReceiveData(void);

        int LoginClient(std::string username, std::string password);

        int LogoutClient(void);
        /**
        * @brief    Fetches Emails From The Connected Mailbox.
        * @details  Downloads Emails From The Specified Mailbox and Saves Them To The Output Directory.
        * @retval   True If Emails Were Successfully Fetched, False Otherwise.
        */
        bool FetchEmails(void);

        int SetMailBox(std::string box);

        /**
        * @brief    Disconnects From The IMAP Server.
        * @retval   True on Successful Disconnection, False Otherwise.
        */
        bool DisconnectImapServer(void);
};


#endif /* NON_SECURE_IMAP_CLIENT_H */

