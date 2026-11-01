#include "../include/RequestHandler.h"
#include "../include/FileUtils.h"
#include "../include/Logger.h"

#include <iostream>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <cerrno>
#include <sstream>
#include <string>

// Seconds a client may stay silent before the worker gives up on it
#ifndef CLIENT_TIMEOUT_SECONDS
#define CLIENT_TIMEOUT_SECONDS 5
#endif


void handleClient(int clientSocket) {

  logMessage("Client connected: ");

      // Stop recv() and send() from blocking forever on a slow or idle client
    timeval timeout{};
    timeout.tv_sec = CLIENT_TIMEOUT_SECONDS;
    timeout.tv_usec = 0;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    char buffer[4096] = {0};

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer),
        0
    );

        if (bytesReceived < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            logMessage("Response: 408 Request Timeout");

            const std::string timedOut =
                "HTTP/1.1 408 Request Timeout\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n"
                "Content-Length: 16\r\n"
                "\r\n"
                "Request Timeout\n";

            send(clientSocket, timedOut.c_str(), timedOut.size(), 0);
        } else {
            std::cerr << "Receive failed\n";
        }
        close(clientSocket);
        return;
    }

    std::string request(buffer);

    std::string body;

size_t bodyPos = request.find("\r\n\r\n");

if (bodyPos != std::string::npos) {
    body = request.substr(bodyPos + 4);
}

    std::stringstream requestStream(request);

    std::string method;
    std::string path;
    std::string version;

    requestStream >> method >> path >> version;
    
    if (method.empty() || path.empty()) {

    logMessage("Response: 400 Bad Request");

    std::string response =
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Bad Request";

    send(
        clientSocket,
        response.c_str(),
        response.size(),
        0
    );

    close(clientSocket);
    return;
}

    logMessage("Request: " + method + " " + path);

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
        "Content-Length: " +
        std::to_string(fileContent.size()) +
        "\r\n\r\n" +
        fileContent;
}
    else if (path == "/health") {
        
 std::string body = "Server is healthy";

    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " +
        std::to_string(body.size()) +
        "\r\n\r\n" +
        body;
}
else if (path == "/api/health") {

    std::string body =
        "{\"status\":\"healthy\","
        "\"server\":\"CustomHTTPServer\","
        "\"version\":\"1.0\"}";

    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " +
        std::to_string(body.size()) +
        "\r\n\r\n" +
        body;
}

else if (path == "/api/echo" && method == "POST") {

    logMessage("Response: 200 OK");
std::string bodyResponse =
    "{\"message\":\"" + body + "\"}";

response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: " +
    std::to_string(bodyResponse.size()) +
    "\r\n\r\n" +
    bodyResponse;
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