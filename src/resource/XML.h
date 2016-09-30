#ifndef THOR_RESOURCE__XML_H
#define THOR_RESOURCE__XML_H

#include <pugixml.hpp>
#include <string>

#include "Element.h"

namespace Thor {
namespace Resource {

class XML { RESOURCE_ELEMENT_CLASS(XML)
    static std::shared_ptr<XML> loadElement(std::string identifier) {
        return std::make_shared<XML>(identifier);
    }
private:
    pugi::xml_document m_doc;
public:
    XML(std::string filename);

    pugi::xml_document &doc() { return m_doc; }
    const pugi::xml_document &doc() const { return m_doc; }

    pugi::xml_node getNode(const std::string &path) const;
    pugi::xml_node node(const std::string &path);
};

}  // namespace Resource
}  // namespace Thor

#endif
