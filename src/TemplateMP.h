#ifndef THOR__TEMPLATE_MP_H
#define THOR__TEMPLATE_MP_H

#include <vector>
#include <tuple>
#include <type_traits>

namespace Thor {
namespace MP {

template <bool Val>
using bool_ = std::integral_constant<bool, Val>;

using true_ = bool_<true>;
using false_ = bool_<false>;

template<typename T>
struct is_vector : false_ {};

template<typename T>
struct is_vector<std::vector<T>> : true_ {};

template<typename T>
struct is_tuple : false_ {};

template<typename... T>
struct is_tuple<std::tuple<T...>> : true_ {};

}  // namespace MP
}  // namespace Thor

#endif
