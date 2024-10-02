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
        std::string mailbox;        //!< Mailbox to Fetch Emails From
        std::string outputDir;      //!< Directory to Store the Emails
        bool        headersOnly;
        bool        newOnly;
    public:
        int         port;           //!< Port That Will Be Used For Connection With IMAP Client

        /**
        * @brief        Class Constructor.
        * @param[in]    mailbox The Name of The Mailbox To Fetch Emails From.
        * @param[in]    outputDir The Directory To Save The Downloaded Emails.
        */
        NonSecureImapClient(const std::string& MailBox, const std::string& OutDirectory, bool HeadersOnly, bool NewOnly);
        
        /**
        * @brief        Connects to the IMAP server.
        * @param[in]    serverAddress IMAP server address.
        * @param[in]    username IMAP account username.
        * @param[in]    password IMAP account password.
        * @retval SUCCESS If Client Has Connected Successfully To IMAP Server.
        */
        int ConnectImapServer(const std::string& serverAddress, const std::string& username, const std::string& password);

        /**
         * @brief       Sends Data to the IMAP Server Through a Non-Secure Connection.
         *
         * @details     This Function Appends the Standard "\r\n" Newline Sequence to The Given Data 
         * and Transmits It to The IMAP Server Using the previously Established Socket Connection.
         * It Ensures That The Full Message is Sent and Returns Appropriate Error Codes 
         * in Case of Transmission Failure.
         *
         * @param[in]   data The Data (Usually an IMAP Command) to Be Sent to The Server.
         *
         * @retval SUCCESS If All Data Has Been Successfully Transmitted to The Server.
         * @retval TRANSMIT_DATA_FAILED If There Was an Error During the Transmission Process
         * or If Not All Data Could Be Transmitted.
         */
        int SendData(const std::string& data);

        /**
         * @brief       Receives Data From The IMAP Server Through a Non-Secure Connection.
         *
         * @details Sets Up a Socket Timeout and Listens For Data From The IMAP Server. 
         * Accumulates The Received Data In a Buffer and Checks For The End of The Server's Response. 
         * If a Timeout or Error Occurs, Appropriate Error Messages Are Logged, and Error Codes Returned.
         *
         * @retval rx_data The Complete Response Received From The Server If The Transmission is Successful.
         * @retval BAD_RESPONSE If an Error Occurs During The Transmission Process or If an Invalid Response is Received.
         * @retval EMPTY_STR If The Reception Fails Due to Timeout or Other Socket-Related Errors.
         */
        std::string ReceiveData(void);

        int LoginClient(std::string username, std::string password);

        /**
         * @brief       Sends a LOGOUT Command to The IMAP Server and Handles The Response.
         *
         * @details     This Function Sends a LOGOUT Command and Waits For The Server's Response. 
         * If The Server Responds Correctly, The Client is Logged Out. 
         * In Case of Transmission or Reception Errors, Error Codes are Returned.
         *
         * @retval SUCCESS If The LOGOUT Command is Sent and The Server Responds Correctly.
         * @retval TRANSMIT_DATA_FAILED If The LOGOUT Command Fails to Send.
         * @retval RECEIVE_DATA_FAILED If The Server's LOGOUT Response is Not Received or Invalid.
         */
        int LogoutClient(void);
        
        /**
         * @brief       Parses UIDs fFrom The IMAP Server's SEARCH Response.
         *
         * @details     This Function Extracts UIDs From The Server's Response To a UID SEARCH Command, 
         * Removing Unnecessary Parts of The Response and Converting The UIDs Into Integers. 
         * If No UIDs Are Found or a Regex Error Occurs, an Appropriate Error Code Is Returned.
         *
         * @param[in]   response The Server's Response To The UID SEARCH Command.
         *
         * @retval SUCCESS If UIDs Are Successfully Parsed And Stored.
         * @retval PARSE_REGEX_FAILED If a Regex Error Occurs During Parsing.
         * @retval NON_UIDS_RECEIVED If No UIDs Are Found in The Response.
         */    
        int ParseUIDs(std::string response);

        /**
         * @brief       Sends a Request To Fetch UIDs From The IMAP Server.
         *
         * @details     This Function Sends a UID SEARCH Command To The Server, Requesting Either All 
         * or Unseen UIDs Based on The newOnly Flag. It Receives The Server's Response and Attempts 
         * to Parse The UIDs. Errors Are Handled If Data Can't Be Sent, Received, or Parsed.
         *
         * @retval SUCCESS If UIDs Are Successfully Fetched and Parsed.
         * @retval TRANSMIT_DATA_FAILED If Sending The UID SEARCH Command Fails.
         * @retval RECEIVE_DATA_FAILED If Receiving The Response From The Server Fails.
         * @retval NON_UIDS_RECEIVED If No UIDs Are Found in The Response or Parsing Fails.
         */
        int FetchUIDs(void);
        
        /**
        * @brief    Fetches Emails From The Connected Mailbox.
        * @details  Downloads Emails From The Specified Mailbox and Saves Them To The Output Directory.
        * @retval   True If Emails Were Successfully Fetched, False Otherwise.
        */
        int FetchEmails(void);

        /**
         * @brief       Fetches an Email From The IMAP Server Based on its UID.
         *
         * @details     Function Sends a Request To Fetch an Email By its UID. It Can Either Fetch The 
         * Whole Message or Just The Email Header, Depending on The Mode Specified. The Response is Received 
         * From The Server and Returned As a String.
         *
         * @param[in]   uid The UID of The Email to Fetch.
         * @param[in]   mode A Flag Indicating Whether To Fetch The Whole Message (WHOLE_MESSAGE) or Just The Header (JUST_HEADER).
         *
         * @retval recv_data The Received Email Data If The Fetch is Successful.
         * @retval EMPTY_STR If The Fetch Command Fails or No Data is Received.
         * @retval BAD_RESPONSE If an Invalid Response is Received From The Server.
         */ 
        std::string FetchEmailByUID(int uid, bool mode);

        /**
         * @brief       Parses The Email Content Based on Its UID and Mode (Headers or Full Message).
         *
         * @details     Function Extracts The Email's Header and Optionally Its body, Depending on 
         * Whether Only Headers are Requested. If `just_headers` Is False, It Fetches The Email Body Separately, 
         * Parses It, and Appends It to The Parsed Header Content.
         *
         * @param[in]   uid The UID of The Email to Parse.
         * @param[in]   email The Raw Email Data to Be Parsed.
         * @param[in]   just_headers A Flag Indicating Whether to Parse Only Headers or Both Headers and Body.
         *
         * @retval email_content Parsed Email Content Including Headers and Optionally The Body.
         */
        std::string ParseEmail(int uid, std::string email, bool just_headers);

        /**
         * @brief       Sends a Command To The IMAP Server To Select a Mailbox.
         *
         * @details     This Function Sends The "SELECT" Command to The IMAP Server to Choose a Mailbox For Further Operations.
         * It Waits For The Server's Response and Handles Any Transmission or Reception Errors During The Process.
         *
         * @retval SUCCESS If The Mailbox Is Successfully Set on The Server.
         * @retval TRANSMIT_DATA_FAILED If There Is An Error In Sending The Command.
         * @retval RECEIVE_DATA_FAILED if The Response From The Server Is Invalid or There Is a Timeout.
         */
        int SetMailBox(void);

        /**
        * @brief    Disconnects From The IMAP Server.
        * @retval   SUCCESS If Client Was Safely Disconnected.
        */
        int DisconnectImapServer(void);

        /**
        * @brief    Provides The IMAP Client Functionality.
        * @retval   SUCCESS If Everything Went Well (From LOGIN, SELECT, FETCH to LOGOUT States).
        * @retval   Non-Zero Error Code Otherwise.
        */
        int Run(const std::string& serverAddress, const std::string& username, const std::string& password);
};


#endif /* NON_SECURE_IMAP_CLIENT_H */
