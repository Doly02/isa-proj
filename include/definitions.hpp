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

#include <sys/socket.h>     /*<!  Hostname Relevant. */
#include <netinet/in.h>
#include <arpa/inet.h>      /*<!  Hostname Relevant. */
#include <netdb.h>          /*<! For getaddrinfo() + Hostname Relevant.*/
/************************************************/
/*             Macro Definitions                */
/************************************************/

/**
 * @brief Definitions of Return Codes.
 */
#define SUCCESS                     (0u)
#define NO_IP_ADDR_FOUND            (1u)
#define PARSE_CREDENTIALS_FAILED    (2u)
#define SERVER_UNKNOWN_RESPONSE     (3u)
#define TRANSMIT_DATA_FAILED        (4u)
#define NOT_IMPLEMENTED             throw std::runtime_error("Not Implemented Yet!")
/**
 * @brief Secure IMAP Client Mode. Communicate The SSL/TLS.
 */
#define SECURE                      (true)
#define PORT_NON_SECURE             (993)

/**
 * @brief Non-Secure IMAP Client Mode. Communicate Just Thru TCP/IP.
 */
#define NON_SECURE                  (false)
#define PORT_SECURE                 (143)

#define DEFAULT_SSL_CERT_LOC        "/etc/ssl/certs"
#define DEFAULT_MAILBOX_DIR         "INBOX"

#define EMPTY_STR                   ""

#define DEBUG_ENABLED               (true)

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

typedef struct 
{
    std::vector<std::string> username;          
    std::vector<std::string> password;           

} Credentials_t;


typedef enum
{
    LOGIN = 0u,
    CONNECT = 1u
} Response_t;

#endif /* DEFINITIONS_HPP */