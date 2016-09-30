#ifndef THOR__MESSAGE_SYSTEM_H
#define THOR__MESSAGE_SYSTEM_H

#include <iosfwd>

#include "StreamAsString.h"

namespace Thor {

/* The different logging subsystems. These are defined in this manner so that
    we can get an enum and a string representation while only providing the
    name in one place throughout the source code.
    
    General is a special subsystem and must always be present. */
#define MS_Subsystems \
    MS_Subsystem(General), \
    MS_Subsystem(Config), \
    MS_Subsystem(Resource), \
    MS_Subsystem(Tracer)

/* Log message types. Fatal is a special type that must always be present. */
#define MS_Types \
    MS_Type(Debug), \
    MS_Type(Log), \
    MS_Type(Warning), \
    MS_Type(Error), \
    MS_Type(Fatal)

/** Message wrapper class. Provides public enums and some static
    logging functions. */
class MessageSystem {
public:
    enum MessageType {
#define MS_Type(type) type
        MS_Types
#undef MS_Type
    };

    enum Subsystem {
#define MS_Subsystem(system) system
        MS_Subsystems
#undef MS_Subsystem
    };
private:
    static const char *s_typeNames[];
    static const char *s_subsystemNames[];
private:
    static std::ofstream *s_logFile;
public:
    static void setLogFile(std::string path);
    static void closeLogFile();
public:
    static void message(Subsystem system, MessageType type, std::string msg);
};

#define Message(msg) \
    Message3(General, Log, msg)

#define Message2(type, msg) \
    Message3(General, type, msg)

#define Message3(sys, type, msg) \
    Thor::MessageSystem::message( Thor::MessageSystem::sys, \
        Thor::MessageSystem::type, Thor::StreamAsString() << msg)

}  // namespace Thor

#endif
