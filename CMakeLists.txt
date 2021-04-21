cmake_minimum_required(VERSION 3.15)
project(osdo LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###############################################################################
## configuration ##############################################################
###############################################################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

###############################################################################
## file globbing ##############################################################
###############################################################################

# These instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables
# `SOURCE_FILES` and `RESOURCE_FILES`.
file(GLOB SOURCE_FILES src/*.cpp src/*.h)
file(GLOB RESOURCE_FILES res/*)

###############################################################################
## target definitions #########################################################
###############################################################################

# The data is just added to the executable, because in some IDEs (QtCreator)
# files are invisible when they are not explicitly part of the project.
add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${RESOURCE_FILES}
    deps/imgui/imgui.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/backends/imgui_impl_glfw.cpp
    deps/imgui/backends/imgui_impl_opengl3.cpp
    deps/ImGui-Addons/FileBrowser/ImGuiFileBrowser.cpp)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -std=c99 -pedantic)

# This allows to include files relative to the root of the src directory with a <> pair
#target_include_directories(${PROJECT_NAME} PUBLIC src/main)

# This copies all resource files in the build directory.
add_custom_target(${PROJECT_NAME}_res ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "share/${PROJECT_NAME}/"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RESOURCE_FILES} "share/${PROJECT_NAME}/")

# Set resource files to target properties.
#set_target_properties(${PROJECT_NAME} PROPERTIES
#  RESOURCE "${RESOURCE_FILES}")

###############################################################################
## dependencies ###############################################################
###############################################################################

set(DE_LIBRARIES m)
set(DE_INCLUDE_DIRS
    deps/imgui
    deps/imgui/backends
    deps/ImGui-Addons/FileBrowser)

# Math
find_library(M_LIBRARIES m REQUIRED)
list(APPEND DE_LIBRARIES ${M_LIBRARIES})

# OpenGL
find_package(OpenGL REQUIRED)
list(APPEND DE_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
list(APPEND DE_LIBRARIES ${OPENGL_LIBRARIES})

# GLEW
find_package(GLEW REQUIRED)
list(APPEND DE_INCLUDE_DIRS ${GLEW_INCLUDE_DIRS})
list(APPEND DE_LIBRARIES ${GLEW_LIBRARIES})

# CGLM
find_package(cglm REQUIRED)
list(APPEND DE_LIBRARIES ${CGLM_LIBRARY})

# GLFW
find_package(glfw3 REQUIRED)
find_library(glfw3_LIBRARY glfw REQUIRED)
list(APPEND DE_LIBRARIES ${glfw3_LIBRARY})

# EASTL
#add_subdirectory(deps/EASTL)
#add_subdirectory(deps/EASTL/test/packages/EABase)
set(EASTL_ROOT_DIR deps/EASTL)
list(APPEND DE_INCLUDE_DIRS
    ${EASTL_ROOT_DIR}/include
    ${EASTL_ROOT_DIR}/test/packages/EAAssert/include
    ${EASTL_ROOT_DIR}/test/packages/EABase/include/Common
    ${EASTL_ROOT_DIR}/test/packages/EAMain/include
    ${EASTL_ROOT_DIR}/test/packages/EAStdC/include
    ${EASTL_ROOT_DIR}/test/packages/EATest/include
    ${EASTL_ROOT_DIR}/test/packages/EAThread/include)
#set(EASTL_LIBRARY debug ${EASTL_ROOT_DIR}/build/Debug/EASTL.lib optimized ${EASTL_ROOT_DIR}/build/Release/EASTL.lib)
#add_custom_target(NatVis SOURCES ${EASTL_ROOT_DIR}/doc/EASTL.natvis)
list(APPEND DE_LIBRARIES ${EASTL_LIBRARY})

target_include_directories(${PROJECT_NAME} PRIVATE ${DE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${DE_LIBRARIES})

###############################################################################
## packaging ##################################################################
###############################################################################

# Search for compiler-provided system runtime libraries and
# add install rules for them.
include(InstallRequiredSystemLibraries)

# Search for compiler-provided runtime libraries and
# add copy them to installation directory.
install(CODE "set(TARGET_PROJCET_FILE \"$<TARGET_FILE:${PROJECT_NAME}>\")")
install(CODE "set(ADDITIONAL_LIB_PATH \"${ADDITIONAL_LIB_PATH}\")")
install(CODE [[
  file(GET_RUNTIME_DEPENDENCIES
       RESOLVED_DEPENDENCIES_VAR _r_deps
       UNRESOLVED_DEPENDENCIES_VAR _u_deps
#       DIRECTORIES ${ADDITIONAL_LIB_PATH}
       EXECUTABLES ${TARGET_PROJCET_FILE})
  file(COPY ${_r_deps}
       DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" FOLLOW_SYMLINK_CHAIN)
  ]])

# All install commands get the same destination. this allows us to use paths
# relative to the executable.
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# This is basically a repeat of the file copy instruction that copies the
# resources in the build directory, but here we tell cmake that we want it
# in the package.
#install(DIRECTORY res DESTINATION share/${PROJECT_NAME})

# Now comes everything we need, to create a package
# there are a lot more variables you can set, and some
# you need to set for some package types, but we want to
# be minimal here.
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "1.0.0")

# We don't want to split our program up into several incomplete pieces.
set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
