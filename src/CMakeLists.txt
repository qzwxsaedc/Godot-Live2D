add_subdirectory(cubism)
add_subdirectory(wrapper)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} _SUB_SOURCES)
set(_SUB_SOURCES ${_SUB_SOURCES} PARENT_SCOPE)