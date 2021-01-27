file(GLOB SRC "*.cpp" "*.hpp" "*.h")

add_executable (meta ${SRC})

target_link_libraries (meta  ${ENGINE_LOG_LIB} ${THREAD_LIB} ${FILE_SYSTEM_LIB})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)
