namespace Thor {
namespace Resource {

template <class T>
struct HasXMLInterfaceImpl {
private:
    template <class U>
    static MP::true_ test(typename U::XMLInterfaceType *);
  
    template <class U>
    static MP::false_ test(...);
public:
    using type = decltype( test<T>(0) );
};

template <class T>
struct HasXMLInterface : HasXMLInterfaceImpl<T>::type {};

template <>
struct HasXMLInterface<int> : MP::false_ {};

template <>
struct HasXMLInterface<double> : MP::false_ {};

template <>
struct HasXMLInterface<bool> : MP::false_ {};

template <class T, class Ret = T, bool HasInterface =
    HasXMLInterface<T>::value>
struct Interface {
    using None = Ret;
    static_assert(!HasInterface, "Overload resolution check");
};

template <class T, class Ret>
struct Interface<std::vector<T>, Ret, false> {
    using Vector = Ret;
};

template <class ...Args, class Ret>
struct Interface<std::tuple<Args...>, Ret, false> {
    using Tuple = Ret;
};

template <class T, class Ret>
struct Interface<T, Ret, true> {
    using Simple = Ret;
};

/* Now for tuple specializations */

template <std::size_t Index, typename T, typename Ret = void,
    bool AtStart = (Index == std::tuple_size<T>::value)>
struct TuplePos {
    using Rest = Ret;
  
    static_assert(!AtStart, "Overload resolution check");
};

template <std::size_t Index, typename T, typename Ret>
struct TuplePos<Index, T, Ret, true> {
    using Start = Ret;
};

}  // namespace Resource
}  // namespace Thor
