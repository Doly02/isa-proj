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

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
/************************************************/
/*            Definition of Functions           */
/************************************************/
/**
 * @brief       Prints Usage Information For The IMAP Client.
 * 
 * @details     This Function Outputs Detailed Instructions On How To Use The IMAP Client
 *              Including The Supported Parameters, Their Descriptions, And Example Usage.
 *              Function Is Called When User Use Wrong Arguments When Starting The Application.
 */
void PrintHelp(void);

/**
 * @brief       Checks If The Given File Exists.
 * 
 * @param[in]   filename The Path Of The File To Check.
 * 
 * @retval      true If The File Exists.
 * @retval      false If The File Does Not Exist.
 */
bool FileExists(const std::string& filename);

/**
 * @brief       Creates A Directory If It Does Not Exist.
 * 
 * @param[in]   dir_path The Path To The Directory To Be Created.
 * 
 * @return      SUCCESS If The Directory Exists Or Is Created Successfully.
 * @return      OUTPUT_DIR_NOT_CREATED If The Directory Could Not Be Created.
 */
int CreateDirectoryIfNotExists(const std::string& dir_path);

/**
 * @brief       Removes All Files In A Directory That Match The Given Pattern.
 * 
 * @details     This Function Iterates Through All Files In The Given Directory,
 *              Removing Files Whose Name Starts With The Provided Pattern_Prefix
 *              And Ends With The Provided Extension.
 * 
 * @param[in]   dir_path The Path To The Directory To Search In.
 * @param[in]   pattern_prefix The Prefix That The File Names Should Start With.
 * @param[in]   extension The Extension That The File Names Should End With.
 * 
 * @return      SUCCESS If Files Were Removed Successfully.
 * @return      REMOVAL_OF_EMAILS_FAILED If An Error Occurs While Removing The Files.
 */
int RemoveFilesMatchingPattern(const std::string& dir_path, const std::string& pattern_prefix, const std::string& extension);

/**
 * @brief       Stores The UIDVALIDITY Value To A ,uidvalidity File.
 * 
 * @details     This Function Stores The UIDVALIDITY Value To A File Named
 *              By The UIDVALIDITY_FILE Constant, In The Given Output Directory.
 * 
 * @param[in]   uid_validity The UIDVALIDITY Value To Store.
 * @param[in]   output_dir The Directory Where The File Should Be Saved.
 */
void StoreUIDVALIDITY(int uid_validity, const std::string& mailbox, const std::string& output_dir);

/**
 * @brief       Reads The UIDVALIDITY Value From A File.
 * 
 * @details     This Function Attempts To Read A UIDVALIDITY Value From A File Located At The Given Filepath. 
 *              If The File Does Not Exist Or Contains Invalid Data, An Error Code Is Returned.
 * 
 * @param[in]   filepath The Path To The .uidvalidity File.
 * 
 * @retval      UIDVALIDITY_FILE_NOT_FOUND If The File Does Not Exist.
 * @retval      UIDVALIDITY_FILE_ERROR If An Error Occurs While Reading The File Or If The Content Is Invalid.
 * @retval      The UIDVALIDITY Value If Successfully Read From The File.
 */
int ReadUIDVALIDITYFile(const std::string& filepath, const std::string& mailbox);

/**
 * @brief       Generates A Filename For A Specific Email.
 * 
 * @details     This Function Generates A Filename Based On The Given UID, Mailbox Name
 *              And Program's Arguments '-h' And '-n' Appending The Standard Output File 
 *              Format Extension.
 * 
 * @param[in]   uid The Unique Identifier Of The Email.
 * @param[in]   mailbox The Name Of The Mailbox.
 * @param[in]   h_only If Header Only Argument Is Enabled. 
 * @param[in]   n_only If New Emails Only Argument Is Enabled
 * 
 * @retval      The Generated Filename In The Format MSG_mailbox_uid.log.
 */
std::string GenerateFilename(int uid, std::string mailbox, bool h_only, bool n_only);

/**
 * @brief       Generates The Full Path To A File.
 * 
 * @details     This Function Combines The Output Directory And Filename To Generate The Full Path
 *              Where The File Should Be Stored.
 * 
 * @param[in]   output_dir The Directory Where The File Will Be Stored.
 * @param[in]   f_name The Name Of The File.
 * 
 * @retval      The Full Path To The File.
 */
std::string GeneratePathToFile(std::string output_dir, std::string f_name);

/**
 * @brief       Stores Email Content To A File.
 * 
 * @details     This Function Writes The Given Email Content To A File At The Specified File Path.
 * 
 * @param[in]   content The Content Of The Email To Be Stored.
 * @param[in]   file_path The Path To The File Where The Email Content Will Be Written.
 */
void StoreEmail(std::string content, std::string file_path);

/**
 * @brief       Parses The Email Header.
 * 
 * @details     This Function Parses The Email Header To Extract The Necessary Information.
 *              It Uses A Regular Expression To Identify The End Of The Header Section And
 *              Then Removes Any Unnecessary Parts. The Result Is The Cleaned Email Header.
 * 
 * @param[in]   header The Complete Email Header As A String.
 * 
 * @retval      The Parsed And Cleaned Email Header.
 * @retval      BAD_RESPONSE If A Regex Error Occurs.
 */
std::string ParseEmailHeader(std::string header);

/**
 * @brief       Parses The Email Body.
 * 
 * @details     This Function Parses The Email Body To Remove Any Unwanted Sections Based On The Provided Tag. 
 *              It Uses A Regular Expression To Identify The End Of The Body Content, Specifically Removing Any 
 *              Tag That Matches The Pattern "<tag> OK". The Function Then Returns The Cleaned Email Body.
 * 
 * @param[in]   body The Complete Email Body As A String.
 * @param[in]   tag The Tag Used To Identify The End Of The Body Content.
 * 
 * @retval      The Parsed And Cleaned Email Body.
 * @retval      BAD_RESPONSE If A Regex Error Occurs.
 */
std::string ParseEmailBody(std::string body, std::string tag);

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

/**
 * @brief       Clear Rx Buffer on Specific Socket on Standard TCP/IP Connection.
 * @param[in]   sockfd Socket File Descriptor.
 * 
 * @retval      void
 */
void ClearSocketBuffer(int sockfd);

/**
 * @brief       Clear Rx Buffer on Specific Socket on Standard SSL/TLS Connection.
 * @param[in]   sockfd Socket File Descriptor.
 * 
 * @retval      void
 */
void ClearSSLBuffer(SSL* ssl);

#endif /* UTILITIES_HPP */