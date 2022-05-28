cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME libgdlive2d)
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(GODOT_PATH ${THIRD_PARTY_PATH}/godot-cpp)
set(CUBISM_PATH ${THIRD_PARTY_PATH}/cubism)
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(GODOT_LIB_PATH ${GODOT_PATH}/bin/libgodot-cpp.windows.debug.64.lib)
    set(CUBISM_LIB_PATH ${CUBISM_PATH}/bin/Live2DCubismCore_MTd.64.lib)
elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    set(GODOT_LIB_PATH ${GODOT_PATH}/bin/libgodot-cpp.windows.release.64.lib)
    set(CUBISM_LIB_PATH ${CUBISM_PATH}/bin/Live2DCubismCore_MT.64.lib)
else()
    set(GODOT_LIB_PATH ${GODOT_PATH}/bin/libgodot-cpp.windows.release.64.lib)
    set(CUBISM_LIB_PATH ${CUBISM_PATH}/bin/Live2DCubismCore_MT.64.lib)
endif()

set(ARCH x86_64)
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})

add_subdirectory(src)

#message(STATUS _SUB_SOURCES=${_SUB_SOURCES})

add_library(${PROJECT_NAME} SHARED ${_SUB_SOURCES})

# GODOT headers
target_include_directories(${PROJECT_NAME} PRIVATE ${GODOT_PATH}/godot-headers)
#target_include_directories(${PROJECT_NAME} PRIVATE ${GODOT_PATH}/godot-headers/gdnative)
target_include_directories(${PROJECT_NAME} PRIVATE ${GODOT_PATH}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${GODOT_PATH}/include/core)
target_include_directories(${PROJECT_NAME} PRIVATE ${GODOT_PATH}/include/gen)

# cubism sdk headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CUBISM_PATH}/include ${CMAKE_CURRENT_SOURCE_DIR}/src/cubism)

target_link_libraries(${PROJECT_NAME}
    ${GODOT_LIB_PATH}
    ${CUBISM_LIB_PATH}
    debug -NODEFAULTLIB:libcmtd.lib
    optimized -NODEFAULTLIB:libcmt.lib
)
