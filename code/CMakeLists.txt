cmake_minimum_required(VERSION 3.1.0)
project(engine)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# version

# platform
if(UNIX)
    include(engines/cmake/linux.cmake)  
    include(${PROJECT_SOURCE_DIR}/engines/thirds/mysql/cmake/findmysql.cmake)  
    #set(MYSQL_LIBRARIES "${PROJECT_SOURCE_DIR}/engines/thirds/mysql/lib/libmysqld.a")
    #set(MYSQL_LIBRARIES "${PROJECT_SOURCE_DIR}/engines/thirds/mysql/lib/libmysqlclient.so.20.3.18")
else()
    include(engines/cmake/windows.cmake)
    #include(${PROJECT_SOURCE_DIR}/engines/thirds/mysql/cmake/findmysql.cmake)
    set(MYSQL_LIBRARIES "${PROJECT_SOURCE_DIR}/engines/thirds/mysql/lib/libmysql.lib")    
endif()

# find protobuf
#find_package(Protobuf REQUIRED)
#if (PROTOBUF_FOUND)
#    message("protobuf found")
#else ()
#    message(FATAL_ERROR "Cannot find Protobuf")
#endif ()

set(PROTOBUF_LIBRARIES 	        protobuf)
set(XML_LIB 	                tinyxml2)
set(YAML_LIB                    yaml-cpp)
set(CURL_LIB                    curl)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# include
#engine lib src
include_directories("${PROJECT_SOURCE_DIR}/engines/")
include_directories("${PROJECT_SOURCE_DIR}/engines/thirds/")
include_directories("${PROJECT_SOURCE_DIR}/engines/thirds/asio/include")        # asio
include_directories("${PROJECT_SOURCE_DIR}/engines/thirds/mysql/include")       # mysql

#project lib src
include_directories("${PROJECT_SOURCE_DIR}/projects/thirds/protobuf/src")            # protobuf
include_directories("${PROJECT_SOURCE_DIR}/projects/thirds/tinyxml2")                # tinyxml2
include_directories("${PROJECT_SOURCE_DIR}/projects/thirds/yaml-cpp/include")        # yaml-cpp
include_directories("${PROJECT_SOURCE_DIR}/projects/thirds/rapidjson/include")       # rapidjson
include_directories("${PROJECT_SOURCE_DIR}/projects/thirds/curl/include")            # curl

#other src
include_directories("${PROJECT_SOURCE_DIR}/projects/staticdata")                     # staticdata

#link
if(UNIX)
     if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        link_directories("${PROJECT_SOURCE_DIR}/engines/lib/linux/Debug")
        link_directories("${PROJECT_SOURCE_DIR}/projects/lib/linux/Debug")
    else()
        link_directories("${PROJECT_SOURCE_DIR}/engines/lib/linux/Release")        
        link_directories("${PROJECT_SOURCE_DIR}/projects/lib/linux/Release")
    endif()    
else()
    link_directories("${PROJECT_SOURCE_DIR}/engines/lib/win")
    link_directories("${PROJECT_SOURCE_DIR}/projects/lib/win")
endif()

#sub
add_subdirectory(projects)
