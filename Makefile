PROJECT_NAME := Alpackage

.PHONY: all debugBuild build test format run docs bench debugBench

all: build

${PWD}/build/ninja:
	@cmake -G "Ninja Multi-Config" -B build/ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=true

docs:
	@doxygen doxygen

build: ${PWD}/build/ninja format
	@cmake --build build/ninja -j 4

debugBuild: ${PWD}/build/ninja format
	@cmake --build build/ninja -j 4 --config Debug

test: build
	@ctest -j 4 --test-dir build/ninja -C Release

debugTest: debugBuild
	@valgrind -s ctest -j 4 --test-dir build/ninja --output-on-failure -C Debug

debugRetest: debugBuild
	@valgrind -s ctest -j 4 --rerun-failed --test-dir build/ninja --output-on-failure -C Debug

retest: build
	@ctest -j 4 --rerun-failed --test-dir build/ninja -C Release

run: build
	@./build/ninja/Release/${PROJECT_NAME}_main

debug: debugBuild
	@valgrind -s --leak-check=full ./build/ninja/Debug/$(PROJECT_NAME)_main

bench: build
	@./build/ninja/bench/Release/$(PROJECT_NAME)_bench

debugBench: debugBuild
	@valgrind -s ./build/ninja/bench/Debug/$(PROJECT_NAME)_bench

format:
	@find . -iregex ".*pp" | grep -v './build' | xargs clang-format -style file -i
