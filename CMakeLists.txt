cmake_minimum_required(VERSION 3.0)
project(osdo LANGUAGES CXX)

###############################################################################
## configuration ##############################################################
###############################################################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++14 -stdlib=libc++")
#add_compile_options("-stdlib=libc++")
#add_link_options("-stdlib=libc++")
set(OpenGL_GL_PREFERENCE "LEGACY")
add_definitions(
    -DEASTL_RTTI_ENABLED=0
    -DEA_COMPILER_NO_STANDARD_CPP_LIBRARY=1
    #-DEASTL_EABASE_DISABLED
    -DEASTL_DEBUG=0
    -DEASTL_EXCEPTIONS_ENABLED=0
    )
if(WIN32)
    add_link_options("-static-libstdc++")
endif()

###############################################################################
## file globbing ##############################################################
###############################################################################

# These instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables
# `SOURCE_FILES` and `RESOURCE_FILES`.
file(GLOB OSDO_SOURCE_FILES osdo/*.cpp osdo/*.h)
file(GLOB DE_SOURCE_FILES druidengine/*.cpp druidengine/*.h)
file(GLOB RESOURCE_FILES res/*)

###############################################################################
## dependencies ###############################################################
###############################################################################

set(OSDO_LIBRARIES )
set(OSDO_INCLUDE_DIRS osdo)
set(OSDO_SOURCES ${OSDO_SOURCE_FILES})

set(DE_LIBRARIES osdo)
set(DE_INCLUDE_DIRS )
set(DE_SOURCES ${DE_SOURCE_FILES} ${RESOURCE_FILES})

# Math
find_library(M_LIBRARIES m)
list(APPEND OSDO_LIBRARIES ${M_LIBRARIES})

# OpenGL
find_package(OpenGL REQUIRED)
list(APPEND OSDO_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
list(APPEND OSDO_LIBRARIES ${OPENGL_LIBRARIES})

# GLEW
find_package(GLEW)
IF(NOT GLEW_FOUND)
    message("GLEW library was not found, compiling from source.")
    add_subdirectory(deps/glew)
    set(GLEW_INCLUDE_DIRS deps/glew/include)
    set(GLEW_LIBRARIES libglew_shared)
ENDIF()
list(APPEND OSDO_INCLUDE_DIRS ${GLEW_INCLUDE_DIRS})
list(APPEND OSDO_LIBRARIES ${GLEW_LIBRARIES})

# CGLM
add_subdirectory(deps/cglm)
#find_package(cglm REQUIRED)
list(APPEND OSDO_INCLUDE_DIRS deps/cglm/include)
list(APPEND OSDO_LIBRARIES cglm)

# GLFW
find_package(glfw3)
if(glfw3_FOUND)
    find_library(glfw3_LIBRARY glfw)
else()
    message("GLFW3 library was not found, compiling from source.")
    add_subdirectory(deps/glfw)
    set(glfw3_INCLUDE_DIRS deps/glfw/include)
    set(glfw3_LIBRARY glfw)
endif()
list(APPEND DE_INCLUDE_DIRS ${glfw3_INCLUDE_DIRS})
list(APPEND DE_LIBRARIES ${glfw3_LIBRARY})

# EASTL
set(EASTL_ROOT_DIR deps/EASTL)
file(GLOB EASTL_SOURCES "${EASTL_ROOT_DIR}/source/*.cpp")
add_library(EASTL SHARED
    ${EASTL_ROOT_DIR}/source/string.cpp
    ${EASTL_ROOT_DIR}/source/hashtable.cpp
    )
set_target_properties(EASTL PROPERTIES CXX_STANDARD 14)
set(EASTL_INCLUDE_DIRS
    ${EASTL_ROOT_DIR}/include
    #${EASTL_ROOT_DIR}/test/packages/EAAssert/include
    ${EASTL_ROOT_DIR}/test/packages/EABase/include/Common
    #${EASTL_ROOT_DIR}/test/packages/EAMain/include
    #${EASTL_ROOT_DIR}/test/packages/EAStdC/include
    #${EASTL_ROOT_DIR}/test/packages/EATest/include
    #${EASTL_ROOT_DIR}/test/packages/EAThread/include
    )
target_include_directories(EASTL PRIVATE ${EASTL_INCLUDE_DIRS})
list(APPEND OSDO_INCLUDE_DIRS ${EASTL_INCLUDE_DIRS})
list(APPEND OSDO_LIBRARIES EASTL)

# stb
list(APPEND OSDO_INCLUDE_DIRS deps/stb)

# ImGui
set(IMGUI_ROOT_DIR deps/imgui)
set(IMGUI_INCLUDE_DIRS
    ${OPENGL_INCLUDE_DIR}
    ${glfw3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${IMGUI_ROOT_DIR}
    ${IMGUI_ROOT_DIR}/backends)
set(IMGUI_LIBRARIES ${GLEW_LIBRARIES} ${glfw3_LIBRARY})
if(WIN32)
    message("Adding imm library for windows platforms")
    list(APPEND IMGUI_LIBRARIES imm32)
endif()
add_library(imgui SHARED
    ${IMGUI_ROOT_DIR}/imgui.cpp
    ${IMGUI_ROOT_DIR}/imgui_draw.cpp
    #${IMGUI_ROOT_DIR}/imgui_demo.cpp
    ${IMGUI_ROOT_DIR}/imgui_widgets.cpp
    ${IMGUI_ROOT_DIR}/imgui_tables.cpp
    ${IMGUI_ROOT_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_ROOT_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PRIVATE ${IMGUI_INCLUDE_DIRS})
target_link_libraries(imgui PRIVATE ${IMGUI_LIBRARIES})
list(APPEND DE_LIBRARIES imgui)
list(APPEND DE_INCLUDE_DIRS ${IMGUI_INCLUDE_DIRS})

# ImGuiFileDialog
set(imfiledialog_ROOT_DIR deps/ImGuiFileDialog)
add_library(imfiledialog STATIC
    ${imfiledialog_ROOT_DIR}/ImGuiFileDialog.cpp)
target_include_directories(imfiledialog PRIVATE
    ${imfiledialog_ROOT_DIR}
    ${IMGUI_INCLUDE_DIRS})
target_link_libraries(imfiledialog PRIVATE imgui)
list(APPEND DE_LIBRARIES imfiledialog)
list(APPEND DE_INCLUDE_DIRS ${imfiledialog_ROOT_DIR})

# ImFileDialog
#list(APPEND DE_INCLUDE_DIRS
#    deps/ImFileDialog)
#list(APPEND DE_SOURCES
#    deps/ImFileDialog/ImFileDialog.cpp)

# Threads
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#list(APPEND DE_LIBRARIES Threads::Threads)

list(APPEND DE_INCLUDE_DIRS ${OSDO_INCLUDE_DIRS})
list(APPEND DE_LIBRARIES ${OSDO_LIBRARIES})

###############################################################################
## target definitions #########################################################
###############################################################################

add_library(osdo SHARED ${OSDO_SOURCES})
target_include_directories(osdo PRIVATE ${OSDO_INCLUDE_DIRS})
target_link_libraries(osdo PRIVATE ${OSDO_LIBRARIES})
target_compile_options(osdo PRIVATE -Wall -fno-exceptions)

add_executable(druidengine ${DE_SOURCES})
set_target_properties(druidengine PROPERTIES CXX_STANDARD 14)
target_include_directories(druidengine PRIVATE ${DE_INCLUDE_DIRS})
target_link_libraries(druidengine PRIVATE ${DE_LIBRARIES})
target_compile_options(druidengine PRIVATE -Wall)

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
## packaging ##################################################################
###############################################################################

# Search for compiler-provided system runtime libraries and
# add install rules for them.
include(InstallRequiredSystemLibraries)

# Search for compiler-provided runtime libraries and
# add copy them to installation directory.
#install(CODE "set(TARGET_PROJCET_FILE \"$<TARGET_FILE:${PROJECT_NAME}>\")")
#install(CODE "set(ADDITIONAL_LIB_PATH \"${ADDITIONAL_LIB_PATH}\")")
#install(CODE [[
#  file(GET_RUNTIME_DEPENDENCIES
#       RESOLVED_DEPENDENCIES_VAR _r_deps
#       UNRESOLVED_DEPENDENCIES_VAR _u_deps
##       DIRECTORIES ${ADDITIONAL_LIB_PATH}
#       EXECUTABLES ${TARGET_PROJCET_FILE})
#  file(COPY ${_r_deps}
#       DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" FOLLOW_SYMLINK_CHAIN)
#  ]])

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
