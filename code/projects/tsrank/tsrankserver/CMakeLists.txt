file(GLOB SRC "*.cpp" "*.hpp" "*.h")

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)

add_executable (tsrankserver ${SRC} ${COMMON_SRC})
target_link_libraries (tsrankserver  ${THREAD_LIB} ${ENGINE_LOG_LIB} ${FILE_SYSTEM_LIB})

