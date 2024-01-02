set(HTABLE_BENCH_SOURCES htable_bench.cpp)
add_executable(htable-bench ${HTABLE_BENCH_SOURCES})

target_link_libraries(htable-bench bustub)
set_target_properties(htable-bench PROPERTIES OUTPUT_NAME bustub-htable-bench)
