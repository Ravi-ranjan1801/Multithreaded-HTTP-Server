#include "../include/Logger.h"
#include "../include/FileUtils.h"
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


void handleClient(int clientSocket) {

  logMessage("Client connected");

    char buffer[4096] = {0};

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer),
        0
    );

    if (bytesReceived < 0) {
        std::cerr << "Receive failed\n";
        close(clientSocket);
        return;
    }

    std::string request(buffer);

    std::stringstream requestStream(request);

    std::string method;
    std::string path;
    std::string version;

    requestStream >> method >> path >> version;

    logMessage("Requested path: " + path);

    std::string filePath;

    if (path == "/") {
        filePath = "static/index.html";
    }
    else if (path == "/about") {
        filePath = "static/about.html";
    }
    else if (path == "/style.css") {
        filePath = "static/style.css";
    }

    std::string response;

    if (!filePath.empty()) {
        logMessage("Response: 200 OK");

        std::string fileContent = readFile(filePath);

        std::string contentType = getContentType(filePath);

        response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: " + contentType + "\r\n"
            "\r\n" +
            fileContent;
    }
    else if (path == "/health") {

        response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Server is healthy";
    }
    else {
        logMessage("Response: 404 Not Found");

        std::string fileContent =
            "<html><body><h1>404 Not Found</h1></body></html>";

        response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "\r\n" +
            fileContent;
    }

    send(
        clientSocket,
        response.c_str(),
        response.size(),
        0
    );

    close(clientSocket);
}

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