# ISA Project: IMAP Client With TLS Support
- Author: Tomáš Dolák 
- Login: [xdolak09](https://www.vut.cz/lide/tomas-dolak-247220)
- Email: <xdolak09@stud.fit.vutbr.cz>

The goal of this project in the subject Network Applications and Network Administration was to create a IMAPv4 Client that is able to communicate just thru non-secure connection - TCP/IP or secure connection with SSL/TLS to the IMAP server.

## Table of contents
- [Requirements](#requirements)
    - [Compiler](#compiler)
    - [Libraries](#libraries)
    - [Build tools](#build-tools)
    - [Operation system](#operating-system)
- [Installation](#installation)
- [Project Organization](#project-organization)
- [Using the `imapcl` Program](#using-the-imapcl-program)
    - [Required Parameters](#required-parameters)
    - [Optional Parameters](#optional-parameters)
    - [Authentication File Format](#authentication-file-format)
- [IMAP in Email Services and How It's Used](#imap-in-email-services-and-how-its-used)
    - [Internet Message Access Protocol](#internet-message-access-protocol)
    - [How IMAP Works?](#how-imap-works)
- [Program's Features](#programs-features)
    - [UIDVALIDITY](#uidvalidity)
    - [Handling Non-Existent Output Directory](#handling-non-existent-output-directory)
    - [Storing The Contents of Multiple Mailboxes In One Folder](#storing-the-contents-of-multiple-mailboxes-in-one-folder)
    - [Gentle Downloading](#gentle-downloading)
- [Resources](#resources)

## Requirements
To build and run `imapcl`, you will need the following:

### Compiler
- **Clang++** with support for **C++17** standard. This project uses specific compiler flags to enforce code quality and standards. Make sure your compiler version supports `-std=c++17` along with the flags `-Wall -Wextra -Werror -Wshadow -Wnon-virtual-dtor -pedantic`.

### Libraries
- **Google Test (gtest)**: Required for compiling and running the unit tests. Ensure you have Google Test installed on your system as it uses `-lgtest -lgtest_main -pthread` flags for linking.

- **Python Libraries For Testing**: For local testing by IMAP server stored in `tests` directory, is important to have install right libraries as `os`, `socket`, `threading`, `subprocess`, `time` and `signal`.

### Build tools
- **Make**: This project uses a `Makefile` for easy building and testing. Ensure you have Make installed on your system.

### Operating system
- The Makefile and C++ code were designed with Unix-like environments in mind (Linux, MacOS). While it may be possible to compile and run the project on Windows, using a Unix-like environment (or WSL for Windows users) is recommended.

## Installation
1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Run `make` to build the client application. This will create the `imapcl` executable.
4. (Optional) Run `make test` to build and run the unit tests. Ensure you have Google Test installed.

Please refer to the Makefile for additional targets and commands.

## Project organization 
```
ipk-proj-1/
│
├── include/                # Header files for class declarations.
│   ├── BaseImapClient.hpp       # Base class for IMAP client functionality.
│   ├── ClientConfig.hpp         # Argument parsing, configuration settings and parameters for the client.
│   ├── definitions.hpp          # General definitions (return codes, structures and adjustable definitions).
│   ├── NonSecureImapClient.hpp  # Derived class for non-secure IMAP client (Communication just thru TCP/IP).
│   ├── SecureImapClient.hpp     # Derived class for secure IMAP client with SSL/TLS.
│   └── utilities.hpp            # Utility functions and helper methods.
├── src/                    # Source files containing class definitions and main application logic.
│   ├── BaseImapClient.cpp       # Implementation of the base IMAP client class.
│   ├── ClientConfig.cpp         # Client configuration setup and parsing.
│   ├── main.cpp                 # Entry point for the imapcl application.
│   ├── NonSecureImapClient.cpp  # Non-secure IMAP client functionalities.
│   ├── SecureImapClient.cpp     # Secure IMAP client functionalities using SSL/TLS.
│   └── utilities.cpp            # Implementations of utility functions.
│
├── test/                   # Test files
│   ├── output/             # Output directory for downloaded test emails.
│   │   
│   │── imap_server.py      # IMAP server for local testing.
│   │   
│   └── test_emails/        # Test email that will send local IMAP server.
│
├── doc/                    # Documentation files and resources
│   │── pics/               # Directory of pictures used in README.md
│   │
│   │── manual.pdf
│   │
│   └── CHANGELOG.md        # Includes features that were implemented in addition (for more details see manual.pdf)
│
├── Makefile                # Makefile for compiling the project
│
└── README.md               # Overview and documentation for the project
```

## Using the `imapcl` Program

The `imapcl` program is designed to download email messages from a server using the IMAP protocol, with optional support for encrypted connections via SSL/TLS. To run the program, use the following command syntax:

```
imapcl server [-p port] [-T [-c certfile] [-C certaddr]] [-n] [-h] -a auth_file [-b MAILBOX] -o out_dir
```
The order of parameters is flexible. Below is a description of each parameter:

### Required Parameters
- `server`: IP address or domain name of the IMAP server.
- `-a auth_file`: Path to the file containing user authentication credentials.
- `-o out_dir`: Output directory where downloaded messages will be saved.

### Optional Parameters
- `-p port`: Specifies the server port. The default value depends on whether the -T parameter is used (use port 993 for encrypted connections).
- `-T`: Enables an encrypted connection using the IMAPS protocol. If not specified, an unencrypted connection (port 143) is used.
- `-c certfile`: Path to a certificate file used to verify the validity of the server’s SSL/TLS certificate.
- `-C certaddr`: Path to a directory containing certificates to verify the SSL/TLS certificate presented by the server. The default value is /etc/ssl/certs.
- `-n`: Only new messages – the program will work only with messages that have not yet been downloaded.
- `-h`: Download only message headers without the content.
- `-b MAILBOX`: Specifies the mailbox name on the server. The default is INBOX.

### Authentication File Format
The configuration file containing authentication credentials must be stored as a text file in the following format:

```
username = your_username
password = your_password
```
The file should follow Unix formatting, with each line ending with a newline character.

## IMAP in Email Services and How It's Used

### Internet Message Access Protocol
The Internet Message Access Protocol (IMAP) is a protocol for receiving email that allows users to access their mailboxes from any device with an internet connection. IMAP was designed to provide flexibility in accessing email messages, regardless of the device or application used. This is achieved by acting as an intermediary between the email server and client, rather than directly downloading emails onto the device.However, IMAP also allows emails to be stored locally, a feature leveraged by this program to download and store emails on the user’s device.[2] [5]

While IMAP enhances flexibility, it does come with security considerations. By default, IMAP transmits login credentials in plain text, leaving usernames and passwords vulnerable if intercepted. This issue can be mitigated by configuring IMAP to operate over Transport Layer Security (TLS) in our case by parameter `-T`, which encrypts the communication between the client and server. Using TLS are recommended practice to improve IMAP security.[5] [6]

### How IMAP Works?
IMAP establishes a connection between the email client and server. In most email applications, IMAP allows users to view email headers quickly and download full messages only when selected, which conserves data. Outgoing emails, on the other hand, are sent via the Simple Mail Transfer Protocol (SMTP), which handles message delivery to the recipient. [7] [8]

## Program's Features 

### UIDVALIDITY
The `imapcl` program uses the `UIDVALIDITY` values obtained from the IMAP server. This value indicates the status of a given mailbox for a given account. When downloading to a specific output directory for the first time, the `UIDVALIDITY` value is stored into `.uidvalidity.txt` file, this value is used in case the user wants to repeatedly download files to the same `output directory` and have synchronized mailboxes. On each subsequent run, the `UIDVALIDITY` value is checked to see if it is the same locally (in this `.uidvalidity!` file) and on the IMAP server, if the values are different, it is a sign that the structure of the mailbox on the server has changed (i.e. the emails have been removed or moved to another mailbox, etc.) and the output directory needs to be purged and the emails downloaded. If the value is the same both locally and on the server, the downloaded files in the output directory are preserved and only those missing in the output directory are downloaded (usually new emails).

Example of `uidvalidity.txt` file, that is stored in `output directory`:
```
INBOX=1232988148
Important=1662977155
```
- **note** `INBOX` and `Important` are mailboxes that was stored into one `output directory` after character `=` is stored their UIDVALIDY value, that was up to date when the mailbox was last time downloaded. If locally stored UIDVALIDY value matches the mailbox value on IMAP server, just new emails are downloaded for the sake of sustainability.

### Handling Non-Existent Output Directory
The client always needs to have a specific `output directory` to which it will download emails from the IMAP server, it can happen that the user specifies his/her own location, but the folder on the given path does not exist (for example `-o /Desktop/email/my_mailbox/`) in this case the client is able to create the folder on the given path (in this example on `/Desktop/email/my_mailbox/`) and store the emails in it.

### Storing The Contents of Multiple Mailboxes In One Folder
The client is able to download the contents of multiple mailboxes into one output directory. Because the program often relies on the value of UIDVALIDITY, the client stores the values of already known mailboxes from which emails are downloaded to the given output directory, thus synchronization of multiple mailboxes is guaranteed.

### Gentle Downloading 
The program also introduces a gentle download, i.e. emails are downloaded only if the 
emails are downloaded to a given output directory for the first time, or the structure of 
the local copy of the mailbox or the structure on the IMAP server has changed. For example, 
in a local folder where emails have been downloaded once before, some email(s) in the folder 
have been removed (perhaps by mistake) or the mailbox on the IMAP server has been modified 
and the `UIDVALIDITY` value has changed. The client is always in sync with the IMAP server.

Example of a downloaded email filename:
```
MSG_INBOX_1717.txt
```
Where `INBOX` is mailbox and `1717` is UID of this email.

## Known Issues 
A user's mailbox can often be bulky and contain a number of large emails whose size can exceed 30MB, in which case it was observed during testing that downloading the entire mailbox can take a considerable amount of time and should be taken into account. 

## Resources
[1] "Transport protocol port registry" [online]. [cited 2024-10-28]. Available at [https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml](https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml)

[2] "INTERNET MESSAGE ACCESS PROTOCOL - VERSION 4rev1" [online]. [cited 2024-10-28]. Available at [https://datatracker.ietf.org/doc/html/rfc3501](https://datatracker.ietf.org/doc/html/rfc3501)

[3] "Internet Message Format" [online]. [cited 2024-10-28]. Available at [https://datatracker.ietf.org/doc/html/rfc5322](https://datatracker.ietf.org/doc/html/rfc5322)

[4] "Secure programming with the OpenSSL API" [online]. [cited 2024-10-28]. Available at [https://developer.ibm.com/tutorials/l-openssl/](https://developer.ibm.com/tutorials/l-openssl/)

[5] "What is IMAP?" [online]. [cited 2024-10-28]. Available at [https://www.cloudflare.com/learning/email-security/what-is-imap/](https://www.cloudflare.com/learning/email-security/what-is-imap/)

[6] "What is the difference between POP and IMAP?" [online]. [cited 2024-10-28]. Available at [https://support.microsoft.com/en-us/office/what-is-the-difference-between-pop-and-imap-85c0e47f-931d-4035-b409-af3318b194a8](https://support.microsoft.com/en-us/office/what-is-the-difference-between-pop-and-imap-85c0e47f-931d-4035-b409-af3318b194a8) 

[7] "IMAP (Internet Message Access Protocol)" [online]. [cited 2024-10-28]. Available at [https://www.techtarget.com/whatis/definition/IMAP-Internet-Message-Access-Protocol](https://www.techtarget.com/whatis/definition/IMAP-Internet-Message-Access-Protocol)

[8] "Email Protocols: Learn The Difference Between IMAP, POP3, and SMTP" [online]. [cited 2024-10-28]. Available at [https://emaillabs.io/en/email-protocols-learn-the-difference-between-imap-pop3-and-smtp/](https://emaillabs.io/en/email-protocols-learn-the-difference-between-imap-pop3-and-smtp/)
