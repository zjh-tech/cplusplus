file(GLOB SRC "*.cpp" "*.hpp" "*.h")

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
#add_executable (dbserver ${SRC} ${FRAME_SRC} ${PB_SRC})

add_executable (dbserver ${SRC})
target_link_libraries (dbserver ${ENGINE_COMMON_LIB} ${ENGINE_LOG_LIB} ${ENGINE_TCP_LIB}  ${ENGINE_TIMER_LIB} ${ENGINE_MYSQL_LIB}  ${PFRAME_LIB} ${PPB_LIB} ${MYSQL_LIBRARIES} ${PROTOBUF_LIBRARIES} ${XML_LIB} ${THREAD_LIB} ${FILE_SYSTEM_LIB})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/dbserver/)