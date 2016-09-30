#ifndef THOR_TRACER__SCENE_CACHE_H
#define THOR_TRACER__SCENE_CACHE_H

#include <CL/cl.hpp>

namespace Thor {
namespace Tracer {

class Scene;

class SceneCache {
private:
    const Scene &m_scene;
    cl::Image2D m_output;
    cl::Buffer m_pixelCounter;
    cl::Buffer m_sphereBuffer;
    cl::Buffer m_cameraBuffer;
    cl::Buffer m_lightBuffer;
public:
    SceneCache(const Scene &scene) : m_scene(scene) {}

    const Scene &scene() const { return m_scene; }

    cl::Image2D &outputBuffer() { return m_output; }
    cl::Buffer &pixelCounter() { return m_pixelCounter; }
    cl::Buffer &sphereBuffer() { return m_sphereBuffer; }
    cl::Buffer &cameraBuffer() { return m_cameraBuffer; }
    cl::Buffer &lightBuffer() { return m_lightBuffer; }
};

}  // namespace Tracer
}  // namespace Thor

#endif
