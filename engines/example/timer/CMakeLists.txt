file(GLOB SRC "*.cpp" "*.hpp" "*.h")

add_executable (timer ${SRC})
target_link_libraries (timer ${THREAD_LIB} ${FILE_SYSTEM_LIB} ${ENGINE_LOG_LIB} ${ENGINE_TIMER_LIB} )

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/example)

