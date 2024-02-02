set CC=clang-cl
set CXX=clang-cl
meson setup --backend=ninja --buildtype=debug build\debug -Denable_trace=true
