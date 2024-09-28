/******************************
 *  Project:        ISA Project - IMAP Client With TLS Support
 *  File Name:      NonSecureImapClient.cpp
 *  Author:         Tomas Dolak
 *  Date:           28.09.2024
 *  Description:    Implements Non-Secure IMAPv4 Client That Communicates Just Thru TCP/IP With Port 143.
 *
 * ****************************/

/******************************
 *  @package        ISA Project - IMAP Client With TLS Support
 *  @file           NonSecureImapClient.cpp
 *  @author         Tomas Dolak
 *  @date           28.09.2024
 *  @brief          Implements Non-Secure IMAPv4 Client That Communicates Just Thru TCP/IP With Port 143.
 * ****************************/

NonSecureImapClient::NonSecureImapClient(const std::string& mailbox, const std::string& outputDir)
    : mailbox(mailbox), 
    outputDir(outputDir) {}


bool NonSecureImapClient::ConnectImapServer(const std::string& serverAddress, const std::string& username, const std::string& password)
{
    // Resolve server hostname to IPv4 address
    std::string serverIP = resolveHostnameToIPv4(serverAddress);
    if (serverIP.empty()) {
        std::cerr << "ERR: Unable to resolve hostname to IP address.\n";
        return false;
    }

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "ERR: Unable to create socket.\n";
        return false;
    }

    // Prepare the server address structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port); // IMAP standard port 143 for non-secure
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "ERR: Invalid server IP address format.\n";
        close(sockfd);
        return false;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "ERR: Unable to connect to the IMAP server.\n";
        close(sockfd);
        return false;
    }

    std::cout << "INFO: Successfully connected to the IMAP server at " << serverAddress << " (IP: " << serverIP << ")\n";

    // Here you would typically authenticate using the username and password
    // (This part of the protocol is omitted for simplicity)

    // After authentication, you'd select the mailbox and start fetching emails
    std::cout << "INFO: Selected mailbox: " << mailbox << "\n";
    std::cout << "INFO: Emails will be saved to: " << outputDir << "\n";

    // Close the socket when done
    close(sockfd);
    return true;
}