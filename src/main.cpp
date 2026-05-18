#include <iostream>
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

        // Accept client connection
        int clientSocket = accept(
            serverSocket,
            (struct sockaddr*)&clientAddress,
            &clientSize
        );

        if (clientSocket < 0) {
            std::cerr << "Client accept failed\n";
            continue;
        }

        std::cout << "Client connected successfully\n";

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}