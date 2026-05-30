#include "../include/RequestHandler.h"
#include "../include/FileUtils.h"
#include "../include/Logger.h"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <string>


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
else if (path == "/api/health") {

    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "{"
        "\"status\":\"healthy\","
        "\"server\":\"CustomHTTPServer\","
        "\"version\":\"1.0\""
        "}";
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