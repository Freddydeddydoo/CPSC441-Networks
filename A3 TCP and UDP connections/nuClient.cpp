/*
this one (split) needs:
tcp client socket
udp server socket

this one ()
*/
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;
int BufferSize = 4096;


int main(){
    //tcp client socket:
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0){
        cerr << "connection failed!!" << endl;
        return(-1);
    }
      //server address::
    const char* servIP = "127.0.0.1";
    int servPortTCp = 8080;
    int servPortUDP = 8080;
      //server setup::

    struct sockaddr_in servAddress;
    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(servPortTCp);
    inet_pton(AF_INET, servIP, &servAddress.sin_addr);

    //connection to server::
    if (connect(clientSocket, (struct sockaddr*)&servAddress, sizeof(servAddress)) == -1) {           
        std::cerr << "Error connecting to the server" << std::endl;
        close(clientSocket);
        return 1;
    }

    //UDP Stuff
    int serverSocketUDPforClient = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocketUDPforClient == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(servPortUDP);
    inet_pton(AF_INET, servIP, &serverAddress.sin_addr);

    //Bind the socket to the specified address and port
    if (bind(serverSocketUDPforClient, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Error binding socket" << endl;
        close(serverSocketUDPforClient);
        return 1;
    }

    cout << "UDP Server listening on port " << servPortUDP << "..." << endl;

    socklen_t clientAddressSize = sizeof(serverAddress);

    printf("\t\t | | | \\    / _  |  _  _  ._ _   _  | | |\n");
    printf("\t\t o o o  \\/\\/ (/_ | (_ (_) | | | (/_ o o o\n");
    cout << "Welcome to the vowelizer program, we split text as we please!\n";
    while(true){
        string sentSentence;
        int clientSocketUDP = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSocketUDP == -1) {
            cerr << "Error creating socket" << endl;
                return 1;
            }
        int val;
        cout << "please select an option from below : \n";
        cout << "[1] Split text (Basic)\n";
        cout << "[2] Merge text (Basic)\n";
        cout << "[3] Split text (advaced)\n";
        cout << "[4] Merge text (advanced)\n";
        cout << "[5] Quit\n";
        cin  >> val;
        cin.ignore();

        //
        if(val < 0 || val > 5 ){
            cout << "PLEASE ENTER A VALID NUMBER";
            continue;
        }
        if(val == 5){
            cout << "Thank you for your time!\n";
            break;
        }
        if(val == 1){
            sentSentence.append("SplitB:");
        }
        if(val == 2){
            sentSentence.append("MergeB:");
        }
        if(val == 3){
            sentSentence.append("SplitA:");
        }
        if(val == 4){
            sentSentence.append("MergeA:");
        }
        if(val == 1 || val == 3){
            string message = "not used here";
            string usrInput;
            cout<< "sending stuff to change : ";
            getline(cin, usrInput);
            sentSentence.append(usrInput);

            cout << "sending " << sentSentence << " to server and waiting response " << endl; 
            int sendtestTCP = send(clientSocket, sentSentence.c_str(), sentSentence.length(), 0);
            if(sendtestTCP < 0){
                cerr << "something went wrong, please try again " << endl;
                exit(1);
            }


            char retunredConsonants[BufferSize];
            char returnedVowels[BufferSize];
            int recvtestTCP = recv(clientSocket, retunredConsonants, sizeof(retunredConsonants),0);
            if (recvtestTCP == -1) {
                cerr << "Error receiving data through TCP" << endl;
                exit(1);
            }
            ssize_t bytesRead = recvfrom(serverSocketUDPforClient, returnedVowels, sizeof(returnedVowels), 0,
                                    (struct sockaddr*)&serverAddress, &clientAddressSize);
            if (bytesRead == -1) {
                cerr << "Error receiving data through UDP" << endl;
                exit(1);
            }
            cout << "sent : " << sentSentence << ". recived : " << returnedVowels << " From UDP" << endl;
            cout << "sent : " << sentSentence << ". recived : " << retunredConsonants<< " From TCP" << endl;
        }
        if(val == 2 || val == 4){
            string consonants;
            string vowels;
            string sentcons;
            string sentvow;
            sentcons.append(sentSentence);
            sentvow.append(sentSentence);
            cout << "sending these consonants :";
            getline(cin, consonants);
            sentcons.append(consonants);
            cout << "sending  these vowels :";
            getline(cin, vowels);
            sentvow.append(vowels);
            send(clientSocket, sentcons.c_str(), sentcons.length(),0);
            cout << "sent : " << sentcons << "though TCP " << endl;
            sendto(clientSocketUDP,sentvow.c_str(), sentvow.length(),0,(struct sockaddr*)&serverAddress, clientAddressSize);
            cout << "sent : " << sentvow << "through UDP " << endl; 
            char returnedItems[BufferSize];
            recv(clientSocket, returnedItems, sizeof(returnedItems),0);
            cout << "recived : " << returnedItems << " through TCP " << endl;

            close(clientSocketUDP);
        }
    }
    close(clientSocket);
    close(serverSocketUDPforClient);
//end here;

    return 0;
}