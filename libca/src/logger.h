#pragma once

#include <string>

namespace ca
{

class i386;

enum class LogLevel
{
    Warning,
    Error
};

void log(LogLevel ll, const i386 &i, const std::string &message);
void log(LogLevel ll, const int line_no, const std::string &line, const std::string &message);

} // namespace ca
