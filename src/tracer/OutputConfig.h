#ifndef THOR_TRACER__OUTPUT_CONFIG_H
#define THOR_TRACER__OUTPUT_CONFIG_H

namespace Thor {
namespace Tracer {

class OutputConfig {
private:
    int m_width;
    int m_height;
public:
    OutputConfig(int width, int height) : m_width(width), m_height(height) {}

    int width() const { return m_width; }
    int height() const { return m_height; }
};

}  // namespace Tracer
}  // namespace Thor

#endif
