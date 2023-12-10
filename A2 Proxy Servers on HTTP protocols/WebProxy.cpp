#include <iostream>
#include <cstring>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
const int MAX_BUFF = 4096;
const int MAX_STRG = 65536;

// 404 error::

   std::string html404err = R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>404 Not Found</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    background-color: #f4f4f4;
                    text-align: center;
                    padding: 50px;
                }
                h1 {
                    font-size: 2em;
                    color: #333;
                }
                p {
                    font-size: 1.2em;
                    color: #777;
                }
            </style>
        </head>
        <body>
            <h1>404 Not Found</h1>
            <p>The requested URL was not found on this server.</p>
        </body>
        </html>
    )";


    int count = 0;
int main() {
    int server_fd, client_socket, proxy_socket;
    struct sockaddr_in address, client_addr;
    char  buffer[MAX_BUFF];
    int valread;
    // Step 1: Create a server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // ... (error handling for socket creation)

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8184);

    // Step 2: Bind the socket
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    // ... (error handling for bind)

    // Step 3: Listen for client connections
    listen(server_fd, 3);
    // ... (error handling for listen)

    std::cout << "Proxy server is listening on port 8184..." << std::endl;

    // Step 4: Accept client connection
    int addrlen = sizeof(address);
    client_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen);
    // ... (error handling for accept)

    // Step 5: Create a socket to connect to the destination server
    proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
    // ... (error handling for proxy socket creation)

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    // get these values from the webserver;
    // reviceve from webclient
        while(true){
            valread = recv(client_socket, buffer, sizeof(buffer), 0);
            if (valread <= 0) {
                break;
            }
            char copyBuf[MAX_BUFF];
            strcpy(copyBuf, buffer);
            char* GetReq = strtok(copyBuf, "\r\n");
            char* GetHost = strtok(NULL,"\r\n");
            std::cout << buffer << std::endl;
            //std::cout << buffer << std::endl;
            // std::cout << GetHost << "\n";
            // std::cout << GetReq << "\n";
            char* HOST = strtok(GetHost, " ");
            char* HOSTNAME = strtok(NULL, "\r\n");
            if(count == 0){
                std::cout << HOST << "\n";
                std::cout << HOSTNAME<< "\n";
            }
            hostent * record = gethostbyname(HOSTNAME);
            char** ipAddressList = record->h_addr_list;
            //works what the hell.
            if(count == 0){
                while (*ipAddressList != nullptr) {
                struct in_addr* ipAddress = reinterpret_cast<struct in_addr*>(*ipAddressList);
                std::cout << "IPv4 Address: " << inet_ntoa(*ipAddress) << std::endl;
                ++ipAddressList;
                }
            }
            // works
            if (record == nullptr) {
            std::cerr << "Error: Unable to resolve hostname." << std::endl;
            char* temp;
            strcpy(temp, html404err.c_str());
            send(client_socket, temp, sizeof(temp), 0);
            std::exit(0);
            }
            struct sockaddr_in dest_addr;
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(80);
            in_addr * addy = (in_addr *)record->h_addr;
            dest_addr.sin_addr.s_addr = inet_addr(inet_ntoa(* addy));
            char* strgBuff = new char[MAX_STRG];
            strgBuff[0] = '\0';
            connect(proxy_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if(proxy_socket < 0 ){
                std::cerr << "Nothing was sent. ending connection :" << std::endl;
                exit(0);
            }
            // else{
            //     std::cout << "Working...\n";
            // }
            //std::cout << buffer << std::endl;
            int bytesent = send(proxy_socket, buffer, strlen(buffer),0);
            //std::cout << buffer << std::endl;
            if(bytesent < 0){
                perror("send");
            }else{
                
                if(count == 0){
                    std::cout << "successfully sent the buffer :\n";
                    std::cout << buffer;
                }
            }

            //         
            while(strstr(strgBuff, "</HTML>") == NULL || strstr(strgBuff, "</html>") == NULL){

            // start receiving message::
                char tmp[MAX_BUFF];
                int Byterecv = recv(proxy_socket, tmp, MAX_BUFF,0);
                std::cout << "bytes recived : " << Byterecv << std::endl;
                if(Byterecv < 0){
                    std::cerr << "\nThere has been an issue recveiving info, \n exiting now. \n";
                    exit(0);
                }
                if(Byterecv == 0){
                    break;
                }
                std::string out = "dog";
                if(count == 0){
                    std::string connectionStatus = std::string(tmp);
                    std::string searchStringOne = "HTTP/1.1 200";
                    size_t found = connectionStatus.find("HTTP/1.1 ");
                    if(connectionStatus.find("HTTP/1.1 ") != std::string::npos|| connectionStatus.find("HTTP/1.0 ") != std::string::npos){
                        out = connectionStatus.substr(found, searchStringOne.length()); 
                    }
                }
                    std::cout << out << std::endl;
                    //char* pos = strstr(tmp, " frog");

                    bool insideTag = false;
                        for (int i = 0; i < Byterecv; ++i) {
                        if (tmp[i] == '<') {
                        insideTag = true;
                        }else if (tmp[i] == '>') {
                        insideTag = false;
                        }
                // Replace "frog" only if we are not inside an HTML tag
                if (!insideTag && i + 3 < Byterecv && tmp[i] == 'f' && tmp[i + 1] == 'r' && tmp[i + 2] == 'o' && tmp[i + 3] == 'g') {
                    tmp[i] = 'F';
                    tmp[i + 1] = 'r';
                    tmp[i + 2] = 'e';
                    tmp[i + 3] = 'd';
                    }
                }

                tmp[Byterecv] = '\0';
                std::cout << tmp;
                strcat(strgBuff, tmp);


                int okaySend = send(client_socket, tmp, Byterecv, 0);
                if(okaySend < 0){
                    perror("send");
                }
            }
            count++;
        }



    // Step 8: Close the sockets
    close(client_socket);
    close(proxy_socket);
    close(server_fd);

    // storage cleanup::
    //delete [] buffer;

    return 0;
}





