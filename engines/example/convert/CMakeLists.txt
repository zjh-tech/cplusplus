file(GLOB SRC "*.cpp" "*.hpp" "*.h")

add_executable (convert ${SRC})
target_link_libraries (convert ${THREAD_LIB} ${FILE_SYSTEM_LIB} ${ENGINE_COMMON_LIB})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/example)

