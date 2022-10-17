add_subdirectory(Activation)
## include as "Extension/..."
target_include_directories(Activation INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(snake_on_plane)
add_subdirectory(snake_pit)
