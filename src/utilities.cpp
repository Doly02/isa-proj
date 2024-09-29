/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      utilities.cpp
 *  Author:         Tomas Dolak
 *  Date:           27.09.2024
 *  Description:    Includes Utilities That Are Not Specifically Connected To Any Class.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           utilities.cpp
 *  @author         Tomas Dolak
 *  @date           27.09.2024
 *  @details        Includes Utilities That Are Not Specifically Connected To Any Class.
 * ****************************/

/************************************************/
/*                  Libraries                   */
/************************************************/
#include "../include/utilities.hpp"


/************************************************/
/*                  Code                        */
/************************************************/
void PrintHelp(void)
{
    printf("Usage: ...\n");
    printf("TODO\n");
}

bool FileExists(const std::string& filename) 
{
    struct stat buffer;
    return (0 == stat(filename.c_str(), &buffer));
}

std::string GenerateFilename(int uid)
{
    return ("MSG_" + std::to_string(uid) + OUTPUT_FILE_FORMAT);
}

std::string GeneratePathToFile(std::string output_dir, std::string f_name)
{
    return (output_dir + "/" + f_name);
}


void StoreEmail(std::string content, std::string file_path)
{
    std::ofstream file(file_path);
    file << content;
}

std::string ParseEmailHeader(std::string header)
{
    std::string delete_part = EMPTY_STR;
    try {
        std::regex reg_expression("(\\r\\n\\)[.|\\s\\S]*)");
        std::smatch match;
        if(regex_search(header, match, reg_expression) && (1 < match.size())) {
            delete_part = match.str(1);
        } 
    } 
    catch(std::regex_error& e) {
        return BAD_RESPONSE;
    }
    header = header.substr(0, header.size() - delete_part.size());
    header.erase(0, header.find("\r\n") + 1);
    return header.erase(0, header.find("\n") + 1);
}

std::string ParseEmailBody(std::string body)
{
    std::string delete_part = EMPTY_STR;
    try
    {
        std::regex reg_expression("(.*(?=OK FETCH completed)[.|\\s\\S]*)");
        std::smatch match;
        if (regex_search(body, match, reg_expression) && (1 < match.size()))
        {
            delete_part = match.str(1);
        }
        body = body.substr(0, body.size() - delete_part.size());
        body.erase(0, body.find("\r\n") + 1);
        return body.erase(0, body.find("\n") + 1);
    }
    catch(std::regex_error& e) {
        return BAD_RESPONSE;
    }

    body = body.substr(0, body.size() - delete_part.size());
    body.erase(0, body.find("\r\n") + 1);
    body = body.substr(0, body.size() - 5);
    return body;
}

bool IsIPv4Address(const std::string& str)
{
    struct sockaddr_in sa;
    return (0 != inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)));    
}

bool isIPv6Address(const std::string& str) 
{
    struct sockaddr_in6 sa;  // Structure of IPv6 Address
    return (0 != inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr)));
}
