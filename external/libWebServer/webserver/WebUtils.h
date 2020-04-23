#ifndef LIBWEBSERVER_WEBUTILS_H
#define LIBWEBSERVER_WEBUTILS_H

#include <vector>
#include <sstream>

namespace webserver {

//Length function to be used on constexpr
//the (&) is basically an anonomous variable. Could have been [maybe_unused] char const (&array) [N])
//Remember arrays can only be passed by reference hence the (&).
template <std::size_t N>
constexpr std::size_t length (char const (&) [N])
{
    return N-1;
}

inline std::vector<std::string> split (const std::string& s, char delim) {
    std::stringstream ss (s);
    std::string token;
    std::vector <std::string> retVal;
    while (std::getline (ss, token, delim)) {
        if (!token.empty()) {//we are skipping empty values (double delimiter)
            retVal.push_back(token);
        }
    }
    return retVal;
}


inline bool deUrlify (std::string& s) {
    int originalLength=s.size ();
    int newLength=0; //for now

    for (int i=0; i<originalLength; ++i) {
        char c = s[i];
        if (c == '%') {
            if (i+2>=originalLength) return false; //not enough room left
            char* temp;
            std::string valStr=s.substr(i+1, 2);
            auto val = std::strtol(valStr.c_str (),&temp,16);
            auto processedLen= temp - valStr.c_str ();
            if (processedLen != 2) return false;
            s[newLength++] = static_cast <char> (val);
            i+=2;
        } else {
            s[newLength++] = c;
        }
    };

    s.resize (newLength);
    return true;
}
} //end of namespace

#endif //LIBWEBSERVER_WEBUTILS_H
