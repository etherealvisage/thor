#ifndef THOR_RESOURCE__FILE_H
#define THOR_RESOURCE__FILE_H

#include <string>
#include <sstream>
#include <vector>

#include "Element.h"

namespace Thor {
namespace Resource {

class File { RESOURCE_ELEMENT_CLASS(File)
    static std::shared_ptr<File> loadElement(std::string identifier) {
        return std::make_shared<File>(identifier);
    }
private:
    std::string m_filename;
    bool m_exists, m_loaded;
    std::string m_content;
public:
    File(std::string filename);

    bool exists() const { return m_exists; }

    void load();

    void contentStream(std::stringstream &tofill);
    const std::string &contentString() { load(); return m_content; }
    std::vector<std::string> fileLines();
private:
    
};

}  // namespace Resource
}  // namespace Thor

#endif
