#ifndef THOR_TRACER__LIGHT_H
#define THOR_TRACER__LIGHT_H

#include "math/Vector.h"

#include "SceneObject.h"

namespace Thor {
namespace Tracer {

class Light : public SceneObject {
private:
    Math::Vector m_pos;
    Math::Vector m_ambColour, m_diffColour, m_specColour;
public:
    Light(Math::Vector pos, Math::Vector ambColour, Math::Vector diffColour,
        Math::Vector specColour) : m_pos(pos), m_ambColour(ambColour),
        m_diffColour(diffColour), m_specColour(specColour) {}

    const Math::Vector &pos() const { return m_pos; }
    const Math::Vector &ambColour() const { return m_ambColour; }
    const Math::Vector &diffColour() const { return m_diffColour; }
    const Math::Vector &specColour() const { return m_specColour; }

    virtual void boundingBox(Math::Vector &a, Math::Vector &b) const {
        a = m_pos;
        b = m_pos;
    }
};

}  // namespace Tracer
}  // namespace Thor

#endif
