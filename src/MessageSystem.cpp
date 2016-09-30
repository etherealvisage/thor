#include <cstdlib>
#include <cstring>
#include <ctime>

#include <fstream>

#include "MessageSystem.h"
//#include "config/Tree.h"

namespace Thor {

std::ofstream *MessageSystem::s_logFile;

const char *MessageSystem::s_typeNames[] = {
#define MS_Type(type) #type
    MS_Types
#undef MS_Type
};

const char *MessageSystem::s_subsystemNames[] = {
#define MS_Subsystem(system) #system
    MS_Subsystems
#undef MS_Subsystem
};

void MessageSystem::setLogFile(std::string path) {
    std::string::size_type pos;
    if((pos = path.find("%d")) != std::string::npos) {
        int low = 0;
        int high = 10000;

        char buf[1000];
        std::string s;
        while(low != high) {
            int middle = (low+high)/2;

            std::sprintf(buf, "%05d", middle);
            s = path;
            s.replace(pos, 2, buf);

            std::ifstream input(s.c_str());
            if(input.is_open()) {
                if(low == middle) low = middle+1;
                else low = middle;
            }
            else {
                high = middle;
            }
        }
        std::sprintf(buf, "%05d", low);
        path.replace(pos, 2, buf);
    }

    s_logFile = new std::ofstream(path.c_str());
}

void MessageSystem::closeLogFile() {
    if(s_logFile) {
        delete s_logFile;
        s_logFile = NULL;
    }
}

void MessageSystem::message(Subsystem system, MessageType type,
    std::string msg) {

    std::time_t t;
    std::time(&t);

    struct tm *lt = std::localtime(&t);

    char buffer[4096];
    /*if(Config::Tree::instance() &&
        Config::Tree::instance()->node("runtime.extended_log")->get<bool>(
            true)) {*/
    if(true) {

        std::snprintf(buffer, sizeof(buffer),
            "[%02i:%02i:%02i %-12s %-7s]         %s\n",
            lt->tm_hour, lt->tm_min, lt->tm_sec,
            s_subsystemNames[system], s_typeNames[type],
            msg.c_str());
    }
    else {
        std::snprintf(buffer, sizeof(buffer), "%s\n", msg.c_str());
    }

    fprintf(stderr, "%s", buffer);
    if(s_logFile != NULL) {
        (*s_logFile) << buffer;
    }
    
    if(type == Fatal) {
        closeLogFile();
#ifdef DEBUG
        std::abort();
#else
        std::exit(1);
#endif
    }
}

}  // namespace Thor
