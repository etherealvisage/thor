#ifndef THOR_TRACER__CAMERA_H
#define THOR_TRACER__CAMERA_H

#include "math/Vector.h"
#include "math/Quaternion.h"

namespace Thor {
namespace Tracer {

class Camera {
private:
    Math::Vector m_pos;
    Math::Quaternion m_rot;
public:
    Camera(Math::Vector pos, Math::Quaternion rot) : m_pos(pos), m_rot(rot) {}

    const Math::Vector &pos() const { return m_pos; }
    const Math::Quaternion &rot() const { return m_rot; }
};

}  // namespace Tracer
}  // namespace Thor

#endif
