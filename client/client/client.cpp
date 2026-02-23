#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <cmath>
#include <string>

#pragma comment(lib, "ws2_32.lib")

struct Task {
    double start;
    double end;
};

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    std::string ip;
    int port;
    std::cout << "Enter Server IP: "; std::cin >> ip;
    std::cout << "Enter Port: "; std::cin >> port;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = { AF_INET, htons(port) };
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        return 1;
    }

    Task t;
    recv(sock, (char*)&t, sizeof(t), 0);
    std::cout << "Received task: range [" << t.start << ", " << t.end << "]\n";

    // Вычисление интеграла методом прямоугольников
    double result = 0.0;
    double h = 0.00001; // Шаг интегрирования
    for (double x = t.start; x < t.end; x += h) {
        result += sqrt(x) * h;
    }

    send(sock, (char*)&result, sizeof(result), 0);
    std::cout << "Result sent: " << result << std::endl;

    closesocket(sock);
    WSACleanup();
    system("pause");
    return 0;
}