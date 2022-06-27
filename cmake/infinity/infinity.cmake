include(FetchContent)

# Adding modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

function(lib_infinity_fetch_populate path)
    if(LIB-INFINITY_PATH)
        message(INFO "INFINITY_PATH specified, skipping fetch")
    endif()

    FetchContent_Declare(
            lib-infinity
            GIT_REPOSITORY https://github.com/claudebarthels/infinity
            GIT_TAG        v1.1
            GIT_PROGRESS   TRUE
    )

    FetchContent_GetProperties(${LIB-INFINITY} POPULATED INFINITY_POPULATED)
    if(NOT LIB-INFINITY_POPULATED)
        message("Cloning lib-infinity library")
        set(FETCHCONTENT_QUIET FALSE) # To see progress

        FetchContent_Populate(lib-infinity)
    endif()
        set(${path} ${lib-infinity_SOURCE_DIR} PARENT_SCOPE)
endfunction()

lib_infinity_fetch_populate(LIB-INFINITY_DIR)
message("lib-infinity sources are in ${LIB-INFINITY_DIR}")

set(LIB_SOURCES
    ${LIB-INFINITY_DIR}/src/infinity/core/Context.cpp
    ${LIB-INFINITY_DIR}/src/infinity/memory/Atomic.cpp
    ${LIB-INFINITY_DIR}/src/infinity/memory/Buffer.cpp
    ${LIB-INFINITY_DIR}/src/infinity/memory/Region.cpp
    ${LIB-INFINITY_DIR}/src/infinity/memory/RegionToken.cpp
    ${LIB-INFINITY_DIR}/src/infinity/memory/RegisteredMemory.cpp
    ${LIB-INFINITY_DIR}/src/infinity/queues/QueuePair.cpp
    ${LIB-INFINITY_DIR}/src/infinity/queues/QueuePairFactory.cpp
    ${LIB-INFINITY_DIR}/src/infinity/requests/RequestToken.cpp
    ${LIB-INFINITY_DIR}/src/infinity/utils/Address.cpp
)
set(LIB_HEADERS
    ${LIB-INFINITY_DIR}/src/infinity/infinity.h 
    ${LIB-INFINITY_DIR}/src/infinity/core/Context.h 
    ${LIB-INFINITY_DIR}/src/infinity/core/Configuration.h 
    ${LIB-INFINITY_DIR}/src/infinity/memory/Atomic.h 
    ${LIB-INFINITY_DIR}/src/infinity/memory/Buffer.h 
    ${LIB-INFINITY_DIR}/src/infinity/memory/Region.h 
    ${LIB-INFINITY_DIR}/src/infinity/memory/RegionToken.h 
    ${LIB-INFINITY_DIR}/src/infinity/memory/RegionType.h 
    ${LIB-INFINITY_DIR}/src/infinity/memory/RegisteredMemory.h 
    ${LIB-INFINITY_DIR}/src/infinity/queues/QueuePair.h 
    ${LIB-INFINITY_DIR}/src/infinity/queues/QueuePairFactory.h 
    ${LIB-INFINITY_DIR}/src/infinity/requests/RequestToken.h 
    ${LIB-INFINITY_DIR}/src/infinity/utils/Debug.h 
    ${LIB-INFINITY_DIR}/src/infinity/utils/Address.h        
)

add_library(lib-infinity STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_include_directories(lib-infinity
        PUBLIC
        $<BUILD_INTERFACE:${LIB-INFINITY_DIR}/src>
        $<INSTALL_INTERFACE:${LIB-INFINITY_DIR}/src>
)
target_link_libraries(lib-infinity
        PUBLIC
        ibverbs
)