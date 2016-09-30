#include <iostream>
#include <fstream>
#include <utility>
#include <CL/cl.hpp>

#include "Interface.h"
#include "Sphere.h"
#include "Light.h"

#include "resource/File.h"
#include "resource/Registry.h"

#include "MessageSystem.h"

namespace Thor {
namespace Tracer {

Interface::Interface(const CLConfig &clConfig, const OutputConfig &outConfig)
    : m_outputConfig(outConfig) {

    cl_int err;
    std::vector<cl::Platform> platformList;
    cl::Platform::get(&platformList);

    bool deviceFound = false;

    for(auto &plat : platformList) {
        std::string vendor;
        plat.getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &vendor);
        if(vendor.size() > 0 && vendor.back() == 0) vendor.pop_back();
        if(vendor != clConfig.vendorName()) continue;

        m_platform = plat;

        std::vector<cl::Device> deviceList;
        plat.getDevices(CL_DEVICE_TYPE_ALL, &deviceList);

        for(auto dev : deviceList) {
            std::string name;
            dev.getInfo(CL_DEVICE_NAME, &name);
            if(name.size() > 0 && name.back() == 0) name.pop_back();
            if(name != clConfig.deviceName()) continue;

            m_device = dev;
            deviceFound = true;
            break;
        }
        if(deviceFound) break;
    }

    if(!deviceFound) {
        Message3(Tracer, Fatal, "Could not find OpenCL device.");
    }

    Message3(Tracer, Debug, "Found OpenCL device!");

    cl_context_properties cprops[3]
        = {CL_CONTEXT_PLATFORM, (cl_context_properties)(m_platform)(), 0};

    m_context = cl::Context(m_device, cprops, NULL, NULL, &err);

    if(err != CL_SUCCESS) {
        Message3(Tracer, Fatal, "Failed to create context!");
    }

    m_queue = cl::CommandQueue(m_context, m_device);

    // create kernel
    std::string kernelSource
        = Resource::Registry::get<Resource::File>(
            "data/tracer.cl")->contentString();
    m_tracerProgram = cl::Program(m_context, cl::Program::Sources(1,
        std::make_pair(kernelSource.c_str(), kernelSource.length()+1)));

    err = m_tracerProgram.build();
    //if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to build program!");

    cl_build_status buildStatus;
    do {
        m_tracerProgram.getBuildInfo(m_device, CL_PROGRAM_BUILD_STATUS,
            &buildStatus);
        if(buildStatus == CL_BUILD_SUCCESS) break;
        else if(buildStatus == CL_BUILD_ERROR) {
            std::string log;
            m_tracerProgram.getBuildInfo(m_device, CL_PROGRAM_BUILD_LOG, &log);
            Message3(Tracer, Error, "Build error for tracer kernel:\n" + log);

            Message3(Tracer, Fatal, "Failed to build tracer kernel!");
        }
        else if(buildStatus == CL_BUILD_NONE) {
            Message3(Tracer, Fatal, "No building?!?");
        }
    } while(1);

    m_tracerKernel = cl::Kernel(m_tracerProgram, "tracer", &err);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to create kernel!");
}

Interface::~Interface() {
    
}

void Interface::render(Scene &scene, void *pxdata) {
    cl_int err;
    cl::Image2D &out = scene.cache().outputBuffer();
    cl::Event pixelCopy, cameraCopy, sphereCopy, lightCopy;

    if(out() == nullptr) {
        cl::ImageFormat format;
        format.image_channel_order = CL_RGBA;
        format.image_channel_data_type = CL_UNORM_INT8;
        out = cl::Image2D(m_context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY,
            format, m_outputConfig.width(), m_outputConfig.height(), 0,
            nullptr, &err);
        if(out() == nullptr) {
            Message3(Tracer, Fatal, "Couldn't create out image!");
        }
        else if(err != CL_SUCCESS) {
            Message3(Tracer, Fatal, "Couldn't create out image: " << err);
        }
    }

    cl::Buffer &pixelCounter = scene.cache().pixelCounter();
    if(pixelCounter() == nullptr) {
        pixelCounter = cl::Buffer(m_context,
            CL_MEM_READ_WRITE | CL_MEM_HOST_WRITE_ONLY, sizeof(int));

    }
    {
        int zero = 0;
        m_queue.enqueueWriteBuffer(pixelCounter, false, 0, sizeof(int),
            &zero, NULL, &pixelCopy);
    }

    cl::Buffer &camera = scene.cache().cameraBuffer();
    if(camera() == nullptr) {
        camera = cl::Buffer(m_context,
            CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(float) * 8);
    }

    // Copy camera representation in
    {
        float camera_data[8] = {
            (float)scene.camera().pos().x(),
            (float)scene.camera().pos().y(),
            (float)scene.camera().pos().z(),
            (float)60.0,

            (float)scene.camera().rot().s(),
            (float)scene.camera().rot().v().x(),
            (float)scene.camera().rot().v().y(),
            (float)scene.camera().rot().v().z()
        };

        m_queue.enqueueWriteBuffer(camera, false, 0, sizeof(camera_data),
            camera_data, NULL, &cameraCopy);
    }

    cl::Buffer &spheres = scene.cache().sphereBuffer();
    cl::Buffer &lights = scene.cache().lightBuffer();
    #if 0
    if(spheres() == nullptr) {
        spheres = cl::Buffer(m_context,
            CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(float) * 4);
    }

    // Copy sphere information
    {
        float sphere_data[4] = {
            0, 0, 10,
            3
        };
        m_queue.enqueueWriteBuffer(spheres, false, 0, sizeof(sphere_data),
            sphere_data, nullptr, &sphereCopy);
    }
    #endif
    int sphereCount = 0;
    int lightCount = 0;
    {
        std::vector<float> lightData, sphereData;
        for(auto object : scene.sceneObjects()) {
            auto sphere = dynamic_cast<Sphere *>(object);
            if(sphere) {
                sphereData.push_back(sphere->origin().x());
                sphereData.push_back(sphere->origin().y());
                sphereData.push_back(sphere->origin().z());
                sphereData.push_back(sphere->radius());
                sphereData.push_back(sphere->colour().x());
                sphereData.push_back(sphere->colour().y());
                sphereData.push_back(sphere->colour().z());
                sphereData.push_back(sphere->reflectivity());
                sphereCount ++;
            }
            auto light = dynamic_cast<Light *>(object);
            if(light) {
                lightData.push_back(light->pos().x());
                lightData.push_back(light->pos().y());
                lightData.push_back(light->pos().z());
                lightData.push_back(light->ambColour().x());
                lightData.push_back(light->ambColour().y());
                lightData.push_back(light->ambColour().z());
                lightData.push_back(light->diffColour().x());
                lightData.push_back(light->diffColour().y());
                lightData.push_back(light->diffColour().z());
                lightData.push_back(light->specColour().x());
                lightData.push_back(light->specColour().y());
                lightData.push_back(light->specColour().z());
                lightCount ++;
            }
        }
        spheres = cl::Buffer(m_context,
            CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY,
            sizeof(float) * sphereData.size());
        m_queue.enqueueWriteBuffer(spheres, false, 0,
            sizeof(float) * sphereData.size(), &sphereData[0], nullptr,
            &sphereCopy);
        lights = cl::Buffer(m_context,
            CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY,
            sizeof(float) * lightData.size());
        m_queue.enqueueWriteBuffer(lights, false, 0,
            sizeof(float) * lightData.size(), &lightData[0], nullptr,
            &lightCopy);
    }

    pixelCopy.wait();
    cameraCopy.wait();
    sphereCopy.wait();
    lightCopy.wait();

    err = m_tracerKernel.setArg(0, out);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to set argument?");
    err = m_tracerKernel.setArg(1, pixelCounter);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to set argument?");
    err = m_tracerKernel.setArg(2, camera);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to set argument?");
    err = m_tracerKernel.setArg(3, spheres);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to set argument?");
    err = m_tracerKernel.setArg(4, sphereCount);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to set argument?");
    err = m_tracerKernel.setArg(5, lights);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to set argument?");
    err = m_tracerKernel.setArg(6, lightCount);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to set argument?");

    cl::Event event;
    m_queue.enqueueNDRangeKernel(m_tracerKernel, cl::NullRange,
        cl::NDRange(1024), cl::NDRange(32),
        NULL, &event);

    event.wait();

    cl::size_t<3> origin, size;
    origin[0] = 0, origin[1] = 0, origin[2] = 0;
    size[0] = 800, size[1] = 600, size[2] = 1;
    err = m_queue.enqueueReadImage(out, true, origin, size, 0, 0, pxdata, nullptr, &event);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to read image? " << err);

    event.wait();

    /*unsigned char *data = new unsigned char[800*600*4];
    for(int i = 0; i < 800*600*4; i ++) data[i] = 0;

    cl::size_t<3> origin, size;
    origin[0] = 0, origin[1] = 0, origin[2] = 0;
    size[0] = 800, size[1] = 600, size[2] = 1;
    err = m_queue.enqueueReadImage(out, true, origin, size, 0, 0, data, nullptr, &event);
    if(err != CL_SUCCESS) Message3(Tracer, Fatal, "Failed to read image? " << err);

    event.wait();

    std::memcpy(pxdata, data, 800*600*4);

    delete[] data;*/

    /*std::ofstream outimage("out.ppm");
    outimage << "P3" << std::endl << "800 600\n255\n";
    for(int i = 0; i < 800*600*4; i += 4) {
        outimage << ((data[i+0]) & 0xff) << " ";
        outimage << ((data[i+1]) & 0xff) << " ";
        outimage << ((data[i+2]) & 0xff) << " ";
    }*/
}

}  // namespace Tracer
}  // namespace Thor
