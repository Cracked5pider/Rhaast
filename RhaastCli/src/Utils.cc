#include <Utils.h>

bool StringIsHex(
    std::string const& s
) {
    return s.compare(0, 2, "0x") == 0
           && s.size() > 2
           && s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
}


bool StringIsNumber(
    const std::string& s
) {
    std::string::const_iterator it = s.begin();
    while ( it != s.end() && std::isdigit( *it ) ) ++it;
    return !s.empty() && it == s.end();
}

void StringTokenize(
    std::string const &str,
    const char delim,
    std::vector<std::string> &out
) {
    // construct a stream from the string
    std::stringstream ss( str );

    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
}

std::wstring StringAnsiToWide(
    const std::string& str
) {
    int          count = 0;
    std::wstring wstr  = {};

    count = MultiByteToWideChar( CP_ACP, 0, str.c_str(), str.length(), NULL, 0 );
    wstr  = std::wstring( count, 0 );

    MultiByteToWideChar( CP_ACP, 0, str.c_str(), str.length(), &wstr[ 0 ], count );

    return wstr;
}
