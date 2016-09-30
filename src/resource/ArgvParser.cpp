#include <vector>

#include "ArgvParser.h"

#include "XMLInterface.h"

#include "MessageSystem.h"


namespace Thor {
namespace Resource {

void ArgvParser::parse(std::shared_ptr<XML> xml, const char **argv) {
    int arg = 1;
    xml->node("/config/runtime/exec-path").text().set(argv[0]);

    std::vector<std::string> arguments;
    while(argv[arg]) {
        std::string sarg = argv[arg++];

        if(sarg == "--") break;

        if(sarg.length() > 2 && sarg[0] == '-' && sarg[1] == '-') {
            std::string long_form = sarg.substr(2);

            auto argnode = xml->getNode("/config/arguments/" + long_form);
            if(!argnode) {
                Message3(Resource, Error, "Unknown argument \"" << long_form << "\".");
                // skip processing.
                continue;
            }

            std::string target = xml->getNode(
                    "/config/arguments/" + long_form + "/target"
                ).text().as_string();
            if(target == "") {
                Message3(Resource, Error, "Argument " << long_form
                    << " missing target.");
                continue;
            }
            std::string type = xml->getNode(
                "/config/arguments/" + long_form + "/type").text().as_string();
            if(type == "") {
                Message3(Resource, Error, "Argument " << long_form
                    << " missing type.");
                continue;
            }
            else if(type == "set") {
                std::string value = xml->getNode(
                        "/config/arguments/" + long_form + "/value"
                    ).text().as_string();
                if(value == "") {
                    Message3(Resource, Error, "Argument " << long_form
                        << " missing value.");
                    continue;
                }
                xml->node(target).text().set(value.c_str());
            }
            else if(type == "replace") {
                if(argv[arg] == NULL) {
                    Message3(Resource, Error, "Argument " << long_form
                        << " expects following value.");
                    continue;
                }
                xml->node(target).text().set(argv[arg++]);
            }
            else {
                Message3(Resource, Error, "Argument " << long_form
                    << " has unknown type.");
                continue;
            }
        }
        else arguments.push_back(sarg);
    }

    while(argv[arg]) arguments.push_back(argv[arg++]);

    xml->node("/config/runtime/arguments").text().set(
        XMLSerialize(arguments).c_str());
}

}  // namespace Resource
}  // namespace Thor
