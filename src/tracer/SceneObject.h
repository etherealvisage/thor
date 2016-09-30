#ifndef THOR_TRACER__SCENE_OBJECT_H
#define THOR_TRACER__SCENE_OBJECT_H

#include "math/Vector.h"

namespace Thor {
namespace Tracer {

class SceneObject {
public:
    virtual ~SceneObject() {}

    virtual void boundingBox(Math::Vector &a, Math::Vector &b) const = 0;
};

}  // namespace Tracer
}  // namespace Thor

#endif
