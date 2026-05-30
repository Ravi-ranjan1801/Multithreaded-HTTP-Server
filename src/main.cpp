#include "../include/Logger.h"
#include "../include/FileUtils.h"
#include "../include/RequestHandler.h"
#include <iostream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <thread>
#include <vector>
#include <ctime>
#include <iomanip>
#include <queue>
#include <mutex>
#include <condition_variable>

std::queue<int> clientQueue;

std::mutex queueMutex;

std::condition_variable conditionVar;


void workerThread() {

    while (true) {

        int clientSocket;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            conditionVar.wait(lock, [] {
                return !clientQueue.empty();
            });

            clientSocket = clientQueue.front();

            clientQueue.pop();
        }

        handleClient(clientSocket);
    }
}

int main() {

    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(serverSocket,
             (struct sockaddr*)&serverAddress,
             sizeof(serverAddress)) < 0) {

        std::cerr << "Bind failed\n";
        return 1;
    }

    // Listen
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }

    logMessage("Server running on port 8080...");

    const int THREAD_COUNT = 4;

std::vector<std::thread> threadPool;

for (int i = 0; i < THREAD_COUNT; i++) {

    threadPool.emplace_back(workerThread);
}

    while (true) {

    sockaddr_in clientAddress{};
    socklen_t clientSize = sizeof(clientAddress);

    int clientSocket = accept(
        serverSocket,
        (struct sockaddr*)&clientAddress,
        &clientSize
    );

    if (clientSocket < 0) {
        std::cerr << "Accept failed\n";
        continue;
    }

    // Create thread for each client
   {
    std::lock_guard<std::mutex> lock(queueMutex);

    clientQueue.push(clientSocket);
}

conditionVar.notify_one();
}

    close(serverSocket);

    return 0;
}