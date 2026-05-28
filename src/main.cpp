#include <iostream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <thread>
#include <vector>

std::string readFile(const std::string& filePath) {

    std::ifstream file(filePath);

    if (!file.is_open()) {
        return "";
    }

    std::string content(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    return content;
}

std::string getContentType(const std::string& path) {

    if (path.find(".css") != std::string::npos) {
        return "text/css";
    }

    if (path.find(".html") != std::string::npos) {
        return "text/html";
    }

    return "text/plain";
}

void handleClient(int clientSocket) {

    std::cout << "\nClient connected\n";

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

    std::cout << "Path: " << path << std::endl;

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

    std::cout << "Server running on port 8080...\n";

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
    std::thread clientThread(
        handleClient,
        clientSocket
    );

    // Detach thread
    clientThread.detach();
}

    close(serverSocket);

    return 0;
}