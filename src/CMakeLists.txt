add_definitions(-Wall -Wextra -std=c++11)

include_directories(. ${pugixmlInclude})

aux_source_directory(. dotSources)
aux_source_directory(math mathSources)
aux_source_directory(tracer tracerSources)
aux_source_directory(resource resourceSources)

set(sources
    ${dotSources}
    ${mathSources}
    ${tracerSources}
    ${resourceSources}
)

add_executable(thor ${sources})

target_link_libraries(thor m OpenCL pugixml SDL2)
