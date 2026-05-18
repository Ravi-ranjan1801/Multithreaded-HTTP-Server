#include <iostream>
#include <string>
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

    std::cout << "Socket created successfully\n";

    // Server address
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(serverSocket,
             (struct sockaddr*)&serverAddress,
             sizeof(serverAddress)) < 0) {

        std::cerr << "Bind failed\n";
        return 1;
    }

    std::cout << "Bind successful on port 8080\n";

    // Listen
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }

    std::cout << "Server listening on port 8080...\n";

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
            std::cerr << "Client accept failed\n";
            continue;
        }

        std::cout << "Client connected\n";

        // HTTP response
        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html>"
            "<head><title>Custom HTTP Server</title></head>"
            "<body>"
            "<h1>Custom HTTP Server Running</h1>"
            "</body>"
            "</html>";

        // Send response
        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0
        );

        std::cout << "Response sent successfully\n";

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}