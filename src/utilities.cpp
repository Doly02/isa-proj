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

int RemoveFilesMatchingPattern(const std::string& dir_path, const std::string& pattern_prefix, const std::string& extension)
{
    try 
    {
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) 
        {
            std::string file_name = entry.path().filename().string();

            // Check If The File Starts With The Pattern_Prefix and Ends With The Extension
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

void StoreUIDVALIDITY(int uid_validity,std::string output_dir)
{
    std::string filename =  ".uidvalidity" + std::string(OUTPUT_FILE_FORMAT);
    std::string path = output_dir + "/" + filename;
    std::ofstream file(path);
    
    if (!file.is_open())
    {
        std::cerr << "ERR: Unable To Open File: " << path << std::endl;
        return;
    }
    
    file << uid_validity;
    file.close();
}
/**
 * retval Number from UIDVALIDITY File.
 * retval Non-Positive Values if Error Occurs.
 */
int ReadUIDVALIDITYFile(const std::string& filepath)
{
    std::ifstream file(filepath);   /* Path to .uidvalidity File */
    std::string content;            /* .uidvalidity File Content */

    if (false == FileExists(filepath)) 
    {
        return UIDVALIDITY_FILE_NOT_FOUND; /* No Need To Check UIDVALIDITY No More Just Download All Messages. */
    }

    if (false == file.is_open()) 
    {
        std::cerr << "ERR: Unable To Open The .uidvalidity File." << std::endl;
        return UIDVALIDITY_FILE_ERROR;
    }
    /* Load Content */
    std::getline(file, content);
    try 
    {
        size_t pos;
        int uidvalidity_num = std::stoi(content, &pos);

        /* Check If File Does Not Contain Any Other Characters */
        if (pos != content.length()) 
        {
            std::cerr << "ERR: File contains extra non-numeric characters.\n";
            return UIDVALIDITY_FILE_ERROR; 
        }
        return uidvalidity_num;
    }
    catch (const std::invalid_argument& e)  /* Non-Valid Format */ 
    {
        std::cerr << "ERR: The file does not contain a valid number.\n";
        return UIDVALIDITY_FILE_ERROR;
    }
    catch (const std::out_of_range& e)      /* Out of The Range of Type INT */
    {
        std::cerr << "ERR: The number in the file is out of range.\n";
        return UIDVALIDITY_FILE_ERROR; 
    }
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

void PrintEmail(const Mail_t& mail, const std::string& file_path) {
    std::ostringstream oss;

    // Tisk hlavičky
    if (!mail.header.from.empty()) {
        oss << "From: " << mail.header.from << "\n";
    }
    if (!mail.header.to.empty()) {
        oss << "To: " << mail.header.to << "\n";
    }
    if (!mail.header.cc.empty()) {
        oss << "Cc: " << mail.header.cc << "\n";
    }
    if (!mail.header.bcc.empty()) {
        oss << "Bcc: " << mail.header.bcc << "\n";
    }
    if (!mail.header.reply_to.empty()) {
        oss << "Reply-To: " << mail.header.reply_to << "\n";
    }
    if (!mail.header.subject.empty()) {
        oss << "Subject: " << mail.header.subject << "\n";
    }
    if (!mail.header.date.empty()) {
        oss << "Date: " << mail.header.date << "\n";
    }
    if (!mail.header.message_id.empty()) {
        oss << "Message-ID: " << mail.header.message_id << "\n";
    }
    if (!mail.header.in_reply_to.empty()) {
        oss << "In-Reply-To: " << mail.header.in_reply_to << "\n";
    }
    if (!mail.header.references.empty()) {
        oss << "References: " << mail.header.references << "\n";
    }
    if (!mail.header.mime_version.empty()) {
        oss << "MIME-Version: " << mail.header.mime_version << "\n";
    }
    if (!mail.header.content_type.empty()) {
        oss << "Content-Type: " << mail.header.content_type << "\n";
    }

    // Oddělení hlavičky a těla
    oss << "\n";

    // Tisk těla e-mailu
    for (const auto& line : mail.body.content) {
        oss << line << "\n";
    }

    // Volání funkce pro uložení obsahu do souboru
    StoreEmail(oss.str(), file_path);
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

std::string ParseFieldValue(const std::string& line) {
    size_t colon_pos = line.find(":");
    if (colon_pos != std::string::npos) {
        return line.substr(colon_pos + 2);  // +2 (Jumps Over ": ")
    }
    return "";
}

Mail_t ParseEmailExtended(std::string email)
{
    Mail_t mail;
    std::istringstream iss(email);
    std::string line;
    bool in_headers = true;

    // Parse Headers
    while (std::getline(iss, line) && in_headers) {
        if (line == "\r" || line == "") {
            in_headers = false;  // Jumps To Body, Headers are Over
            continue;
        }

        if (line.find("From:") == 0) {
            mail.header.from = ParseFieldValue(line);
        } else if (line.find("To:") == 0) {
            mail.header.to = ParseFieldValue(line);
        } else if (line.find("Cc:") == 0) {
            mail.header.cc = ParseFieldValue(line);
        } else if (line.find("Bcc:") == 0) {
            mail.header.bcc = ParseFieldValue(line);
        } else if (line.find("Reply-To:") == 0) {
            mail.header.reply_to = ParseFieldValue(line);
        } else if (line.find("Subject:") == 0) {
            mail.header.subject = ParseFieldValue(line);
        } else if (line.find("Date:") == 0) {
            mail.header.date = ParseFieldValue(line);
        } else if (line.find("Message-ID:") == 0) {
            mail.header.message_id = ParseFieldValue(line);
        } else if (line.find("In-Reply-To:") == 0) {
            mail.header.in_reply_to = ParseFieldValue(line);
        } else if (line.find("References:") == 0) {
            mail.header.references = ParseFieldValue(line);
        } else if (line.find("MIME-Version:") == 0) {
            mail.header.mime_version = ParseFieldValue(line);
        } else if (line.find("Content-Type:") == 0) {
            mail.header.content_type = ParseFieldValue(line);
        }
    }

    // Parse Email Body
    while (std::getline(iss, line)) {
        mail.body.content.push_back(line);
    }

    return mail;
}

std::string ParseEmailBody(std::string body)
{
    std::string delete_part = EMPTY_STR;
    try
    {
        /* TODO: std::regex_constants::icase For Case Insensitivity 
         * Another Idea -> Use Or ([ | ] in Regex)*/
        std::regex reg_expression("(.*(?=OK FETCH completed)[.|\\s\\S]*)", std::regex_constants::icase);
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
