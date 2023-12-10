/*
File: tictactoeClient.cpp

Author: Fardin Mahid

UCID: 30114084

Date: September 28, 2023

Version 1.1

Socket code was sampled from Microsofts website for socket programming:
https://learn.microsoft.com/en-us/windows/win32/winsock/finished-server-and-client-code

************** Purpose *********************************************************************************** 
This code is a clientside script meant to play a game with perhaps one of the last remaining servers still 
alive online. It recives a message from the user in the form HEAD:BODY where HEAD is a 4-letter instruction 
and BODY is extra information given to server if needed.

*/

#define WIN32_LEAN_AND_MEAN

//Header files included
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
//#define DEFAULT_PORT "6969"
 int filesMade = 0;
 int clientwins = 0;
 int serverwins = 0;
 int noWin = 0;
 int gamesplayed = 0;
const char* commands[4] = {"NEWG", "LOAD", "EXIT", "SHOW"};

int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    int iResult;
    // Validate the parameters
    if (argc != 3) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], argv[2], &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Game stuff down here:

    char sendbuf[DEFAULT_BUFLEN];


    // comment this when testing cin.
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

        // Send user input to the server

    // Receive until the peer closes the connection
    //  we need to communicate to the server 


    // Start game here with welcome message ::
    printf("\t\t | | | \\    / _  |  _  _  ._ _   _  | | |\n");
    printf("\t\t o o o  \\/\\/ (/_ | (_ (_) | | | (/_ o o o\n");

while(true){
    std::cout << "     Welcome to the tictactoe game !!     \n";
    std::cout << "---- Please select one of the following options by typing the value associated ----\n";
    //tally games:
    //initial game menu.
    char menuOption[32];
        std::cout << "\t     New game: <NEWG> \n";
        std::cout << "\t     Load Saved game: <LOAD:<Textfile.txt>>\n";
        std::cout << "\t     Exit Game:<EXIT> \n";
        std::cout << "\t     Show Score:<SHOW> \n";
    
    std::string filename = "Game" + std::to_string(char(filesMade));
        filename += ".txt";
    std::cin.getline(sendbuf, DEFAULT_BUFLEN);
    
    
    
    // Excecute new game
    const char* found = strstr(sendbuf, "NEWG");
    if(found != nullptr){
        filesMade++;
        std::ofstream myFile(filename);
        int roundsPlayed = 0;
        gamesplayed++;
        while (true) {
            
            if (roundsPlayed != 0){
                std::cin.getline(sendbuf, DEFAULT_BUFLEN);
            }
            if(strstr(recvbuf, "OVER:") != nullptr){
                if(strstr(recvbuf, "OVER:C") != nullptr){
                    std::cout << "Client Wins!" << std::endl;
                    std::cout << recvbuf << std::endl;
                    clientwins++;
                }
                if(strstr(recvbuf, "OVER:S") != nullptr){
                    std::cout << "Server Wins!" << std::endl;
                    std::cout << recvbuf << std::endl;
                    serverwins++;
                }
                if(strstr(recvbuf, "OVER:N") != nullptr){
                    std::cout << "No one won :(" << std::endl;
                    std::cout << recvbuf << std::endl;
                    noWin++;
                }
                myFile << recvbuf;
                myFile.close();
                break;
            }
            
            iResult = send(ConnectSocket, sendbuf, (int)sizeof(sendbuf), 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes Sent: %d\n", iResult);
            //Receive and print data from the server
            if(strstr(sendbuf, "CLOS") != nullptr){
                std::cout << "Thanks for playing!" << std::endl;
                break;

            }
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            std::cout << recvbuf << '\n';
            
            roundsPlayed++;
           
        }
        myFile.close();
    }


    //Load Game:
    found = strstr(sendbuf, "LOAD");
    int size = sizeof(sendbuf)/sizeof(sendbuf[0]);
    if(found != nullptr && size > 5){
        //std::ifstream myFile;
       char * LOAD = strtok(sendbuf, ":");
       //test you wrote all of lOAD:
       if(LOAD == nullptr){
        std::cout << "Please type up a file name !\n";
        continue;
       }
       char * FILENAME = strtok(nullptr, ":");
    
        //find file name.
        std::ofstream myFile;
        std::ifstream filegood(FILENAME);
        std::cout << FILENAME << "\n";
        if(!(filegood.good())){
            std::cout << "Please enter a good file name that exsits in this directory ! \n";
            filegood.close();
            continue;
        }
        filegood.open(FILENAME);
        //sendbuf = strtok(sendbuf, ":");
        //sendbuf = strapp(stuff from file)
        //final should be sendbuf = LOAD:X,0,0,0,0,0,0,0,0,0,
        //getting stuff from file:
        char  filestuffs[100];
        filegood >> filestuffs;
        std::cout << "Filestuffs is :" << filestuffs;
        char * OVER = strtok(filestuffs, "S,C.N");
        char * GAMEBOARD = strtok(nullptr, "S,C.N");
        if(OVER == nullptr){
            std::cout << "Please enter a valid file thank you ! \n";
            filegood.close();
            myFile.close(); 
        }
        if(GAMEBOARD == nullptr){
           std::cout << "Please enter a valid file thank you ! \n";
            filegood.close();
            myFile.close(); 
        }
        std::cout << "Expected Over getting" << OVER  << "\n";
        std::cout << GAMEBOARD << "\n";


       filesMade++;

        
    }

    //Exit game
    found = strstr(sendbuf, "EXIT");
    if(found != nullptr){
        std::cout << "Thanks for playing !";
        break;
    }

    //show score
    found = strstr(sendbuf, "SHOW");
    if(found != nullptr){
        std::cout << "Client wins : " << clientwins << std::endl;
        std::cout << "Server wins : " << serverwins << std::endl;
        std::cout << "No Winners  : " << noWin << std::endl;
        std::cout << "total games : " << gamesplayed << std::endl;
        continue;
    }    
}
// while (true) {
//         //if(roundsPlayed != 0){std::cout << "     Show Score:<SHOW> \n";}
//     std::cin.getline(sendbuf, DEFAULT_BUFLEN);
//     iResult = send(ConnectSocket, sendbuf, (int)sizeof(sendbuf), 0);
//     if (iResult == SOCKET_ERROR) {
//         printf("send failed with error: %d\n", WSAGetLastError());
//         closesocket(ConnectSocket);
//         WSACleanup();
//         return 1;
//     }
//     printf("Bytes Sent: %d\n", iResult);
//     // Receive and print data from the server
//     iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
//     if (iResult == 0) {
//         printf("Connection closed\n");
//     } else {
//         printf("recv failed with error: %d\n", WSAGetLastError());
//     }
//     std::cout << recvbuf << '\n';
//     //if(){}
//     roundsPlayed++;
// }

    // cleanup
    //NuFile.close();

    //cleanup values and close socket
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}

