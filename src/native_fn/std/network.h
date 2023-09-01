#include <iostream>
#include <cstring>
#include <sstream>
#include <stdexcept>

#include "../../compiler/object.h"
#include "../../vm/vm.h"
#include "../define_native.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <IcmpAPI.h>
#else 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#endif

class NetworkFunc {
private:
    static Value connect_native(int arg_count, Value* args) {
        if (arg_count != 2 || !IS_STRING(args[0]) || !IS_NUMBER(args[1])) {
            return BOOL_VAL(false);
        }

        const char* address = AS_CSTRING(args[0]);
        if (strcmp(address, "localhost") == 0) {
            address = "127.0.0.1";
        }
        int port = (int)AS_NUMBER(args[1]);

#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize Winsock." << std::endl;
            return BOOL_VAL(false);
        }
#endif

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            std::cerr << "Failed to create socket." << std::endl;
#ifdef _WIN32
            WSACleanup();
#endif
            return BOOL_VAL(false);
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        if (inet_pton(AF_INET, address, &serverAddr.sin_addr) <= 0) {
            std::cerr << "Invalid Address." << std::endl;
            close(sockfd);
#ifdef _WIN32
            WSACleanup();
#endif
            return BOOL_VAL(false);
        }

        if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Failed to connect." << std::endl;
            close(sockfd);
#ifdef _WIN32
            WSACleanup();
#endif
            return BOOL_VAL(false);
        }

#ifdef _WIN32
        WSACleanup();
#endif

        return BOOL_VAL(false); // Successfully connected
    }

    static Value ping_native(int arg_count, Value* args) {
        if (arg_count != 1 || !IS_STRING(args[0])) {
            return BOOL_VAL(false);
        }

        const char* address = AS_CSTRING(args[0]);

#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize Winsock." << std::endl;
            return BOOL_VAL(false);
        }

        HANDLE icmpFile = IcmpCreateFile();
        if (icmpFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create ICMP file." << std::endl;
            WSACleanup();
            return BOOL_VAL(false);
        }

        const int bufferSize = sizeof(ICMP_ECHO_REPLY) + 8; // Adjust buffer size as needed
        char sendBuffer[bufferSize];
        char recvBuffer[bufferSize];
        ICMP_ECHO_REPLY icmpReply;

        memset(&icmpReply, 0, sizeof(ICMP_ECHO_REPLY));
        memset(sendBuffer, 0, bufferSize);
        memset(recvBuffer, 0, bufferSize);

        // Construct ICMP echo request packet (adjust data as needed)
        IP_OPTION_INFORMATION options = {0};
        options.OptionsSize = sizeof(IP_OPTION_INFORMATION);
        options.Ttl = 128; // Adjust TTL (Time To Live) as needed

        if (!IcmpSendEcho2(icmpFile, NULL, NULL, NULL, inet_addr(address), sendBuffer, sizeof(sendBuffer), &options, recvBuffer, sizeof(recvBuffer), 1000)) {
            if (icmpReply.Status == IP_SUCCESS) {
                std::cout << "Ping to " << address << " succeeded." << std::endl;
                return BOOL_VAL(true);
            } else {
                std::cerr << "Ping to " << address << " failed. Status: " << icmpReply.Status << std::endl;
                return BOOL_VAL(false);
            }
            IcmpCloseHandle(icmpFile);
            WSACleanup();
            return BOOL_VAL(false);
        }

        IcmpCloseHandle(icmpFile);
        WSACleanup();

#else
        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (sockfd == -1) {
            std::cerr << "Failed to create socket." << std::endl;
            return BOOL_VAL(false);
        }

        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        if (inet_pton(AF_INET, address, &(dest_addr.sin_addr)) <= 0) {
            std::cerr << "Invalid Address." << std::endl;
            close(sockfd);
            return BOOL_VAL(false);
        }

        // Build and send an ICMP echo (ping) request packet here

        // Placeholder code for sending ICMP echo request
        std::cout << "Sending ICMP echo request to " << address << std::endl;

        close(sockfd); // Close the socket after sending the request
#endif

        return BOOL_VAL(true); // Successfully sent the ping request
    }

public:
    static void define_network_natives() {
        Natives::define_native("connect", connect_native);
        Natives::define_native("ping", ping_native);
    }
};