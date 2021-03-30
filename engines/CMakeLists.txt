cmake_minimum_required(VERSION 3.1.0)
project(engine)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# version

# platform

include(cmake/linux.cmake)
include(${PROJECT_SOURCE_DIR}/thirds/mysql/cmake/findmysql.cmake)  
#set(MYSQL_LIBRARIES "${PROJECT_SOURCE_DIR}/thirds/mysql/lib/libmysqld.a")    
#set(MYSQL_LIBRARIES "${PROJECT_SOURCE_DIR}/thirds/mysql/lib/libmysqlclient.so.20.3.18")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# include
include_directories("${PROJECT_SOURCE_DIR}/")

include_directories("${PROJECT_SOURCE_DIR}/thirds/asio/include")               # asio
include_directories("${PROJECT_SOURCE_DIR}/thirds/mysql/include")              # mysql

# link
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/Debug)
else()
    set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/Release)
endif()

# sub
add_subdirectory(thirds)
add_subdirectory(engine/src)
add_subdirectory(example)
