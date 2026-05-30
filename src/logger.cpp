#include "../include/Logger.h"

#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string getCurrentTime() {

    std::time_t now = std::time(nullptr);

    std::tm* localTime = std::localtime(&now);

    std::stringstream timeStream;

    timeStream << std::put_time(localTime,
                                "%Y-%m-%d %H:%M:%S");

    return timeStream.str();
}

void logMessage(const std::string& message) {

    std::cout
        << "[" << getCurrentTime() << "] "
        << message
        << std::endl;
}