#include "logger.h"
#include <ca/i386.h>
#include <iostream>

namespace ca
{

static auto to_string(const LogLevel ll)
{
    switch (ll)
    {
        case LogLevel::Warning:
            return "warning";
        case LogLevel::Error:
            return "error";
    }
    return "unknown";
}

void log(LogLevel ll, const i386 &i, const std::string &message)
{
    std::cerr << to_string(ll) << ": " << i.line_number() << ": " << message << ": `" << i.line_text() << "`\n";
}

void log(LogLevel ll, const int line_no, const std::string &line, const std::string &message)
{
    std::cerr << to_string(ll) << ": " << line_no << ": " << message << ": `" << line << "`\n";
}

} // namespace ca
