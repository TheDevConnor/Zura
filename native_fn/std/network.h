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
#include <netinet/ip.h> // Added for non-Windows
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

        return BOOL_VAL(true); // Successfully connected
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
                IcmpCloseHandle(icmpFile);
                WSACleanup();
                return BOOL_VAL(false);
            }
        }

        IcmpCloseHandle(icmpFile);
        WSACleanup();

#else
        unsigned short checksum(unsigned short *buf, int len) {
            unsigned long sum = 0;
            while (len > 1) {
                sum += *buf++;
                len -= 2;
            }
            if (len == 1) {
                sum += *(unsigned char*)buf;
            }

            sum = (sum >> 16) + (sum & 0XFFFF);
            sum += (sum >> 16);
            return (unsigned short)(-sum);
        }

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

        struct icmphdr icmp_header;
        icmp_header.type = ICMP_ECHO;
        icmp_header.code = 0;
        icmp_header.checksum = 0;
        icmp_header.un.echo.id = 0;
        icmp_header.un.echo.sequence = 0;

        icmp_header.checksum = checksum((unsigned short *)&icmp_header, sizeof(icmp_header));

        struct ip ip_header;
        ip_header.ip_hl = 5;
        ip_header.ip_v = 4;
        ip_header.ip_tos = 0;
        ip_header.ip_len = sizeof(struct ip) + sizeof(struct icmphdr);
        ip_header.ip_id = htons(0);
        ip_header.ip_off = 0;
        ip_header.ip_ttl = 64;
        ip_header.ip_p = IPPROTO_ICMP;
        ip_header.ip_src.s_addr = INADDR_ANY;
        ip_header.ip_dst = dest_addr.sin_addr;

        sendto(sockfd, &icmp_header, sizeof(struct icmphdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        std::cout << "Sending ICMP echo request to " << address << std::endl;

        close(sockfd); // Close the socket after sending the request
        return BOOL_VAL(true); // Successfully sent the ping request
#endif
    }

public:
    static void define_network_natives() {
        Natives::define_native("connect", connect_native);
        Natives::define_native("ping", ping_native);
    }
};
