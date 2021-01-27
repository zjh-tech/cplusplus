file(GLOB SRC "*.cpp" "*.hpp" "*.h")

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)

add_executable (tsgatewayserver ${SRC} ${FRAME_SRC} ${PB_SRC})
target_link_libraries (tsgatewayserver   ${ENGINE_LOG_LIB} ${ENGINE_COMMON_LIB}  ${ENGINE_TCP_LIB}  ${XML_LIB} ${ENGINE_TIMER_LIB}  ${PROTOBUF_LIBRARIES} ${THREAD_LIB} ${FILE_SYSTEM_LIB})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/tsrank/tsgatewayserver)