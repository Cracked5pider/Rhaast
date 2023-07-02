#ifndef RHAAST_UTILS_H
#define RHAAST_UTILS_H

#include <string>
#include <vector>
#include <sstream>
#include <stringapiset.h>

bool StringIsHex(
    std::string const& s
);

bool StringIsNumber(
    const std::string& s
);

void StringTokenize(
    std::string const&        str,
    const char                delim,
    std::vector<std::string>& out
);

std::wstring StringAnsiToWide(
    const std::string& str
);

#endif