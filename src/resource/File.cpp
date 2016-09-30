#include <fstream>

#include "File.h"

namespace Thor {
namespace Resource {

RESOURCE_ELEMENT_IMPL(File)

File::File(std::string filename) : m_filename(filename) {
    std::ifstream f(filename.c_str());
    if(f.is_open()) m_exists = true;
    else m_exists = false;
    m_loaded = false;
}

void File::load() {
    if(m_loaded) return;

    std::ifstream f(m_filename.c_str());

    f.seekg (0, f.end);
    int length = f.tellg();
    f.seekg (0, f.beg);

    char *buffer = new char[length];
    f.read(buffer, length);
    m_content = std::string(buffer, length);
    delete[] buffer;

    m_loaded = true;
}

void File::contentStream(std::stringstream &tofill) {
    load();
    tofill << m_content;
}

std::vector<std::string> File::fileLines() {
    if(!m_loaded) load();

    std::vector<std::string> lines;

    /*boost::split(lines, m_content, boost::algorithm::is_any_of("\n\r"),
        boost::algorithm::token_compress_off);*/
    std::string::size_type cursor = 0;
    do {
        std::string::size_type end = m_content.find_first_of("\n\r", cursor);
        lines.push_back(m_content.substr(cursor, end));
        cursor = end;
    } while(cursor != std::string::npos);

    return lines;
}


}  // namespace Resource
}  // namespace Thor
