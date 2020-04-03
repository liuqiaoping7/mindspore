message(STATUS "ENABLE_GE set to FALSE, compiling GraphEngine")
set(GE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/graphengine)

message(STATUS "ge dir: ${GE_SOURCE_DIR}")
# download json headers, rather than whole repository
include(${GE_SOURCE_DIR}/cmake/ge_utils.cmake)
include(${GE_SOURCE_DIR}/cmake/external_libs/json.cmake)
include(${GE_SOURCE_DIR}/cmake/external_libs/eigen.cmake)
include(${GE_SOURCE_DIR}/cmake/external_libs/gtest.cmake)
include(${GE_SOURCE_DIR}/cmake/external_libs/protobuf.cmake)
include(${GE_SOURCE_DIR}/cmake/external_libs/onnx.cmake)

# for CPU/GPU mode, find c_sec and slog from local prebuild
if (NOT ENABLE_D)
    set(GE_PREBUILD_PATH ${GE_SOURCE_DIR}/third_party/prebuild/${CMAKE_HOST_SYSTEM_PROCESSOR})
    find_library(c_sec libc_sec.so ${GE_PREBUILD_PATH})
    find_library(slog libslog.so ${GE_PREBUILD_PATH})
elseif (DEFINED ENV{D_LINK_PATH})
    set(GE_LIB_PATH $ENV{D_LINK_PATH})
    set(GE_SYS_ARCH "")
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
        # x86 ubuntu
        set(GE_SYS_ARCH "x86_64")
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
        # arm euleros
        set(GE_SYS_ARCH "aarch64")
    else()
        message(FATAL_ERROR "Running on a unsupported architecture: ${SYSTEM_TYPE}, build terminated")
    endif()
    set(GE_LIB_PATH ${GE_LIB_PATH}/${GE_SYS_ARCH})
    find_library(c_sec libc_sec.so ${GE_LIB_PATH})
    find_library(slog libslog.so ${GE_LIB_PATH})
    find_library(mmpa libmmpa.so ${GE_LIB_PATH})
    find_library(runtime libruntime.so ${GE_LIB_PATH})
    find_library(msprof libmsprof.so ${GE_LIB_PATH})
    find_library(register libregister.so ${GE_LIB_PATH})
    find_library(hccl libhccl.so ${GE_LIB_PATH})
    find_library(cce libcce.so ${GE_LIB_PATH})
    find_library(resource libresource.so ${GE_LIB_PATH})
else()
    # Ascend mode
    set(ASCEND_PATH /usr/local/Ascend)
    set(ASCEND_DRIVER_PATH ${ASCEND_PATH}/driver/lib64/common)
    set(ASCEND_RUNTIME_PATH ${ASCEND_PATH}/fwkacllib/lib64)

    find_library(c_sec libc_sec.so ${ASCEND_DRIVER_PATH})
    find_library(slog libslog.so ${ASCEND_DRIVER_PATH})
    find_library(mmpa libmmpa.so ${ASCEND_DRIVER_PATH})
    find_library(cce libcce.so ${ASCEND_RUNTIME_PATH})
    find_library(hccl libhccl.so ${ASCEND_RUNTIME_PATH})
    find_library(runtime libruntime.so ${ASCEND_RUNTIME_PATH})
    find_library(msprof libmsprof.so ${ASCEND_RUNTIME_PATH})
    find_library(register libregister.so ${ASCEND_RUNTIME_PATH})
    find_library(resource libresource.so ${ASCEND_RUNTIME_PATH})
endif()

# compile libraries from following directories
# this cmake file is called only when NOT ENABLE_GE is set
set(_ge_tmp_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

string(REPLACE " -Wall" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE " -Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# force __FILE__ to show relative path of file, from source directory
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILE__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"' -Wno-builtin-macro-redefined")
add_subdirectory(${GE_SOURCE_DIR}/src/common/graph)
if(ENABLE_D)
    add_subdirectory(${GE_SOURCE_DIR}/src/ge/common)
    add_subdirectory(${GE_SOURCE_DIR}/src/ge/ge_runtime)
endif()

set(CMAKE_CXX_FLAGS ${_ge_tmp_CMAKE_CXX_FLAGS})
