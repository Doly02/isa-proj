/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      ClientConfig.cpp
 *  Author:         Tomas Dolak
 *  Date:           22.09.2024
 *  Description:    Implements The Client Configuration Settings Based On The Parameters With Which The Program Was Launched.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           ClientConfig.cpp
 *  @author         Tomas Dolak
 *  @date           22.09.2024
 *  @brief          Implements The Client Configuration Settings Based On The Parameters With Which The Program Was Launched.
 * ****************************/

/************************************************/
/*                  Libraries                   */
/************************************************/
#include "../include/ClientConfig.hpp"

/************************************************/
/*             Class Implementation             */
/************************************************/
ImapClientConfig::ImapClientConfig()
    : mode(NON_SECURE), 
    port(0), 
    server(EMPTY_STR), 
    certF(EMPTY_STR), 
    certD(DEFAULT_SSL_CERT_LOC),
    mailboxD(DEFAULT_MAILBOX_DIR),
    outputD(EMPTY_STR),
    justNew(false),
    justHeaders(false),
    authData{ {EMPTY_STR}, {EMPTY_STR} } {}

ImapClientConfig::ImapClientConfig(int argc, char* argv[])   
{
    bool retVal = false;
    mode        = NON_SECURE;
    justHeaders = false;
    justNew     = false;
    retVal = this->ProcessArguments(argc, argv);
    if(SUCCESS == retVal)
    {
        printf("Success!\n");
    }
}

bool ImapClientConfig::GetClientMode()
{
    return mode;
}

std::string ImapClientConfig::GetServerAddress()
{
    return server;
}

std::string ImapClientConfig::GetOutputDirectory()
{
    return outputD;
}

std::string ImapClientConfig::GetMailbox()
{
    return mailboxD;
}

bool ImapClientConfig::GetNewOnly()
{
    return justNew;
}

bool ImapClientConfig::GetHeadersOnly()
{
    return justHeaders;
}


bool ImapClientConfig::ParseArguments(int argc, char* argv[])
{
    int c = 0; 

    
    if (6 > argc || 16 < argc) 
    {
        return false;  
    }
    server = argv[1];

    while (-1 != (c = getopt(argc, argv, "p:c:C:a:b:o:Tnh")))
    {
        switch(c)
        {
            case 'p':   /* Port Option */
                port = std::atoi(optarg);
                break;
            case 'c':   /* Cert. File */
                certD = optarg;
                break;
            case 'C':   /* Cert. Directory */
                certF = optarg;
                break;
            case 'T':   /* If Defined, Configured Client Will Operate in Secure Mode */
                mode = SECURE;
                break;
            case 'a':   /* Auth. File*/
                authF = optarg;
                break;
            case 'b':   /* Defines Mail Box Directory*/
                mailboxD = optarg;
                break;
            case 'n':   /* Just New Messages Will Be Read */
                justNew = true;
                break;
            case 'h':   /* Just Email Headers Will Be Downloaded */
                justHeaders = true;
                break;
            case 'o':
                outputD = optarg;
                break;
            case '?':
            default:
                return false;
        }
    }

#if (DEBUG_ENABLED == true)

    printf("Choosen Sever: %s, Port: %d\n",server.c_str(), port);

#endif /* (DEBUG_ENABLED == true) */

    return true;
}

int ImapClientConfig::ExtractAuthData(void)  
{
    // Check The Auth. File Path
    std::ifstream in(authF);

#if (DEBUG_ENABLED == true)

    if (!FileExists(authF)) {
        std::cerr << "ERR: The file does not exist at path: " << authF << std::endl;
        return PARSE_CREDENTIALS_FAILED;
    }
#endif /* (DEBUG_ENABLED == true) */


    if (false == in.is_open()) 
    {
        /* TODO: Maybe Return Some Err. Code? */
        printf("ERR: Unable To Open The Authentication File.\n");
        return PARSE_CREDENTIALS_FAILED;
    }

    std::string contents((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>()); /* Read The Context of The Auth. File */

    std::regex auth_file_template(R"(username\s*=\s*(\S+)\s*\npassword\s*=\s*(\S+))");
    std::smatch match;

    if (std::regex_search(contents, match, auth_file_template) && (3 == match.size())) 
    {
        authData.username = match.str(1); 
        authData.password = match.str(2);  
    } 
    else 
    {   /* TODO: Maybe Return Some Err. Code? */
        printf("ERR: Unable To Find Valid Credentials in The File.\n");
        return PARSE_CREDENTIALS_FAILED;
    }

#if(DEBUG_ENABLED == true)

    if (!authData.username.empty()) {  
        std::cerr << "Username: " << authData.username.c_str() << std::endl;
    }

    if (!authData.password.empty()) { 
        std::cerr << "Password: " << authData.password.c_str() << std::endl;
    }

#endif /* (DEBUG_ENABLED == 1) */

    return SUCCESS;
}

bool ImapClientConfig::ProcessArguments(int argc, char* argv[])
{
    /* Parse The Arguments */
    ParseArguments(argc, argv);

    /* Checkout The Credentials & Parse Them */
    if (SUCCESS != ExtractAuthData())
        return (bool)PARSE_CREDENTIALS_FAILED;

    return (bool)SUCCESS;
}