#include "Scene.h"
#include "SceneCache.h"

namespace Thor {
namespace Tracer {

Scene::Scene() : m_cache(new SceneCache(*this)),
    m_camera(Math::Vector(0,0,0), Math::Quaternion()) {

}

Scene::~Scene() {
    delete m_cache;
    for(auto object : m_sceneObjects) {
        delete object;
    }
}

}  // namespace Tracer
}  // namespace Thor
