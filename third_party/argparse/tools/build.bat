call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DARGPARSE_BUILD_TESTS=ON
ninja -C build
