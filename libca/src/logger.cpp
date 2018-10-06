#include "logger.h"
#include <ca/i386.h>
#include <iostream>

namespace ca
{

static auto to_string(const log_level ll)
{
    switch (ll)
    {
        case log_level::warning:
            return "warning";
        case log_level::error:
            return "error";
    }
    return "unknown";
}

void log(log_level ll, const i386 &i, const std::string &message)
{
    std::cerr << to_string(ll) << ": " << i.line_number() << ": " << message << ": `" << i.line_text() << "`\n";
}

void log(log_level ll, const int line_no, const std::string &line, const std::string &message)
{
    std::cerr << to_string(ll) << ": " << line_no << ": " << message << ": `" << line << "`\n";
}

} // namespace ca
