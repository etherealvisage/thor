namespace Thor {
namespace Resource {

// forward declarations
template<typename T>
typename Interface<T, void>::None XMLSerialize(
        std::ostringstream &into, const T &object);

template<typename T>
typename Interface<T, void>::Vector XMLSerialize(
    std::ostringstream &into, const T &object);

template<typename T>
typename Interface<T, void>::Tuple XMLSerialize(
    std::ostringstream &into, const T &object);

template<typename T>
typename Interface<T, void>::Simple
    XMLSerialize(std::ostringstream &into, const T &object);

// vector specialization
template<typename T>
typename Interface<T, void>::Vector XMLSerialize(
    std::ostringstream &into, const T &object) {

    into << '[';
    for(typename T::size_type i = 0; i < object.size(); i ++) {
        if(i != 0) into << ',';
        XMLSerialize<typename T::value_type>(into, object[i]);
    }
    into << ']';
}


// start of tuple specialization
template<std::size_t index, typename T>
typename TuplePos<index, T, void>::Start
XMLSerializeTupleHelper(std::ostringstream& /*unused*/, const T& /*unused*/) {
    // empty, nothing more to do
}

template<std::size_t index, typename T>
typename TuplePos<index, T, void>::Rest
XMLSerializeTupleHelper(std::ostringstream &into, const T &object) {
    // if not first, add a comma.
    if(index != 0) {
        into << ",";
    }

    XMLSerialize<typename std::tuple_element<index, T>::type>(
        into, std::get<index>(object)
    );
    
    XMLSerializeTupleHelper<index+1, T>(into, object);
}

template<typename T>
void XMLSerializeTupleHelper(std::ostringstream &from, const T &object) {
    XMLSerializeTupleHelper<0, T>(from, object);
}

// tuple specialization
template<typename T>
typename Interface<T, void>::Tuple XMLSerialize(
    std::ostringstream &into, const T &object) {

    into << '(';
    XMLSerializeTupleHelper<T>(into, object);
    into << ')';
}

// XML interface specialization
template<typename T>
typename Interface<T, void>::Simple
    XMLSerialize(std::ostringstream &into, const T &object) {

    typename T::XMLInterfaceType result = object.XMLInterfaceSerialize(object);
    XMLSerialize(into, result);
}


template<typename T>
std::string XMLSerialize(const T &object) {
    std::ostringstream ss;
    XMLSerialize<T>(ss, object);
    return ss.str();
}

}  // namespace Resource
}  // namespace Thor
