#ifndef THOR_TRACER__INTERFACE_H
#define THOR_TRACER__INTERFACE_H

#include <set>

#include <CL/cl.hpp>

#include "CLConfig.h"
#include "Scene.h"
#include "SceneCache.h"
#include "OutputConfig.h"

namespace Thor {
namespace Tracer {

class Interface {
private:
    cl::Platform m_platform;
    cl::Device m_device;
    cl::Context m_context;
    cl::CommandQueue m_queue;
    cl::Program m_tracerProgram;
    cl::Kernel m_tracerKernel;

    OutputConfig m_outputConfig;
public:
    Interface(const CLConfig &config, const OutputConfig &outConfig);
    ~Interface();

    const OutputConfig &outputConfig() { return m_outputConfig; }

    void render(Scene &scene);
};

}  // namespace Tracer
}  // namespace Thor

#endif
