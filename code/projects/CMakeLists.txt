# include
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/staticdata STATIC_DATA_SRC)

set(PFRAME_LIB pframe)
set(PPB_LIB ppb)

# sub
add_subdirectory(pb)
add_subdirectory(frame)
add_subdirectory(registryserver)
add_subdirectory(gatewayserver)
add_subdirectory(centerserver)
add_subdirectory(hallserver)
add_subdirectory(matchserver)
add_subdirectory(battleserver)
add_subdirectory(dbserver)
