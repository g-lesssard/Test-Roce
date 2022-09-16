set(LIB_NAME "buffer_manager")
set(LIB_ALIAS "${PROJECT_NAME}::BufferManager")

set(LIB_SOURCES
    src/BRAMBuffer.cpp

)

set(LIB_HEADERS
    include/buffer_manager/IBuffer.h
    include/buffer_manager/StaticBuffer.h
    include/buffer_manager/StaticBuffer.tpp
    include/buffer_manager/BRAMBuffer.h
    include/buffer_manager/ICircularBuffer.h
    include/buffer_manager/CircularBuffer.h
    include/buffer_manager/CircularBuffer.tpp
)

add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
add_library(${LIB_ALIAS} ALIAS ${LIB_NAME})
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${LIB_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>

    #PRIVATE
)

target_link_libraries(${LIB_NAME}
    PUBLIC
    ${${PROJECT_NAME}_ibverbs}
)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
