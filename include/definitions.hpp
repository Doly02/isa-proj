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

#define DEBUG_ENABLED               (true)

/**
 * @brief Return Codes For The Program.
 */
typedef enum 
{
    SUCCESS     = 0u,    /**< Success */
    ERROR       = 1u      /**< Error */

} ReturnCodes;

#endif /* DEFINITIONS_HPP */