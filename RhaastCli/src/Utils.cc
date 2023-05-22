#include <Utils.h>

bool StringIsNumber( const std::string& s )
{
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