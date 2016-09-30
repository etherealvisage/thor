#define XML_AUTO_PARSE2(type) \
    static type XMLInterfaceParse( \
        const XMLInterfaceType &data) { \
        \
        return type(std::get<0>(data), std::get<1>(data)); \
    }

#define XML_AUTO_PARSE3(type) \
    static type XMLInterfaceParse( \
        const XMLInterfaceType &data) { \
        \
        return type(std::get<0>(data), std::get<1>(data), std::get<2>(data)); \
    }

#define XML_PARSE(type) \
    static type XMLInterfaceParse(const XMLInterfaceType &data)
    
#define XML_AUTO_SERIALIZE3(type, name1, name2, name3) \
    static XMLInterfaceType XMLInterfaceSerialize( \
        const type &instance) { \
        \
        return std::make_tuple(instance.name1, instance.name2, instance.name3); \
    }

#define XML_SERIALIZE(type) \
    static XMLInterfaceType XMLInterfaceSerialize(const type &instance)

#define XML_INTERFACE_HEAD \
    public: 

#define XML_INTERFACE2(name1, name2) \
    XML_INTERFACE_HEAD \
    typedef std::tuple<decltype(name1), decltype(name2)> \
        XMLInterfaceType;

#define XML_INTERFACE3(name1, name2, name3) \
    XML_INTERFACE_HEAD \
    typedef std::tuple<decltype(name1), decltype(name2), decltype(name3)> \
        XMLInterfaceType;

#define XML_INTERFACE_TYPE2(type1, type2) \
    XML_INTERFACE_HEAD \
    typedef std::tuple<type1, type2> XMLInterfaceType;

#define XML_INTERFACE_TYPE3(type1, type2, type3) \
    XML_INTERFACE_HEAD \
    typedef std::tuple<type1, type2, type3> XMLInterfaceType;
