file(GLOB SRC "*.cpp" "*.hpp" "*.h")

add_executable (mysql ${SRC})
target_link_libraries (mysql ${THREAD_LIB} ${FILE_SYSTEM_LIB} ${ENGINE_COMMON_LIB} ${ENGINE_LOG_LIB} ${ENGINE_MYSQL_LIB} ${MYSQL_LIBRARIES})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/example)

