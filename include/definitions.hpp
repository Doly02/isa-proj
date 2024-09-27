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
/*             Macro Definitions                */
/************************************************/
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>

/**
 * @brief Definitions of Return Codes.
 */
#define NO_IP_ADDR_FOUND

/**
 * @brief Secure IMAP Client Mode. Communicate The SSL/TLS.
 */
#define SECURE                      (true)
/**
 * @brief Non-Secure IMAP Client Mode. Communicate Just Thru TCP/IP.
 */
#define NON_SECURE                  (false)

#define DEFAULT_SSL_CERT_LOC        "/etc/ssl/certs"
#define DEFAULT_MAILBOX_DIR         "INBOX"

#define EMPTY_STR                   ""

#define DEBUG_ENABLED               (true)

/**
 * @brief Return Codes For The Program.
 */
typedef enum 
{
    SUCCESS     = 0u,    /**< Success */
    ERROR       = 1u      /**< Error */

} ReturnCodes;

/**
 * @brief Structure For The Email Message.
 */
typedef struct 
{
    std::vector<std::string> from;          //<! Sender Address.
    
    std::vector<std::string> to;            //<! Receiver Address.
    std::vector<std::string> cc;            //<! Carbon Copy.
    std::vector<std::string> bcc;           //<! Blind Carbon Copy.

    std::string subject;                    //<! Subject of The Email.
    std::string date;                       //<! Date
    std::string message_id;                 //<! Message-ID
    std::vector<std::string> content_type;  //<! Content-Type

} MailHeader_t;

/**
 * @brief Structure of The Email Body.
 */
typedef struct 
{
    std::vector<std::string> content;           //<! Mail Content.

} MailBody_t;

/**
 * @brief Structure of The Email.
 */
typedef struct
{
    MailHeader_t    header;
    MailBody_t      body;                   //<! Header of The Mail.
} Mail_t;

#endif /* DEFINITIONS_HPP */