# cxx flag
if (MSVC_VERSION GREATER_EQUAL "1900")
    add_compile_options("/std:c++latest")
else()
    message(FATAL_ERROR "Current msvc not support")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# mt
set(CompilerFlags
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_MINSIZEREL
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELWITHDEBINFO
    CMAKE_C_FLAGS_RELEASE
    CMAKE_C_FLAGS_MINSIZEREL
)
foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

# complie options
add_compile_options(/MP)
add_compile_options(-bigobj)

# def


set(THREAD_LIB)
set(FILE_SYSTEM_LIB)
set(GPERFTOOLS_LIB)
set(DYNAMIC_LOAD_LIB )

set(ENGINE_COMMON_LIB 		    ecommon)
set(ENGINE_LOG_LIB              elogger)
set(ENGINE_TCP_LIB              etcp)
set(ENGINE_UDP_LIB              eudp)
set(ENGINE_MYSQL_LIB            emysql)
set(ENGINE_TIMER_LIB 		    etimer)
set(ENGINE_REDIS_LIB 	        eredis)
set(ENGINE_CSHARP_LIB 	        ecsharp)


# warning
add_definitions(
    /wd4018 # 'expression' : signed/unsigned mismatch
    /wd4065 # switch statement contains 'default' but no 'case' labels
    /wd4146 # unary minus operator applied to unsigned type, result still unsigned
    /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
    /wd4251 # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
    /wd4305 # 'identifier' : truncation from 'type1' to 'type2'
    /wd4307 # 'operator' : integral constant overflow
    /wd4309 # 'conversion' : truncation of constant value
    /wd4334 # 'operator' : result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
    /wd4355 # 'this' : used in base member initializer list
    /wd4506 # no definition for inline function 'function'
    /wd4800 # 'type' : forcing value to bool 'true' or 'false' (performance warning)
    /wd4996 # The compiler encountered a deprecated declaration.
    /wd4819 # Unicode
)