#ifndef THOR_TRACER__SPHERE_H
#define THOR_TRACER__SPHERE_H

#include "SceneObject.h"

namespace Thor {
namespace Tracer {

class Sphere : public SceneObject {
private:
    Math::Vector m_origin;
    double m_radius;
public:
    Sphere(const Math::Vector &origin, double radius)
        : m_origin(origin), m_radius(radius) {}

    virtual void boundingBox(Math::Vector &a, Math::Vector &b) const {
        a = m_origin - Math::Vector(m_radius, m_radius, m_radius);
        b = m_origin + Math::Vector(m_radius, m_radius, m_radius);
    }
};

}  // namespace Tracer
}  // namespace Thor

#endif
