set(LIB_NAME "buffer_manager")
set(LIB_ALIAS "BufferManager")

set(LIB_SOURCES
)

set(LIB_HEADERS
    include/buffer_manager/IBuffer.h
    include/buffer_manager/StaticBuffer.h
    include/buffer_manager/StaticBuffer.tpp
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

#target_link_libraries(${LIB_NAME}
#    PUBLIC
#)