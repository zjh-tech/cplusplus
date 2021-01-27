include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB SRC "*.cpp" "*.hpp" "*.h" "*.c" "*.cc")

# set lib_name
set (lib_name ${PPB_LIB})

# set lib_type  STATIC or SHARED
set (lib_type STATIC)

message(STATUS "${lib_name} ${lib_type}  ${CMAKE_BUILD_TYPE}")

add_library(${lib_name} ${lib_type} ${SRC} ${STATIC_DATA_SRC})
target_link_libraries(${lib_name}  ${PROTOBUF_LIBRARIES} ${THREAD_LIB})