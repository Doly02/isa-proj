/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      utilities.hpp
 *  Author:         Tomas Dolak
 *  Date:           27.09.2024
 *  Description:    Includes Utilities That Are Not Specifically Connected To Any Class.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           utilities.hpp
 *  @author         Tomas Dolak
 *  @date           27.09.2024
 *  @details        Includes Utilities That Are Not Specifically Connected To Any Class.
 * ****************************/

#ifndef UTILITIES_HPP
#define UTILITIES_HPP

/************************************************/
/*                  Libraries                   */
/************************************************/
#include <arpa/inet.h>
#include <sys/socket.h>
#include "definitions.hpp"

#if (DEBUG_ENABLED == 1)
    #include <fstream>
    #include <sys/stat.h>
#endif /* (DEBUG_ENABLED == 1) */

/************************************************/
/*            Definition of Functions           */
/************************************************/
void PrintHelp(void);

bool FileExists(const std::string& filename);

int RemoveFilesMatchingPattern(const std::string& dir_path, const std::string& pattern_prefix, const std::string& extension);

void StoreUIDVALIDITY(int uid_validity,std::string output_dir);

int ReadUIDVALIDITYFile(const std::string& filepath);

std::string GenerateFilename(int uid);

std::string GeneratePathToFile(std::string output_dir, std::string f_name);
/**
    * @brief       Creates The File And Stores The Specified Content in It.
    * @details     None Yet :D
    *
    * @param[in]   content File Content.
    * @param[in]   file_path Path To The File.
    *
    * @retval      void
    */
void StoreEmail(std::string content, std::string file_path);

std::string ParseEmailHeader(std::string header);

std::string ParseEmailBody(std::string body);

/**
* @brief        Checks Whether The String Is IPv4 Address.
* @param str    String With Potential IPv4 Address
* 
* @details      Checks If The String Is IPv4 Address.
* @retval       True If The String Is IPv4 Address.
* @retval       False Otherwise.
*/
bool IsIPv4Address(const std::string& str);

/**
* @brief        Checks Whether The String Is IPv6 Address.
* @param str    String With Potential IPv6 Address.
* 
* @details      Checks If The String Is IPv6 Address.
* @retval       True If The String Is IPv6 Address.
* @retval       False Otherwise.
*/
bool isIPv6Address(const std::string& str);


#endif /* UTILITIES_HPP */