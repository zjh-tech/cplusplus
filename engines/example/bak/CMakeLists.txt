add_executable (log_example log_example.cpp)
target_link_libraries (log_example  ${THREAD_LIB}  ${ENGINE_LOG_LIB})

#add_executable (asan_example asan_example.cpp)
#target_link_libraries (asan_example   ${THREAD_LIB})

#add_executable (gperftools_example gperftools_example.cpp)
#target_link_libraries (gperftools_example ${THREAD_LIB} ${GPERFTOOLS_LIB})

#add_executable (client_example client_example.cpp)
#target_link_libraries (client_example   ${THREAD_LIB} ${ENGINE_TCP_LIB} ${GPERFTOOLS_LIB} )

#add_executable (server_example server_example.cpp)
#target_link_libraries (server_example   ${THREAD_LIB} ${ENGINE_TCP_LIB} ${GPERFTOOLS_LIB} )

#add_executable (udp_client_example udp_client_example.cpp)
#target_link_libraries (udp_client_example   ${THREAD_LIB}  ${ENGINE_UDP_LIB})

#add_executable (udp_server_example udp_server_example.cpp)
#target_link_libraries (udp_server_example  ${THREAD_LIB} ${ENGINE_UDP_LIB})

#add_executable (mysql_example mysql_example.cpp)
#target_link_libraries (mysql_example  ${THREAD_LIB} ${ENGINE_MYSQL_LIB} )

#add_executable (timer_example timer_example.cpp)
#target_link_libraries (timer_example  ${THREAD_LIB} ${ENGINE_LOG_LIB} ${ENGINE_TIMER_LIB} ${ENGINE_OBJ_POOL_LIB})

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../engine/src/config/ CONFIG_SRC)
#add_executable (config_example ${CONFIG_SRC} config_example.cpp)
#target_link_libraries (config_example  ${THREAD_LIB} )

#add_executable (convert_example convert_example.cpp)
#target_link_libraries (convert_example  ${THREAD_LIB})

#add_executable (obj_pool_example  obj_pool_example.cpp)
#target_link_libraries (obj_pool_example  ${THREAD_LIB} ${ENGINE_LOG_LIB} ${ENGINE_OBJ_POOL_LIB})

#add_executable (csharp_example  csharp_example.cpp)
#target_link_libraries (csharp_example  ${THREAD_LIB} ${FILE_SYSTEM_LIB} ${ENGINE_CSHARP_LIB})

#add_executable (protobuf_example  protobuf_example.cpp)
#target_link_libraries (protobuf_example  ${THREAD_LIB})

if(UNIX)    
    set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/linux/example)    
else()
    set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/win/example)    
endif()
