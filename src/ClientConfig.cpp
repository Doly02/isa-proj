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
    justNew(false),
    justHeaders(false) {}

ImapClientConfig::ImapClientConfig(int argc, char* argv[])   
{
    this->ParseArguments(argc, argv);
}

bool ImapClientConfig::GetClientMode()
{
    return mode;
}


bool ImapClientConfig::ParseArguments(int argc, char* argv[])
{
    int c = 0; 

    
    if (6 > argc || 16 < argc) 
    {
        return false;  
    }
    
    mode = SECURE;
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
            case '?':
            default:
                return false;
        }
    }

#if (DEBUG_ENABLED == 1)

    printf("Choosen Sever: %s, Port: %d\n",server.c_str(), port);

#endif /* (DEBUG_ENABLED == 1) */

    return true;
}




