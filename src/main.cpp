#include <iostream>
#include <memory>

#include "tracer/Interface.h"
#include "tracer/Scene.h"
#include "tracer/Sphere.h"

#include "resource/XML.h"
#include "resource/Registry.h"

using namespace Thor;

int main() {
    std::shared_ptr<Resource::XML> config
        = Resource::Registry::get<Resource::XML>("config.xml");

    std::string vendor
        = config->getNode("/config/cl").attribute("vendor").as_string("");
    std::string device
        = config->getNode("/config/cl").attribute("device").as_string("");

    Tracer::CLConfig clc(vendor, device);
    Tracer::Interface tinterface(clc, Tracer::OutputConfig(800, 600));

    Tracer::Scene scene;

    scene.addSceneObject(new Tracer::Sphere(Math::Vector(10,0,0), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(-10,0,10), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,10,0), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,-10,10), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,0,10), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,0,-10), 3));

    tinterface.render(scene);

    return 0;
}
