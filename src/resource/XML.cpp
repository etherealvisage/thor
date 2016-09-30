#include "XML.h"
#include "File.h"
#include "Registry.h"

#include "MessageSystem.h"

namespace Thor {
namespace Resource {

RESOURCE_ELEMENT_IMPL(XML)

XML::XML(std::string filename) {
    auto file = Registry::get<File>(filename);
    if(!file || !file->exists()) {
        Message3(Resource, Error, "Could not find XML file \"" << filename
            << "\"");
        return;
    }

    auto result = m_doc.load_buffer(file->contentString().c_str(),
        file->contentString().length());

    if(!result) {
        Message3(Resource, Error, "Could not load XML resource " << filename 
            << ": " << result.description());
    }
}

pugi::xml_node XML::getNode(const std::string &path) const {
    auto n = m_doc.first_child().first_element_by_path(("/" + path).c_str());
    if(n) {
        if(n.first_child().type() != pugi::node_pcdata) {
            n.prepend_child(pugi::node_pcdata);
        }
        return n;
    }

    return pugi::xml_node();
}

pugi::xml_node XML::node(const std::string &path) {
    auto n = m_doc.first_child().first_element_by_path(("/" + path).c_str());
    if(n) {
        if(n.first_child().type() != pugi::node_pcdata) {
            n.prepend_child(pugi::node_pcdata);
        }
        return n;
    }

    auto lastslash = path.rfind("/");
    std::string shortname = path.substr(0, lastslash);
    n = node(shortname);

    auto nn = n.append_child();
    nn.set_name(path.substr(lastslash+1).c_str());

    return nn;
}

}  // namespace Resource
}  // namespace Thor
