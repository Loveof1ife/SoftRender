# Minimum CMake version requirement
cmake_minimum_required(VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Project name and language specification
project(
    PathTracing
	LANGUAGES CXX
)

# Automatically add all source files from the src directory
aux_source_directory(src SRC)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Specify the build type if not defined
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Add executable target based on source files
add_executable(PathTracing ${SRC})

# Include directories
target_include_directories(PathTracing PUBLIC include)

# Specify the executable output directory on UNIX-like systems
if(UNIX)
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
endif()

# If using CMake 3.12 or later, you can set the C++ standard property
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PathTracing PROPERTY CXX_STANDARD 17)
endif()

