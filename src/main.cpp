#include <iostream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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

        // Accept client
        int clientSocket = accept(
            serverSocket,
            (struct sockaddr*)&clientAddress,
            &clientSize
        );

        if (clientSocket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "\nClient connected\n";

        // Buffer to store request
        char buffer[4096] = {0};

        // Read request
        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer),
            0
        );

        if (bytesReceived < 0) {
            std::cerr << "Receive failed\n";
            close(clientSocket);
            continue;
        }

        // Convert request to string
        std::string request(buffer);

        // Print raw HTTP request
        std::cout << "\n===== HTTP REQUEST =====\n";
        std::cout << request << std::endl;

        // Parse request line
        std::stringstream requestStream(request);

        std::string method;
        std::string path;
        std::string version;

        requestStream >> method >> path >> version;

        std::cout << "Method: " << method << std::endl;
        std::cout << "Path: " << path << std::endl;
        std::cout << "Version: " << version << std::endl;

        // Response
       std::string response;

if (path == "/") {

    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html>"
        "<body>"
        "<h1>Welcome to Custom HTTP Server</h1>"
        "</body>"
        "</html>";

}
else if (path == "/about") {

    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html>"
        "<body>"
        "<h1>About Page</h1>"
        "<p>Built from scratch in C++</p>"
        "</body>"
        "</html>";

}
else if (path == "/health") {

    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Server is healthy";

}
else {

    response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html>"
        "<body>"
        "<h1>404 Not Found</h1>"
        "</body>"
        "</html>";
}

        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0
        );

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}