# build type
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
	#set(CMAKE_BUILD_TYPE "Release")
endif()

#asan
option(ASAN_FLAG "Enable Asan" ON)
if (ASAN_FLAG)
	#option: -fsanitize=address -fno-omit-frame-pointer -fno-stack-protector  -fsanitize=leak ==>  exit progame and generate asan file
	#/etc/profile:  export ASAN_OPTIONS=halt_on_error=0:use_sigaltstack=0:detect_leaks=1:malloc_context_size=15:log_path=/tmp/asan/asan.log:suppressions=$SUPP_FILE
	set (CMAKE_ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer -fsanitize=leak -fno-stack-protector")

	#option: -fsanitize-recover=address==> not exit progame
	#set (CMAKE_ASAN_FLAGS "-fsanitize-recover=address")
else()	
    set (CMAKE_ASAN_FLAGS "")
endif()
message(STATUS "ASAN_FLAG: ${CMAKE_ASAN_FLAGS}")

#gperftools need Debug 
option(GPERFTOOLS_FLAG "Enable Gperftools" OFF)
if (GPERFTOOLS_FLAG)	
	add_definitions(-DGPERFTOOLS_CPU_MACRO)
	add_definitions(-DGPERFTOOLS_MEMORY_MACRO)
endif()
message(STATUS "GPERFTOOLS_FLAG: ${GPERFTOOLS_FLAG}")


# cxx flag
set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -fPIC ${CMAKE_ASAN_FLAGS} -Wno-invalid-offsetof")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -O0 ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2 ")

# def
add_definitions(-DASIO_HAS_EPOLL)

# lib

set(THREAD_LIB pthread)
set(FILE_SYSTEM_LIB stdc++fs)
if (GPERFTOOLS_FLAG)
	set(GPERFTOOLS_LIB profiler unwind tcmalloc)
endif()
set(DYNAMIC_LOAD_LIB  dl)


set(ENGINE_COMMON_LIB 			ecommon)
set(ENGINE_LOG_LIB 				elogger)
set(ENGINE_TCP_LIB 				etcp)
set(ENGINE_UDP_LIB      		eudp)
set(ENGINE_MYSQL_LIB 			emysql)
set(ENGINE_TIMER_LIB 			etimer)
set(ENGINE_REDIS_LIB 	    	eredis)
set(ENGINE_CSHARP_LIB 	    	ecsharp)

