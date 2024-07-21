#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#define DEFAULT_PORT "27015"

#pragma comment(lib, "Ws2_32.lib")

int CreateClientSocket(SOCKET& client, char *argv[]){
    //получение ip
    addrinfo *result = NULL;
    addrinfo *ptr = NULL;
    addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int ip_res = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if(ip_res != 0){
        std::cout << "getaddrinfo failed: " << ip_res << "\n";
        WSACleanup();
        return 1;
    }

    ptr = result;
    SOCKET client = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if(client == INVALID_SOCKET){
        std::cout << "error type is: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    ip_res = connect(client, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
    if(ip_res == SOCKET_ERROR){
        closesocket(client);
        client = INVALID_SOCKET;
    }

    //должны быть попытки подключения
    freeaddrinfo(result);
    if(client == INVALID_SOCKET){
        std::cout << "unable to connect to server";
        WSACleanup();
        return 1;
    }

    return 0;
}

int CreateServerSocket(SOCKET& server){
    addrinfo *ptr = NULL;
    addrinfo *result = NULL;
    addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int ip_res = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(ip_res != 0){
        std::cout << "getaddrinfo failed: " << ip_res << "\n";
        WSACleanup();
        return 1;
    }

    server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(server == INVALID_SOCKET){
        std::cout << "error type is: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    ip_res = bind(server, result->ai_addr, result->ai_addrlen);
    if(ip_res == SOCKET_ERROR){
        std::cout << "bind failed with error: " << ip_res << "\n";
        freeaddrinfo(result);
        closesocket(server);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

}

int main(int argc, char *argv[]){
    WSADATA wsaData; //данные 
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);// установка сетевого взаимодействия приложения и базовыми сет. системами
    SOCKET client, server; //клиентский сокет

    if(CreateClientSocket(client, argv)){
        std::cout << "create client socket error" << "\n";
        return 1;
    }
}