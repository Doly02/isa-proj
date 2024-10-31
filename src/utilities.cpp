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
#include <filesystem>

/************************************************/
/*                  Code                        */
/************************************************/
void PrintHelp(void)
{
    std::cout << "Usage: imapcl server [-p port] [-T [-c certfile] [-C certaddr]] [-n] [-h] -a authfile [-b mailbox] -o outdir\n\n";
    
    std::cout << "Order Of Parameters Is Arbitrary. Parameter Descriptions:\n";

    std::cout << "    server:      The Mandatory Server Name (IP Address Or Domain Name) Of The Desired Resource.\n";
    std::cout << "    -p port:     Optional Parameter That Specifies The Server Port Number. Choose An Appropriate Default Value Depending On The\n";
    std::cout << "                 Specification Of The -T Parameter And Port Numbers Registered By The IANA.\n";
    std::cout << "    -T:          Enables Encryption (IMAPS). If Not Specified, The Unencrypted Version Of The Protocol Will Be Used.\n";
    std::cout << "    -c certfile: Optional Parameter That Specifies A Certificate File Used To Verify The Validity Of The SSL/TLS Certificate\n";
    std::cout << "                 Presented By The Server.\n";
    std::cout << "    -C certaddr: Optional Parameter That Specifies The Directory Where Certificates Will Be Searched To Verify The Validity\n";
    std::cout << "                 Of The SSL/TLS Certificate Presented By The Server. Default Is /etc/ssl/certs.\n";
    std::cout << "    -n: When Used, Only New Messages Will Be Read.\n";
    std::cout << "    -h: When Used, Only Email Headers Will Be Downloaded.\n";
    std::cout << "    -a authfile: Mandatory Parameter That Refers To The File With Authentication Data (LOGIN Command).\n";
    std::cout << "    -b mailbox:  Optional Parameter That Specifies The Name Of The Mailbox To Be Used On The Server. Default Is INBOX.\n";
    std::cout << "    -o outdir:   Mandatory Parameter That Specifies The Output Directory Where The Downloaded Messages Will Be Saved.\n\n";

    std::cout << "Examples Of Usage:\n";
    std::cout << "    ./imapcl eva.fit.vutbr.cz -p 143 -a ~/auth_file.txt -b INBOX -o ~/output/\n";
    std::cout << "    ./imapcl eva.fit.vutbr.cz -T -c ~/cert.pem -C /etc/ssl/certs -a ~/auth_file.txt -b INBOX -o ~/output/\n";
    std::cout << "    ./imapcl eva.fit.vutbr.cz -n -h -a ~/auth_file.txt -o ~/output/\n";
}

bool FileExists(const std::string& filename) 
{
    struct stat buffer;
    return (0 == stat(filename.c_str(), &buffer));
}

int CreateDirectoryIfNotExists(const std::string& dir_path)
{
    std::filesystem::path path(dir_path);

    if (!std::filesystem::exists(path)) 
    {
        if (std::filesystem::create_directories(path)) 
        {
            return SUCCESS;
        } 
        else 
        {
            std::cerr << "Error: Failed To Create Directory: " << dir_path << std::endl;
            return OUTPUT_DIR_NOT_CREATED;
        }
    }
    return SUCCESS;
}

int RemoveFilesMatchingPattern(const std::string& dir_path, const std::string& pattern_prefix, const std::string& extension)
{
    try 
    {
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) 
        {
            std::string file_name = entry.path().filename().string();

            /* Check If The File Starts With The Pattern_Prefix and Ends With The Extension */
            if (0 == file_name.find(pattern_prefix) && extension == file_name.substr(file_name.length() - extension.length())) 
            {
                std::filesystem::remove(entry.path());
            }
        }
        return SUCCESS;
    }
    catch (const std::filesystem::filesystem_error& e) 
    {
        std::cerr << "ERR: " << e.what() << std::endl;
        return REMOVAL_OF_EMAILS_FAILED;
    }
}

void StoreUIDVALIDITY(int uid_validity, const std::string& mailbox, const std::string& output_dir)
{
    std::string filename = UIDVALIDITY_FILE;
    std::string path = output_dir + "/" + filename;
    std::ifstream file_in(path);
    std::vector<std::string> lines;
    bool mailbox_found = false;

    /* Load The Content of .uidvalidity File If Exists And Store The Content To Vector */
    if (file_in.is_open()) 
    {
        std::string line;
        while (std::getline(file_in, line)) 
        {
            /* Check If File Contains Line For Current Used Mailbox */
            if (line.rfind(mailbox + "=", 0) == 0)
            {
                lines.push_back(mailbox + "=" + std::to_string(uid_validity));
                mailbox_found = true;
            } 
            else 
            {
                lines.push_back(line);
            }
        }
        file_in.close();
    }

    /* If Mailbox Was Not Found In The File, Create New Record */
    if (!mailbox_found) 
    {
        lines.push_back(mailbox + "=" + std::to_string(uid_validity));
    }

    /* Open File For Write And Write All The Lines */
    std::ofstream file_out(path);
    if (!file_out.is_open()) 
    {
        std::cerr << "ERR: Unable To Open File: " << path << std::endl;
        return;
    }

    for (const auto& line : lines) 
    {
        file_out << line << "\n";
    }
    file_out.close();
}

int ReadUIDVALIDITYFile(const std::string& filepath, const std::string& mailbox)
{
    std::ifstream file(filepath);   /* Path To .uidvalidity File */
    if (!FileExists(filepath)) 
    {
        return UIDVALIDITY_FILE_NOT_FOUND; /* No Need To Check UIDVALIDITY Further If File Is Missing. */
    }

    if (!file.is_open()) 
    {
        std::cerr << "ERR: Unable To Open The .uidvalidity File." << std::endl;
        return UIDVALIDITY_FILE_ERROR;
    }

    std::string line;
    while (std::getline(file, line)) 
    {
        size_t equal_pos = line.find('=');
        if (equal_pos == std::string::npos) 
        {
            return 0;
        }

        std::string line_mailbox = line.substr(0, equal_pos);
        std::string uidvalidity_str = line.substr(equal_pos + 1);

        if (line_mailbox == mailbox) 
        {
            try 
            {
                size_t pos;
                int uidvalidity_num = std::stoi(uidvalidity_str, &pos);

                if (pos != uidvalidity_str.length()) 
                {
                    return UIDVALIDITY_FILE_NOT_FOUND;
                }

                return uidvalidity_num;
            }
            catch (const std::invalid_argument& e) 
            {
                return UIDVALIDITY_FILE_NOT_FOUND;
            }
            catch (const std::out_of_range& e) 
            {
                return UIDVALIDITY_FILE_NOT_FOUND; 
            }
        }
    }
    return UIDVALIDITY_FILE_NOT_FOUND;
}

std::string GenerateFilename(int uid, std::string mailbox, bool h_only, bool n_only)
{
    std::string suffix = EMPTY_STR;

    if (true == n_only && true == h_only)          
    {
        suffix = "_new_header_";
    }
    else if (true == h_only && false == n_only)
    {
        suffix = "_header_";
    }
    else if (false == h_only && true == n_only)
    {
        suffix = "_new_";
    }
    else
    {
        suffix = EMPTY_STR;
    }
    
    return ("MSG_" + mailbox + "_" + std::to_string(uid) + suffix + OUTPUT_FILE_FORMAT);
}

std::string GeneratePathToFile(std::string output_dir, std::string f_name)
{
    return (output_dir + f_name);
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
    header.erase(0, header.find("\n") + 1);
    return header;
}

std::string ParseEmailBody(std::string body, std::string tag)
{
    std::string delete_part = EMPTY_STR;
    try
    {
        std::regex reg_expression("(" + tag + "\\s+.*(?=OK)[.|\\s\\S]*)");
        std::smatch match;
        if (regex_search(body, match, reg_expression))
        {
            delete_part = match.str(0);
        }
    
        body = body.substr(0, body.size() - delete_part.size());
        body.erase(0, body.find("\r\n") + 1);
        body = body.substr(0, body.size()-5);
        return body; 
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
