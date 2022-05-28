# Add sub directories.
add_subdirectory(Effect)
add_subdirectory(Id)
add_subdirectory(Math)
add_subdirectory(Model)
add_subdirectory(Motion)
add_subdirectory(Physics)
# add_subdirectory(Rendering)
add_subdirectory(Type)
add_subdirectory(Utils)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} _SUB_SOURCES)
set(_SUB_SOURCES ${_SUB_SOURCES} PARENT_SCOPE)