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
#include "../include/definitions.hpp"


/************************************************/
/*                   Main                       */
/************************************************/
int main(int argc, char *argv[]) 
{
try {
    /* Start of The Program Section... */
    (void)argc;
    (void)argv;
    printf("Alles Gute!\n");

    /* End of The Program Section */

    return 0;
}
catch (const std::exception &e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}

}