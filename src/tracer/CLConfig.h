#ifndef THOR_TRACER__CLCONFIG_H
#define THOR_TRACER__CLCONFIG_H

#include <string>

namespace Thor {
namespace Tracer {

class CLConfig {
private:
    std::string m_vendorName;
    std::string m_deviceName;
public:
    CLConfig(std::string vendorName, std::string deviceName)
        : m_vendorName(vendorName), m_deviceName(deviceName) {}

    const std::string &vendorName() const { return m_vendorName; }
    const std::string &deviceName() const { return m_deviceName; }
};

}  // namespace Tracer
}  // namespace Thor

#endif
