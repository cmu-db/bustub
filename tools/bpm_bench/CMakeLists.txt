set(BPM_BENCH_SOURCES bpm_bench.cpp)
add_executable(bpm-bench ${BPM_BENCH_SOURCES})

target_link_libraries(bpm-bench bustub)
set_target_properties(bpm-bench PROPERTIES OUTPUT_NAME bustub-bpm-bench)
