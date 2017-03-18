############################
# Generate Lockwood Source #
############################

cmake_minimum_required(VERSION 2.8)
PROJECT(Lockwood)
set(CMAAKE_CXX_STANDARD 14)

#Third Party Dependencies
set(THIRDP "ThirdParty")
#---------------------------------------
set(GLFW_ROOT "${THIRDP}/glfw")                                     #GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_ROOT})
include_directories("$(GLFW_ROOT}/include")

include_directories("${THIRDP}/glm/")                               #GLM
include_directories("${THIRDP}/stb/")                               #STB
include_directories("${THIRDP}/tinyobjloader/")                     #TINY OBJ LOADER
include_directories("${THIRDP}/imgui/")                             #IMGUI
set(IMGUI "${THIRDP}/imgui/imgui.cpp" "${THIRDP}/imgui/imgui_draw.cpp" "${THIRDP}/imgui/imgui_demo.cpp")

list(APPEND CMAKE_MODULE_PATH "${THIRDP}/glfw/CMake/modules/")  
find_package(Vulkan REQUIRED)                                       #VULKAN
#----------------------------------------

#Source Files. Append New Files Here.
set(SRC "Lockwood")
#----------------------------------------
set(SOURCES 
    "${SRC}/Source/main.cpp"
    "${SRC}/Source/LApplication.cpp"
    "${SRC}/Source/LWindow.cpp"
    "${SRC}/Source/LUtility.cpp"
    "${SRC}/Source/LVGui.cpp"
    "${SRC}/Source/LVContext.cpp"
    "${SRC}/Source/LVForwardPlusRenderCore.cpp"
)

set(HEADERS
    "${SRC}/LApplication.h"
    "${SRC}/LWindow.h"
    "${SRC}/LUtility.h"
    "${SRC}/LVGui.h"
    "${SRC}/LVContext.h"
    "${SRC}/LVWrapper.h"
    "${SRC}/LVForwardPlusRenderCore.h"
)
#----------------------------------------

source_group(Source FILES ${SOURCES})
source_group(Header FILES ${HEADERS})
source_group(ThirdP FILES ${IMGUI})
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${IMGUI})

target_link_libraries(${CMAKE_PROJECT_NAME} glfw) 
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${Vulkan_LIBRARIES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")