PROJECT_NAME := Alpackage

.PHONY: all debugBuild ASANBuild build test debugTest ASANTest format run debug ASAN docs bench debugBench ASANBench install debugInstall ASANInstall
all: build

# MISC

${PWD}/build/ninja:
	cmake -G "Ninja Multi-Config" -B build/ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=true

clean:
	rm build/ninja -rf

docs:
	doxygen doxygen

format:
	find . -iregex ".*pp" | grep -v -E './build|./lib' | xargs clang-format -style file -i

tidy:
	find . -iregex ".*pp" | grep -v -E './build|./lib' | xargs clang-tidy include/Alpackage/Package.hpp -checks="-*,clang-analyzer-*,bugprone-*,boost-*,cert-*,cppcoreguidelines-*,hicpp-*,modernize-*,-modernize-use-trailing-return-type,-modernize-pass-by-value,performance-*,portability-*,readability-*" -fix


# Release

build: ${PWD}/build/ninja format
	cmake --build build/ninja -j 4

run: build
	./build/ninja/Release/alpkg

test: build
	ctest -j 4 --test-dir build/ninja/test -C Release

testAll: build
	ctest -j 4 --test-dir build/ninja -C Release

retest: build
	ctest -j 4 --rerun-failed --output-on-failure --test-dir build/ninja/test -C Release

retestAll: build
	ctest -j 4 --rerun-failed --output-on-failure --test-dir build/ninja

bench: build
	./build/ninja/bench/Release/$(PROJECT_NAME)_bench

coverage: build
	cmake --build build/ninja -j 4 --config Release --target TestCoverage

install: build
	sudo cmake --install build/ninja --config Release --prefix /usr

# Debug

debugBuild: ${PWD}/build/ninja format
	cmake --build build/ninja -j 4 --config Debug

debug: debugBuild
	valgrind -s --leak-check=full ./build/ninja/Debug/alpkg

debugTest: debugBuild
	valgrind -s ctest -j 4 --test-dir build/ninja --output-on-failure -C Debug

debugRetest: debugBuild
	valgrind -s ctest -j 4 --rerun-failed --test-dir build/ninja --output-on-failure -C Debug

debugBench: debugBuild
	valgrind -s ./build/ninja/bench/Debug/$(PROJECT_NAME)_bench

debugCoverage: build
	cmake --build build/ninja -j 4 --config Debug --target TestCoverage

debugInstall: debugBuild
	sudo cmake --install build/ninja --config Debug --prefix /usr
