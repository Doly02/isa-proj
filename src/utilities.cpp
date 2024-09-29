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

std::string GeneratePathToFile(std::string output_dir, std::string f_name)
{
    return (output_dir + "/" + f_name);
}


int StoreEmail(std::string content, std::string file_path)
{
    std::ofstream file(file_path);
    file << content;
    return SUCCESS;
}

std::string ParseEmailHeader(std::string header)
{
    std::string delete_part = EMPTY_STR;
    try {
        regex reg_expression("(\\r\\n\\)[.|\\s\\S]*)");
        smatch match;
        if(regex_search(message, match, reg_expression) && (1 < match.size())) {
            delete_part = match.str(1);
        } 
    } 
    catch(regex_error& e) {
        new BAD_RESPONSE;
    }
}

std::string ParseEmailBody(std::string body)
{
    std::string delete_part = EMPTY_STR;
    try
    {
        regex reg_expression("(.*(?=OK FETCH completed)[.|\\s\\S]*)");
        smatch match;
        if (regex_search(body, match, reg_expression) && (1 < match.size()))
        {
            delete_part = match.str(1);
        }
        body = body.substr(0, body.size() - delete_part.size());
        body.erase(0, body.find("\r\n") + 1);
        return body.erase(0, body.find("\n") + 1);
    }
    catch(regex_error& e) {
        return BAD_RESPONSE;
    }

    body = body.substr(0, body.size() - delete_part.size());
    body.erase(0, body.find("\r\n")+1);
    body = body.substr(0, body.size()-5);
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
