#include "../include/FileUtils.h"

#include <fstream>
#include <iterator>

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

    if (path.find(".css") != std::string::npos)
        return "text/css";

    if (path.find(".html") != std::string::npos)
        return "text/html";

    return "text/plain";
}