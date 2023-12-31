#include <iostream>
#include <WS2tcpip.h>
#include <random>
#include <string>
#include <thread>
#pragma comment(lib, "ws2_32.lib")

using namespace std;
sockaddr_in createSocket(int portNum, string ipAddress);
void registerToPublisher(string registerShape, int portNum, string ipAddress);
int generatePortNum();
const int registrationPortNum = 65535;

int main(int argc, char* argv[])
{
    WSADATA data;
    WORD version = MAKEWORD(2, 2);
    int wsOk = WSAStartup(version, &data);
    if (wsOk != 0)
    {
        cout << "Can't start Winsock! " << wsOk;
        return 1;
    }


    int portNum = generatePortNum();
    //string shape = argv[1];
    registerToPublisher("squares", portNum, "127.0.0.1");//squares only
    //registerToPublisher("circles", portNum, "127.0.0.1"); //squares & circles

    // wait one sec and then connect to portNum:
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sockaddr_in PortNumSocket = createSocket(portNum, "127.0.0.1");
    SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind(in, (sockaddr*)&PortNumSocket, sizeof(PortNumSocket)) == SOCKET_ERROR)
    {
        cout << "Can't bind socket to port " << portNum << "bc error number: " << WSAGetLastError() << endl;
        return 1;
    }
    cout << "Socket is bound  successfully to port " << portNum << endl;


    sockaddr_in client;
    int clientLength = sizeof(client);
    char buf[1024];

    while (true)
    {
        // Receive on port portNum
        ZeroMemory(&client, clientLength); //holds information about the client (sender) that sent the data.
        ZeroMemory(buf, 1024);

        int bytesIn55000 = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
        if (bytesIn55000 != SOCKET_ERROR)
        {
            char clientIp[256];
            ZeroMemory(clientIp, 256);
            inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);
            cout << "Message recv from " << clientIp << " on port " << portNum << ": " << buf << endl;
        }
    }

    // Close socket
    closesocket(in);

    // Shutdown winsock
    WSACleanup();
    return 0;
}

sockaddr_in createSocket(int portNum, string ipAddress) {
    sockaddr_in server;
    server.sin_family = AF_INET; // AF_INET = IPv4 addresses
    server.sin_port = htons(portNum); // Little to big endian conversion

    inet_pton(AF_INET, ipAddress.c_str(), &server.sin_addr); // Convert from string to byte array
    return server;
}

int generatePortNum() {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(49152, 65533);
    return distribution(gen);
}

void registerToPublisher(string registerShape, int portNum, string ipAddress) {
    sockaddr_in ListeningSocket = createSocket(registrationPortNum, "127.0.0.1");
    SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);
    cout << "tries to register with port number" << portNum << endl;
    string registrationMessage = "REGISTER " + registerShape + to_string(portNum);// Inform the publisher about the subscriber's port
    cout << "registration message: " << registrationMessage << endl;
    sendto(out, registrationMessage.c_str(), registrationMessage.size() + 1, 0, (sockaddr*)&ListeningSocket, sizeof(ListeningSocket));



}