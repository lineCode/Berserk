#######################################################################################
#                                                                                     #
# BERSERK Engine (2019)                                                               #
# CMake build file                                                                    #
#                                                                                     #
# Created by Orachyov Egor on 16.03.2019                                              #
#                                                                                     #
#######################################################################################

cmake_minimum_required (VERSION 3.9)

# Engine Version Specification

set (BERSERK_VERSION \"0.3\")
set (BERSERK_VERSION_MAJOR 0)
set (BERSERK_VERSION_MINOR 3)

#######################################################################################
#                                                                                     #
# Configure compilation options for chosen third-party dependencies and additional    #
# macro extra data (version, etc.)                                                    #
#                                                                                     #
#######################################################################################

# Rendering Engine Options

option (USE_METAL "Use for rendering MacOS GPU Driver" OFF)
option (USE_OPEN_GL "Use for rendering OpenGL driver" ON)
option (USE_DIRECT_3D "Use for rendering MS DirectX 3D driver" OFF)

# Audio Engine Options

option (USE_OPEN_AL "Use for audio Open AL driver" OFF)
option (USE_DIRECT_AUDIO "Use for rendering MS DirectX Audio driver" OFF)

# Image Importer Options

option (USE_FREE_IMAGE "Use for FreeImage library for importing and exporting images" ON)

# Configure files

configure_file (
    "${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem/Public/Info/Version.h.in"
    "${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem/Public/Info/Version.h"
)

configure_file (
    "${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem/Public/Info/VideoDriver.h.in"
    "${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem/Public/Info/VideoDriver.h"
)

configure_file (
    "${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem/Public/Info/AudioDriver.h.in"
    "${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem/Public/Info/AudioDriver.h"
)

configure_file (
    "${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem/Public/Info/ImageImporter.h.in"
    "${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem/Public/Info/ImageImporter.h"
)

#######################################################################################
#                                                                                     #
# Configure common engine modules with default compilation params                     #
#                                                                                     #
#######################################################################################

# Extra compilation flags to support hardware extensions and compilation modes

set (BERSERK_LINK_COMMON)
set (BERSERK_LINK_THIRD_PARTY)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -mmmx -msse -msse2 -msse3 -msse4")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -v -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")

include_directories ("${PROJECT_SOURCE_DIR}/Engine/ThirdParty")
include_directories ("${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem/Public")
include_directories ("${PROJECT_SOURCE_DIR}/Engine/Source/Application/Public")
include_directories ("${PROJECT_SOURCE_DIR}/Engine/Source/EngineSystem/Public")
include_directories ("${PROJECT_SOURCE_DIR}/Engine/Source/RenderCore/Public")
include_directories ("${PROJECT_SOURCE_DIR}/Engine/Source/RenderSystem/Public")
include_directories ("${PROJECT_SOURCE_DIR}/Engine/Source/ProfilingSystem/Public")
include_directories ("${PROJECT_SOURCE_DIR}/Engine/Source/XMLParser/Public")
include_directories ("${PROJECT_SOURCE_DIR}/Engine/Source/OpenGLDriver/Public")
include_directories ("${PROJECT_SOURCE_DIR}/Engine/Source/FreeImageImporter/Public")

add_subdirectory ("${PROJECT_SOURCE_DIR}/Engine/Source/CoreSystem")
add_subdirectory ("${PROJECT_SOURCE_DIR}/Engine/Source/Application")
add_subdirectory ("${PROJECT_SOURCE_DIR}/Engine/Source/EngineSystem")
add_subdirectory ("${PROJECT_SOURCE_DIR}/Engine/Source/RenderCore")
add_subdirectory ("${PROJECT_SOURCE_DIR}/Engine/Source/RenderSystem")
add_subdirectory ("${PROJECT_SOURCE_DIR}/Engine/Source/XMLParser")
add_subdirectory ("${PROJECT_SOURCE_DIR}/Engine/Source/ProfilingSystem")

set (BERSERK_LINK_COMMON ${BERSERK_LINK_COMMON} BerserkCoreSystem)
set (BERSERK_LINK_COMMON ${BERSERK_LINK_COMMON} BerserkApplication)
set (BERSERK_LINK_COMMON ${BERSERK_LINK_COMMON} BerserkEngineSystem)
set (BERSERK_LINK_COMMON ${BERSERK_LINK_COMMON} BerserkRenderCore)
set (BERSERK_LINK_COMMON ${BERSERK_LINK_COMMON} BerserkRenderSystem)
set (BERSERK_LINK_COMMON ${BERSERK_LINK_COMMON} BerserkXMLParser)
set (BERSERK_LINK_COMMON ${BERSERK_LINK_COMMON} BerserkProfilingSystem)

#######################################################################################
#                                                                                     #
# Configure OpenGL platform module with its include dirs and library                  #
# (required libs: OpenGL, GLFW, GLEW, see also for platform)                          #                                                                                     #
#                                                                                     #
#######################################################################################

if (USE_OPEN_GL)

    include             ("${PROJECT_SOURCE_DIR}/Engine/Source/OpenGLDriver/CMakeInclude.txt")
    add_subdirectory    ("${PROJECT_SOURCE_DIR}/Engine/Source/OpenGLDriver")
    set                 (BERSERK_LINK_THIRD_PARTY ${BERSERK_LINK_THIRD_PARTY} BerserkOpenGLDriver)
    message             ("-- Use for rendering OpenGL driver")

endif (USE_OPEN_GL)

#######################################################################################
#                                                                                     #
# Configure FreeImage importer as component for importing/exporting image data        #
# into the engine (see lib requirements and third-party dependencies)                 #
#                                                                                     #
#######################################################################################

if (USE_FREE_IMAGE)

    include             ("${PROJECT_SOURCE_DIR}/Engine/Source/FreeImageImporter/CMakeInclude.txt")
    add_subdirectory    ("${PROJECT_SOURCE_DIR}/Engine/Source/FreeImageImporter")
    set                 (BERSERK_LINK_THIRD_PARTY ${BERSERK_LINK_THIRD_PARTY} BerserkFreeImageImporter)
    message             ("-- Use for FreeImage library for importing and exporting images")

endif (USE_FREE_IMAGE)

#######################################################################################
#                                                                                     #
# Successful engine linking                                                           #
#                                                                                     #
#######################################################################################

message ("-- Use for BERSERK Engine (CMakeInclude.txt)")
message ("-- Use 'BERSERK_LINK_COMMON' to link default engine components")
message ("-- Use 'BERSERK_LINK_THIRD_PARTY' to link third-party platform-based engine components")