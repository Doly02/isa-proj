/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      definitions.hpp
 *  Author:         Tomas Dolak
 *  Date:           22.09.2024
 *  Description:    Includes Definitions For IMAP Client.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           definitions.hpp
 *  @author         Tomas Dolak
 *  @date           22.09.2024
 *  @brief          Includes Definitions For IMAP Client.
 * ****************************/

#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

/************************************************/
/*                  Libraries                   */
/************************************************/
#include <string>
#include <cstring>          /*<! For memset() */
#include <vector>
#include <cstdio>
#include <iostream>         /*<! For 'cerr' Output.*/
#include <fstream>          /*<! For std::ifstream */
#include <regex>

#include <sys/socket.h>     /*<! Hostname Relevant. */
#include <sys/stat.h>       /*<! For FileExists()   */
#include <netinet/in.h>
#include <arpa/inet.h>      /*<! Hostname Relevant. */
#include <netdb.h>          /*<! For getaddrinfo() + Hostname Relevant.*/

/************************************************/
/*             Macro Definitions                */
/************************************************/
/**
 * @brief   Rx Buffer Size For Receiving Data From IMAP Server.
 * @details 1024 Bytes Should Be Enough.
 */
static constexpr int RX_BUFFER_SIZE = 2048; /* RX Buffer = 1024B */

/**
 * @defgroup ReturnCodes Definitions of Return Codes
 * @brief This Group Contains The Return codes Used By The Application.
 * @{
 */

/**
 * @brief Output Directory Could Not Be Created.
 */
#define OUTPUT_DIR_NOT_CREATED          (-7)

/**
 * @brief .uidvalidity File Not Found.
 */
#define UIDVALIDITY_FILE_NOT_FOUND      (-6)

/**
 * @brief Error With .uidvalidity File (Could Not Be Opened, Invalid Format, Out of Range of INT).
 */
#define UIDVALIDITY_FILE_ERROR          (-5)

/**
 * @brief Return Code When Creating a Connection With IMAP Server Failed.
 */
#define CREATE_CONNECTION_FAILED        (-4)

/**
 * @brief Return Code When SSL Cert. Verification Failed.
 */
#define SSL_CERT_VERIFICATION_FAILED    (-3)

/**
 * @brief Return Code When Fetching Email By UID Failed.
 */
#define FETCH_EMAIL_FAILED              (-2)

/**
 * @brief Operation was successful.
 */
#define SUCCESS                         (0u)

/**
 * @brief No IPv4 Address was Found.
 */
#define NO_IP_ADDR_FOUND                (1u)

/**
 * @brief Return Code When Parsing Program's Arguments Failed.
 */
#define PARSE_ARGUMENTS_FAILED          (2u)

/**
 * @brief Return Code When Parsing Credentials Failed.
 */
#define PARSE_CREDENTIALS_FAILED        (3u)

/**
 * @brief Server Sent an Unknown Response.
 */
#define SERVER_UNKNOWN_RESPONSE         (4u)

/**
 * @brief Transmission of Data Failed.
 */
#define TRANSMIT_DATA_FAILED            (5u)

/**
 * @brief Reception of Data Failed.
 */
#define RECEIVE_DATA_FAILED             (6u)

/**
 * @brief Expected Server's Response Was Not Found (e.g. "<tag> OK",...).
 */
#define RESPONSE_NOT_FOUND              (7u)

/**
 * @brief Parsing of Regular Expression failed.
 */
#define PARSE_BY_REGEX_FAILED           (8u)

/**
 * @brief None UIDs Were Received From The IMAP Server.
 */
#define NON_UIDS_RECEIVED               (9u)

/**
 * @brief Continue Receiving More Data.
 */
#define CONTINUE_IN_RECEIVING           (10u)

/**
 * @brief Undefined State Encountered.
 */
#define UNDEFINED_STATE                 (11u)

/**
 * @brief Unable to Receive UIDVALIDITY From The Server.
 */
#define UID_VALIDITY_ERROR_IN_RECV      (14u)
/**
 * @brief Failed to Remove Emails When UIDVALIDITY Does Not Match.
 */
#define REMOVAL_OF_EMAILS_FAILED        (15u)

/**
 * @brief Error During Receiving of Server's Response.
 * @details Special Return Code For Methods That Returns String.
 */
#define BAD_RESPONSE                    "Bad Response :("




/** @} */ // End of ReturnCodes group



#define NOT_IMPLEMENTED                 throw std::runtime_error("Not Implemented Yet!")

#define EMPTY_STR                       ""

#define WHOLE_MESSAGE                   (true)
#define JUST_HEADER                     (false)

#define DEBUG_ENABLED                   (false)

#define OUTPUT_FILE_FORMAT              ".log"

/***************************************************************/
/*                  Specific For Non-Secure Mode               */
/***************************************************************/

/**
 * @brief Secure IMAP Client Mode. Communicate The SSL/TLS.
 */
#define SECURE                      (true)
/**
 * @brief Default Port for Secure Mode If Client Not Specify.
 */
#define PORT_SECURE                 (993)
/**
 * @brief   Define Timeout For Receiving Data From IMAP Server. Time in [s].
 * @details Used Only for Non-Secure Client Mode.
 */
#define TIMEOUT_NON_SECURE          (20)

/***************************************************************/
/*                  Specific For Secure Mode                   */
/***************************************************************/
/**
 * @brief Non-Secure IMAP Client Mode. Communicate Just Thru TCP/IP.
 */
#define NON_SECURE                  (false)
/**
 * @brief Default Port for Non-Secure Mode If Client Not Specify.
 */
#define PORT_NON_SECURE             (143)
/**
 * @brief   Define Timeout For Receiving Data From IMAP Server. Time in [s].
 * @details Used Only for Secure Client Mode.
 */
#define TIMEOUT_SECURE              (30)

#define DEFAULT_SSL_CERT_LOC        "/etc/ssl/certs"
#define DEFAULT_MAILBOX_DIR         "INBOX"
#define UIDVALIDITY_FILE            ".uidvalidity.txt"

typedef struct 
{
    std::string username;          
    std::string password;           

} Credentials_t;

typedef enum
{
    /* Responses To Login Command */
    LOGIN = 0u,                     //<! login completed, now in authenticated state
    CONNECT,
    LOGOUT,  
    SEARCH,                         //<! search cmd for fetch of the UIDs.
    FETCH,
    SELECT,
    DEFAULT,
} State_t;

#endif /* DEFINITIONS_HPP */