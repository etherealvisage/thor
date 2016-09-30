#ifndef THOR_RESOURCE__ARGV_PARSER_H
#define THOR_RESOURCE__ARGV_PARSER_H

#include "XML.h"

namespace Thor {
namespace Resource {

class ArgvParser {
public:
    void parse(std::shared_ptr<XML> configXML, const char **argv);
};

}  // namespace Resource
}  // namespace Thor

#endif
