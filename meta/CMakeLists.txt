cmake_minimum_required(VERSION 3.1.0)
project(meta)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# version

# platform
if(UNIX)
    include(engines/cmake/linux.cmake)    
else()
    include(engines/cmake/windows.cmake)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# include
#engine lib src
include_directories("${PROJECT_SOURCE_DIR}/engines/")
include_directories("${PROJECT_SOURCE_DIR}/engines/thirds/asio/include")        # asio

#link
if(UNIX)
     if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        link_directories("${PROJECT_SOURCE_DIR}/engines/lib/linux/Debug")        
    else()
        link_directories("${PROJECT_SOURCE_DIR}/engines/lib/linux/Release")                
    endif()    
else()
    link_directories("${PROJECT_SOURCE_DIR}/engines/lib/win")    
endif()

# sub
add_subdirectory(meta)
