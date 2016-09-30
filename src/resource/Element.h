#ifndef THOR_RESOURCE__ELEMENT_H
#define THOR_RESOURCE__ELEMENT_H

#include <map>
#include <string>
#include <memory>

#define RESOURCE_ELEMENT_CLASS(type) \
    private: \
    friend class Thor::Resource::Registry; \
    static std::map<std::string, std::shared_ptr<type>> s_registryElements;

#define RESOURCE_ELEMENT_IMPL(type) \
    std::map<std::string, std::shared_ptr<type>> type::s_registryElements;

namespace Thor {
namespace Resource {

class Registry;

}  // namespace Resource
}  // namespace Thor

#endif
