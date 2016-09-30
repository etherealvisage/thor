#include <sstream>

#include "Vector.h"

#include "StreamAsString.h"
#include "resource/XMLInterface.h"

namespace Thor {
namespace Math {

std::string Vector::toString() const {
    return StreamAsString() << "(" << m_x << "," << m_y << "," << m_z << ")";
}

}  // namespace Math
}  // namespace Thor
