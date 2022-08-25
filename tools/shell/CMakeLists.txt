set(SHELL_SOURCES shell.cpp)
add_executable(shell ${SHELL_SOURCES})

target_link_libraries(shell bustub)
set_target_properties(shell PROPERTIES OUTPUT_NAME bustub-shell)
