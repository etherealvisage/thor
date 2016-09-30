

namespace Thor {
namespace Resource {

// forward declarations
template<typename T>
typename Interface<T>::None XMLParse(
        std::istringstream &from, bool &worked);

template<typename T>
typename Interface<T>::Vector XMLParse(
    std::istringstream &from, bool &worked);

template<typename T>
typename Interface<T>::Tuple XMLParse(
    std::istringstream &from, bool &worked);

template<typename T>
typename Interface<T>::Simple
    XMLParse(std::istringstream &from, bool &worked);

// vector specialization
template<typename T>
typename Interface<T>::Vector XMLParse(
    std::istringstream &from, bool &worked) {

    worked = false;
    // empty string -> empty vector
    if(!from.good()) { worked = true; return T(); }

    char next;
    from >> next;
    if(next != '[') return T();

    if(from.peek() == ']') {
        worked = true;
        return T();
    }
    if(!from.good()) return T();

    T result;

    while(from.good()) {
        result.push_back(
            XMLParse<typename T::value_type>(from, worked)
        );
        if(!worked) return T();

        if(!from.good()) { worked = false; break; }

        if(from.peek() == ']') { from.get(); worked = true; break; }
        if(from.peek() != ',') break;
        else from.get();
    }

    if(!worked) return T();
    return result;
}

template<std::size_t index, typename T>
typename TuplePos<index, T, void>::Start
XMLParseTupleHelper(std::istringstream& /*unused*/,
    bool &worked, T& /*unused*/) {
    // empty, nothing more to parse
    worked = true;
}

template<std::size_t index, typename T>
typename TuplePos<index, T, void>::Rest
XMLParseTupleHelper(std::istringstream &from, bool &worked, T &result) {

    std::get<index>(result) = XMLParse<
        typename std::tuple_element<index, T>::type>(from, worked);

    // if not last, expect a comma.
    if(index+1 != std::tuple_size<T>::value) {
        if(!from.good()) { worked = false; return; }
        if(from.get() != ',') { worked = false; return; }
    }
    
    XMLParseTupleHelper<index+1, T>(from, worked, result);
}

template<typename T>
T XMLParseTupleHelper(std::istringstream &from, bool &worked) {
    T result;

    XMLParseTupleHelper<0, T>(from, worked, result);

    return result;
}

// tuple specialization
template<typename T>
typename Interface<T>::Tuple XMLParse(
    std::istringstream &from, bool &worked) {

    worked = false;
    if(!from.good()) return T();

    char next;
    from >> next;
    if(next != '(') return T();

    if(from.peek() == ')') {
        worked = true;
        return T();
    }
    if(!from.good()) return T();

    return XMLParseTupleHelper<T>(from, worked);
}

// simple interface specialization
template<typename T>
typename Interface<T>::Simple
    XMLParse(std::istringstream &from, bool &worked) {

    return T::XMLInterfaceParse(
        XMLParse<typename T::XMLInterfaceType>(from, worked));
}

template<typename T>
T XMLParse(const std::string &from, bool &worked) {
    std::istringstream ss(from);

    return XMLParse<T>(ss, worked);
}

template<typename T>
T XMLParse(const std::string &from) {
    bool worked = false;
    return XMLParse<T>(from, worked);
}

}  // namespace Resource
}  // namespace Thor
