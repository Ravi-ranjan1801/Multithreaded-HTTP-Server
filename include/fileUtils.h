#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>

std::string readFile(const std::string& filePath);

std::string getContentType(const std::string& path);

#endif