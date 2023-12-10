/*
for split:
this one needs:
tcp server socket
udp client socket
*/
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

//to check if vowel
bool isVow(char string);

//to merge (simple)
char* mergeText(char* cons, char* vow);

int bufLen = 4096;

int main(){

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Server address and port
    const char* serverIP = "127.0.0.1";  // Server IP address (loopback address in this case)
    int serverPort = 8080;                // Server port number
    //int serverPortUDP = 8081;


    // Set up server address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP, &serverAddress.sin_addr);

    // Bind the socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    std::cout << "listening on port 8080..." << std::endl;
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening for connections" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Accept incoming connections
    int clientSocket;
    sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    // Set up server address structure

    //setting up UDP server
    int serverSocketUDP = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocketUDP == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in servAddress;
    servAddress.sin_family = AF_INET;
    servAddress.sin_addr.s_addr = INADDR_ANY;
    servAddress.sin_port = htons(8081);

    // //bind for UDP server portion.
    if (bind(serverSocketUDP, (struct sockaddr*)&servAddress, sizeof(servAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocketUDP);
        return 1;
    }

    while (true){
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }
        else{
            std::cout << "Accepted Tcp connenction. Awating Response..." << std::endl;
        }

        //UDP client stuff
        int clientSocketUDP = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSocketUDP == -1) {
            std::cerr << "Error creating socket" << std::endl;
            return 1;
        }

        //to recieve
        char buffer[bufLen];
        char bufferudp[bufLen];

        recv(clientSocket, buffer, sizeof(buffer), 0);

        //to send back
            char VowelsUDP[bufLen] = {0};
            char ConsonantsTCP[bufLen] = {0};
            char mergedPhrase[bufLen] = {0};
            // Basic Split:
            std::cout << "recived : " << buffer << std::endl;
        if(strstr(buffer, "SplitB:") != nullptr){
            std::string tmpString(buffer);
           // std::cout << "tmpString is : " << tmpString << std::endl;
            size_t pos = tmpString.find("SplitB:");
            tmpString.erase(0,pos + strlen("SplitpB:") - 1);
            strcpy(buffer, tmpString.c_str());
            //std::cout << "buffer is : " << buffer << std::endl;
            int i = 0;
            while(buffer[i] != '\0'){
                if(isVow(buffer[i])){
                    VowelsUDP[i] = buffer[i];
                    ConsonantsTCP[i] = ' ';
                }
                else{
                    ConsonantsTCP[i] = buffer[i];
                    VowelsUDP[i] = ' ';
                }
                i++;
            }
            VowelsUDP[i] = '\0';
            ConsonantsTCP[i] = '\0';

            std::cout << "sending back : " << ConsonantsTCP << " through TCP" << std::endl;
            std::cout << "sending back : " << VowelsUDP << " through UDP" << std::endl;  
            sendto(clientSocketUDP, VowelsUDP, sizeof(VowelsUDP), 0,
           (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            //std::cout << "finished on this end 3:" << std::endl;
            send(clientSocket, ConsonantsTCP, sizeof(ConsonantsTCP), 0);
            //std::cout << "finished on this end 2:" << std::endl;
            close(clientSocketUDP);
            //std::cout << "finished on this end 1:" << std::endl;
            close(clientSocket);
            //std::cout << "finished on this end :" << std::endl;
        }
        if(strstr(buffer, "SplitA:") != nullptr){
            std::string tmpString(buffer);
           // std::cout << "tmpString is : " << tmpString << std::endl;
            size_t pos = tmpString.find("SplitA:");
            tmpString.erase(0,pos + strlen("SplitpA:") - 1);
            strcpy(buffer, tmpString.c_str());

            int j = 0;
            int counter = 0;
            int c = 0;
            int v = 0;

            while (buffer[j] != '\0') {
                if (!isVow(buffer[j])) {
                    if (buffer[j] == ' ') {
                        ConsonantsTCP[c] = buffer[j];
                        counter = 0;
                        c++;
                    } else {
                        ConsonantsTCP[c] = buffer[j];
                        counter++;
                        c++;
                    }
                } else {
                    if (counter == 0) {
                        VowelsUDP[v] = buffer[j];
                        v++;
                    } else {
                        VowelsUDP[v] = static_cast<char>(counter + '0'); // Convert counter to char and store as numeric value
                        VowelsUDP[v + 1] = buffer[j];
                        counter = 0;
                        v += 2;
                    }
                }
                j++;
            }


            std::cout << "sending back : " << ConsonantsTCP << " through TCP" << std::endl;
            std::cout << "sending back : " << VowelsUDP << " through UDP" << std::endl;  
            sendto(clientSocketUDP, VowelsUDP, sizeof(VowelsUDP), 0,
           (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            //std::cout << "finished on this end 3:" << std::endl;
            send(clientSocket, ConsonantsTCP, sizeof(ConsonantsTCP), 0);
            //std::cout << "finished on this end 2:" << std::endl;
            close(clientSocketUDP);
            //std::cout << "finished on this end 1:" << std::endl;
            close(clientSocket);
            //std::cout << "finished on this end :" << std::endl;
        }
        if(strstr(buffer,"MergeB:") != nullptr){
            std::cout << "going into a simple merge : " << std::endl;
            ssize_t bytesRead = recvfrom(serverSocketUDP, bufferudp, sizeof(bufferudp), 0,
                                 (struct sockaddr*)&clientAddress, &clientAddressSize);
            if(strstr(bufferudp, "MergeB:") == nullptr){
                std::cerr << "Error, Vowel string and consonant string do not match " << std::endl;
                break;
            }
            // timeout stuff if I want to implement
            std::cout << "Recieved : " << buffer << "from TCP socket : " << std::endl;
            std::cout << "Recieved : " << bufferudp << "from UDP soccket : " << std::endl;
            //get rid of mergeB:
            std::string constmp(buffer);
            size_t pos = constmp.find("SplitB:");
            constmp.erase(0,pos + strlen("SplitB:") - 1);
            strcpy(buffer, constmp.c_str());
            std::string vowTmp(bufferudp);
            pos = vowTmp.find("SplitB:");
            vowTmp.erase(0,pos + strlen("SplitB:") - 1);
            strcpy(bufferudp, vowTmp.c_str());

            //merge text:
            char* toReturn = mergeText(buffer, bufferudp);

            send(clientSocket, toReturn, sizeof(toReturn), 0);
            close(clientSocketUDP);   
        }

        //Basic Merge

            std::cout << "finished one round, going again. " << std::endl;
            //close(clientSocketUDP);
            close(clientSocket);
            continue;
           

    }
    //close(serverSocketUDP);
    close(serverSocket);
}



bool isVow(char string){
  if (string == 'a' || string == 'e' || string == 'i' ||    string == 'o' || string == 'u' 
      || string == 'A' || string == 'E' || string == 'I' || string == 'O' || string  == 'U'){
    return true;
  }
  return false;
}

char* mergeText(char* cons, char* vow){
//   if(strlen(cons) != strlen(vow)){
//     std::cerr << "Error: Consonants and vowels are not the same length" << std::endl;
//     exit(1);
//   }
  std::cout << strlen(cons) << std::endl;
  char* merged = new char[strlen(cons) + 1];
  for(int i = 0; i < strlen(cons); i++){
    if(cons[i] == ' ' && vow[i] == ' '){
      merged[i] = ' ';
    }
    if(cons[i] == ' ' && vow[i] != ' '){
      merged[i] = vow[i];
    }
    else{
      merged[i] = cons[i];
    }
  }
  merged[sizeof(cons)] = '\0';
  return merged;
  
}