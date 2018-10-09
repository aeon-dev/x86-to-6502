#pragma once

#include <string>

namespace ca
{

class intel_386;

enum class log_level
{
    warning,
    error
};

void log(log_level ll, const intel_386 &i, const std::string &message);
void log(log_level ll, const int line_no, const std::string &line, const std::string &message);

} // namespace ca
