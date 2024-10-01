/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      main.cpp
 *  Author:         Tomas Dolak
 *  Date:           22.09.2024
 *  Description:    Implements The Final Program.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           main.cpp
 *  @author         Tomas Dolak
 *  @date           22.09.2024
 *  @brief          Implements The Final Program.
 * ****************************/

/************************************************/
/*                  Libraries                   */
/************************************************/
#include "../include/ClientConfig.hpp"
#include "../include/NonSecureImapClient.hpp"

/************************************************/
/*                   Main                       */
/************************************************/
int main(int argc, char *argv[]) 
{
try 
{
    /* Variable Definitions  */
    bool mode                   = NON_SECURE;
    bool only_headers           = false;
    bool only_new               = false;
    std::string mail_box        = EMPTY_STR;
    std::string out_directory   = EMPTY_STR;
    std::string server_addr     = EMPTY_STR;
    std::string username        = EMPTY_STR;
    std::string password        = EMPTY_STR;

    int ret_val = 1;
    std::string current_val;
    ImapClientConfig app_config(argc, argv);

    /* Start of The Program Section... */
    mode = app_config.GetClientMode();
    if (NON_SECURE == mode)
    {
        mail_box = app_config.GetMailbox();
        out_directory = app_config.GetOutputDirectory();
        server_addr = app_config.GetServerAddress();
        only_headers = app_config.GetHeadersOnly();               /* Only Headers */
        only_new = app_config.GetNewOnly();

        NonSecureImapClient client(mail_box, out_directory, only_headers, only_new);
        ret_val = client.Run(server_addr, app_config.authData.username, app_config.authData.password);
        if (SUCCESS != ret_val)
        {
            return ret_val;
        }
    }
    else if (SECURE == ret_val)
    {
        printf("Alles Gute!\n");
    }

    /* End of The Program Section */
    return ret_val;
}
catch (const std::exception &e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}

}