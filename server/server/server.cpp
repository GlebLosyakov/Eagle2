#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

struct Task {
    double start;
    double end;
};

int main() {
    // 1. Инициализация WinSock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed. Error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // 2. Создание сокета
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // РЕШЕНИЕ ОШИБКИ 10048: Разрешаем повторное использование адреса
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    // 3. Настройка адреса (используем порт 9090 для надежности)
    sockaddr_in server_addr = { AF_INET, htons(9090), INADDR_ANY };

    // 4. Привязка
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error: " << WSAGetLastError() << "\nCheck if the port is busy!" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        system("pause");
        return 1;
    }

    listen(server_fd, 5);

    const int TOTAL_CLIENTS = 2; // Для теста поставим 2
    std::vector<SOCKET> clients;

    std::cout << "SERVER STARTED ON PORT 9090\n";
    std::cout << "Waiting for " << TOTAL_CLIENTS << " clients to connect...\n";

    while (clients.size() < TOTAL_CLIENTS) {
        SOCKET client_sock = accept(server_fd, NULL, NULL);
        if (client_sock != INVALID_SOCKET) {
            clients.push_back(client_sock);
            std::cout << "Client " << clients.size() << " connected!\n";
        }
    }

    // 5. Распределение задач
    std::cout << "All clients connected. Calculating...\n";
    double global_start = 1.0, global_end = 100.0;
    double step = (global_end - global_start) / TOTAL_CLIENTS;
    double final_result = 0.0;

    for (int i = 0; i < TOTAL_CLIENTS; ++i) {
        Task t = { global_start + i * step, global_start + (i + 1) * step };
        send(clients[i], (char*)&t, sizeof(t), 0);
    }

    // 6. Сбор результатов
    for (int i = 0; i < TOTAL_CLIENTS; ++i) {
        double res;
        recv(clients[i], (char*)&res, sizeof(res), 0);
        final_result += res;
        std::cout << "Received " << res << " from client " << i + 1 << "\n";
        closesocket(clients[i]);
    }

    std::cout << "-------------------------------------\n";
    std::cout << "Final Result of Integral: " << final_result << std::endl;
    std::cout << "-------------------------------------\n";

    closesocket(server_fd);
    WSACleanup();
    system("pause");
    return 0;
}