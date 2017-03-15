#LOCKWOOD PLUS: FORWARD PLUS RENDER ENGINE

cmake_minimum_required(VERSION 2.8)
PROJECT(Lockwood)
set(CMAAKE_CXX_STANDARD 14)

set(THIRDP "ThirdParty") #Include Third Party

#GLFW FindVulkan Module.
list(APPEND CMAKE_MODULE_PATH "${THIRDP}/glfw/CMake/modules/")

include_directories("${THIRDP}/stb/")
include_directories("${THIRDP}/tinyobjloader/")
include_directories("${THIRDP}/glm/")

#GLFW Config
set(GLFW_ROOT "${THIRDP}/glfw")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_ROOT})
include_directories("$(GLFW_ROOT}/include")

#Locate Vulkan
find_package(Vulkan REQUIRED)

set(SOURCE 
    "Source/main.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE})
target_link_libraries(${CMAKE_PROJECT_NAME} glfw) 
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${Vulkan_LIBRARIES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")