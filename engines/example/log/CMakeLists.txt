file(GLOB SRC "*.cpp" "*.hpp" "*.h")

add_executable (log ${SRC})
target_link_libraries (log ${THREAD_LIB} ${FILE_SYSTEM_LIB} ${ENGINE_LOG_LIB})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/example)

