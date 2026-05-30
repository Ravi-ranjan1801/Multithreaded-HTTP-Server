# Multithreaded HTTP Server

A custom HTTP/1.1 server built from scratch in C++ using TCP socket programming, featuring request parsing, routing, static file serving, MIME type handling, structured logging, and thread-pool based concurrency.

## Features

- HTTP/1.1 request parsing
- Static file serving (HTML, CSS)
- Route handling
- MIME type detection
- JSON API endpoint support
- Structured server logging
- Thread-pool based concurrency model
- Modular architecture

## Tech Stack

- C++
- POSIX Sockets
- Multithreading (std::thread)
- Mutexes
- Condition Variables
- File I/O
- Linux (WSL Ubuntu)

## Project Structure

```text
custom-http-server/
│
├── include/
│   ├── Logger.h
│   ├── FileUtils.h
│   └── RequestHandler.h
│
├── src/
│   ├── main.cpp
│   ├── Logger.cpp
│   ├── FileUtils.cpp
│   └── RequestHandler.cpp
│
├── static/
│   ├── index.html
│   ├── about.html
│   └── style.css
│
└── build/
```

## Supported Routes

| Route | Description |
|---------|-------------|
| / | Home Page |
| /about | About Page |
| /health | Plain-text health check |
| /api/health | JSON API health check |

## Build

```bash
g++ src/main.cpp \
    src/Logger.cpp \
    src/FileUtils.cpp \
    src/RequestHandler.cpp \
    -o build/server -pthread
```

## Run

```bash
./build/server
```

Open:

```text
http://localhost:8080
```

## Concepts Demonstrated

- Socket Programming
- HTTP Protocol
- Request Parsing
- Multithreading
- Thread Pools
- Synchronization
- File Serving
- Backend Architecture
- Concurrent Client Handling

## Future Improvements

- Image Serving
- Configuration Files
- Persistent Connections
- Unit Testing

## Screenshots

### Home Page

![Home](screenshots/home.png)

### API Response

![API](screenshots/api.png)

### Server Logs

![Logs](screenshots/logs.png)