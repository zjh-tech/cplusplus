file(GLOB SRC "*.cpp" "*.hpp" "*.h")

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
#add_executable (hallserver ${SRC} ${FRAME_SRC} ${PB_SRC})

add_executable (hallserver ${SRC} ${FRAME_SRC} ${PB_SRC})
target_link_libraries (hallserver ${ENGINE_COMMON_LIB} ${ENGINE_LOG_LIB} ${ENGINE_TCP_LIB}  ${ENGINE_TIMER_LIB}  ${PFRAME_LIB} ${PPB_LIB} ${PROTOBUF_LIBRARIES} ${XML_LIB} ${THREAD_LIB} ${FILE_SYSTEM_LIB})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/hallserver/)