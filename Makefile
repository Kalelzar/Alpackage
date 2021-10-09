PROJECT_NAME := Alpackage

.PHONY: all debugBuild ASANBuild build test debugTest ASANTest format run debug ASAN docs bench debugBench ASANBench

all: build

# MISC

${PWD}/build/ninja:
	@cmake -G "Ninja Multi-Config" -B build/ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=true

docs:
	@doxygen doxygen

format:
	@find . -iregex ".*pp" | grep -v './build' | xargs clang-format -style file -i

# Release

build: ${PWD}/build/ninja format
	@cmake --build build/ninja -j 4

run: build
	@./build/ninja/Release/${PROJECT_NAME}_main

test: build
	@ctest -j 4 --test-dir build/ninja -C Release

retest: build
	@ctest -j 4 --rerun-failed --output-on-failure --test-dir build/ninja -C Release

bench: build
	@./build/ninja/bench/Release/$(PROJECT_NAME)_bench

coverage: build
	@cmake --build build/ninja -j 4 --config Release --target TestCoverage

# Debug

debugBuild: ${PWD}/build/ninja format
	@cmake --build build/ninja -j 4 --config Debug

debug: debugBuild
	@valgrind -s --leak-check=full ./build/ninja/Debug/$(PROJECT_NAME)_main

debugTest: debugBuild
	@valgrind -s ctest -j 4 --test-dir build/ninja --output-on-failure -C Debug

debugRetest: debugBuild
	@valgrind -s ctest -j 4 --rerun-failed --test-dir build/ninja --output-on-failure -C Debug

debugBench: debugBuild
	@valgrind -s ./build/ninja/bench/Debug/$(PROJECT_NAME)_bench

debugCoverage: build
	@cmake --build build/ninja -j 4 --config Debug --target TestCoverage

# Address Sanitizer

ASANBuild: ${PWD}/build/ninja format
	@cmake --build build/ninja -j 4 --config ASAN

ASAN: ASANBuild
	@./build/ninja/ASAN/$(PROJECT_NAME)_main

ASANTest: ASANBuild
	@ctest -j 4 --test-dir build/ninja --output-on-failure -C ASAN

ASANRetest: ASANBuild
	@ctest -j 4 --rerun-failed --test-dir build/ninja --output-on-failure -C ASAN

ASANBench: ASANBuild
	@valgrind -s ./build/ninja/bench/ASAN/$(PROJECT_NAME)_bench

ASANCoverage: build
	@cmake --build build/ninja -j 4 --config ASAN --target TestCoverage
