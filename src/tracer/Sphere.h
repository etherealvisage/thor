#ifndef THOR_TRACER__SPHERE_H
#define THOR_TRACER__SPHERE_H

#include "SceneObject.h"

namespace Thor {
namespace Tracer {

class Sphere : public SceneObject {
private:
    Math::Vector m_origin;
    double m_radius;
    Math::Vector m_colour;
    double m_reflectivity;
public:
    Sphere(const Math::Vector &origin, double radius,
        const Math::Vector &colour, double reflectivity) : m_origin(origin), m_radius(radius),
            m_colour(colour), m_reflectivity(reflectivity) {}

    const Math::Vector &origin() const { return m_origin; }
    double radius() const { return m_radius; }
    const Math::Vector &colour() const { return m_colour; }
    double reflectivity() const { return m_reflectivity; }

    virtual void boundingBox(Math::Vector &a, Math::Vector &b) const {
        a = m_origin - Math::Vector(m_radius, m_radius, m_radius);
        b = m_origin + Math::Vector(m_radius, m_radius, m_radius);
    }
};

}  // namespace Tracer
}  // namespace Thor

#endif
