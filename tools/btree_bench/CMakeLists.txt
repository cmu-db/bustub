set(BTREE_BENCH_SOURCES btree_bench.cpp)
add_executable(btree-bench ${BTREE_BENCH_SOURCES})

target_link_libraries(btree-bench bustub)
set_target_properties(btree-bench PROPERTIES OUTPUT_NAME bustub-btree-bench)
