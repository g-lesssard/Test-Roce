message("Override flag value is: ${OVERRIDE_RDMA_CORE}")
if(EXISTS ${OVERRIDE_RDMA_CORE})
    set(rdma_core_DIR /root/git/rdma-core)
    message("rdma-core sources should be in ${OVERRIDE_RDMA_CORE}")
    find_library(${PROJECT_NAME}_ibverbs NAMES libibverbs.so PATHS ${rdma_core_DIR}/build/lib
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH
            NO_CMAKE_FIND_ROOT_PATH
            NO_CMAKE_PATH
            NO_CMAKE_ENVIRONMENT_PATH
    )


    if(${PROJECT_NAME}_ibverbs)
        message("Found libibverbs lib: ${${PROJECT_NAME}_ibverbs}")
    else()
        message("libibverbs lib not found")
    endif()


    find_library(${PROJECT_NAME}_rdmacm NAMES librdmacm.so PATHS ${rdma_core_DIR}/build/lib
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH
            NO_CMAKE_FIND_ROOT_PATH
            NO_CMAKE_PATH
            NO_CMAKE_ENVIRONMENT_PATH
            )
    if(${PROJECT_NAME}_rdmacm)
        message("Found librdmacm-git lib: ${${PROJECT_NAME}_rdmacm}")
    else()
        message("librdmacm-git lib not found")
    endif()
else()
    message("Using system libraries for rdmacm and ibverbs")
    set(${PROJECT_NAME}_ibverbs ibverbs)
    set(${PROJECT_NAME}_rdmacm rdmacm)
endif()