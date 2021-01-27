file(GLOB SRC "*.cpp" "*.hpp" "*.h")


add_executable (meta ${SRC})
#add_executable (meta ${SRC} ${COMMON_SRC} )
target_link_libraries (meta ${THREAD_LIB} ${FILE_SYSTEM_LIB} ${ENGINE_LOG_LIB} ${ENGINE_COMMON_LIB})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/example)

