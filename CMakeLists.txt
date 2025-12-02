cmake_minimum_required(VERSION 3.16)

# # Initial configuration
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
   set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
endif()

project(Retargeting) #project name
set(TargetName Retargeting) # executable name 

# Shared C++ version settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Optionally, set global warning flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# Add subprojects
add_subdirectory(pkg/retargeting)
add_subdirectory(cmd/retargeting)

# If you need to add additional shared deps, you can include them as:
#  add_subdirectory(pkg/my-shared-pkg)
# Keep in mind that anything in pkg will be dynamic or statically linked to anything in cmd
