include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB SRC "*.cpp" "*.hpp" "*.h" "*.c")

# set lib_name
set (lib_name ${PFRAME_LIB})

# set lib_type  STATIC or SHARED
set (lib_type STATIC)

message(STATUS "${lib_name} ${lib_type}  ${CMAKE_BUILD_TYPE}")

add_library(${lib_name} ${lib_type} ${SRC})
target_link_libraries(${lib_name} ${ENGINE_COMMON_LIB} ${ENGINE_LOG_LIB} ${ENGINE_TCP_LIB} ${CURL_LIB} ${XML_LIB} ${ENGINE_TIMER_LIB}  ${PROTOBUF_LIBRARIES} ${THREAD_LIB} ${FILE_SYSTEM_LIB})