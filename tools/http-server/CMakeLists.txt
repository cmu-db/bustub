set(HTTP_SERVER_SOURCES http-server.cpp)
add_executable(http-server ${HTTP_SERVER_SOURCES})

target_link_libraries(http-server bustub)
set_target_properties(http-server PROPERTIES OUTPUT_NAME bustub-http-server)
