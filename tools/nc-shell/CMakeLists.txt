set(NC_SHELL_SOURCES nc-shell.cpp)
add_executable(nc-shell ${NC_SHELL_SOURCES})

target_link_libraries(nc-shell bustub argparse utf8proc)
set_target_properties(nc-shell PROPERTIES OUTPUT_NAME bustub-nc-shell)
