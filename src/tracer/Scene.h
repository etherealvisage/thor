#ifndef THOR_TRACER__SCENE_H
#define THOR_TRACER__SCENE_H

#include <vector>

#include "Camera.h"
#include "SceneObject.cpp"

namespace Thor {
namespace Tracer {

class SceneCache;

class Scene {
private:
    SceneCache *m_cache;

    Camera m_camera;

    std::vector<SceneObject *> m_sceneObjects;
public:
    Scene();
    ~Scene();

    SceneCache &cache() { return *m_cache; }
    const SceneCache &cache() const { return *m_cache; }

    const Camera &camera() const { return m_camera; }

    const std::vector<SceneObject *> &sceneObjects() const
        { return m_sceneObjects; }
    /* Takes ownership of the pointer it is given. */
    void addSceneObject(SceneObject *object)
        { m_sceneObjects.push_back(object); }
};

}  // namespace Tracer
}  // namespace Thor

#endif
