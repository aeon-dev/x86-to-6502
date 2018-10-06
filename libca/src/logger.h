#pragma once

#include <string>

namespace ca
{

class i386;

enum class log_level
{
    warning,
    error
};

void log(log_level ll, const i386 &i, const std::string &message);
void log(log_level ll, const int line_no, const std::string &line, const std::string &message);

} // namespace ca
