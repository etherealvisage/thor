#include <sstream>
#include <cstring>

#include "XMLInterface.h"

namespace Thor {
namespace Resource {

// int functions
template<>
int XMLParse<int>(std::istringstream &from, bool &worked) {
    int value;
    (from >> value);
    worked = !from.fail();

    return value;
}

template<>
void XMLSerialize<int>(std::ostringstream &into, const int &object) {
    into << object;
}

// double functions
template<>
double XMLParse<double>(std::istringstream &from, bool &worked) {
    double value;
    from >> value;
    worked = !from.fail();

    return value;
}

template<>
void XMLSerialize<double>(std::ostringstream &into, const double &object) {
    into << object;
}

// std::string functions

template<>
std::string XMLParse<std::string>(std::istringstream &from, bool &worked) {
    std::string result;

    if(from.peek() != '"') {
        worked = false;
        return "";
    }
    from.get();

    while(from.good() && from.peek() != '"') result += from.get();

    worked = from.good();
    from.get();

    return result;
}

template<>
void XMLSerialize<std::string>(std::ostringstream &into,
    const std::string &object) {

    into << "\"" << object << "\"";
}

}  // namespace Resource
}  // namespace Thor
