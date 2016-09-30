#ifndef THOR_RESOURCE__REGISTRY_H
#define THOR_RESOURCE__REGISTRY_H

#include <map>
#include <memory>
#include <string>

#include "MessageSystem.h"

namespace Thor {
namespace Resource {

class Registry {
private:
    // prevent construction
    Registry() {}
public:
    template<typename ResourceType>
    static void alias(std::string existing, std::string newname) {

        auto &reg = ResourceType::s_registryElements;

        reg[newname] = reg[existing];
    }

    template<typename ResourceType>
    static std::shared_ptr<ResourceType> get(std::string identifier) {
        auto &reg = ResourceType::s_registryElements;

        auto ir = reg.find(identifier);

        std::shared_ptr<ResourceType> result;
        if(ir == reg.end()) {
            result = ResourceType::loadElement(identifier);
            if(!result) {
                Message3(Resource, Error,
                    "Could not locate resource with identifier \""
                    << identifier << "\"");
            }
            reg[identifier] = result;
        }
        else result = (*ir).second;

        return std::dynamic_pointer_cast<ResourceType>(result);
    }

    template<typename ResourceType>
    static void unload(std::string identifier) {
        auto &reg = ResourceType::s_registryElements;
        
        auto ir = reg.find(identifier);
        if(ir != reg.end()) reg.erase(ir);
    }
};

}  // namespace Resource
}  // namespace Thor

#endif
