file(REMOVE_RECURSE
  "../lib/libbustub.a"
  "../lib/libbustub.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/bustub.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
