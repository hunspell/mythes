# Standalone makefile for the MyThes libFuzzer harness.
# Use the oss-fuzz build.sh in CI; this is for local fuzzing only.
#
#     make CXX=clang++
#     ./mythes_fuzzer -max_len=65536 corpus/
#
# Coverage (separate binary without ASAN to keep llvm-cov line mapping clean):
#
#     make coverage                       # report against corpus/
#     make coverage CORPUS=/path/to/dir   # any libFuzzer corpus
#     make coverage-html                  # browsable HTML in coverage-html/

CXX      ?= clang++
SANITIZE ?= fuzzer,address,undefined
CXXFLAGS ?= -std=c++14 -g -O1 -fsanitize=$(SANITIZE)
CPPFLAGS  = -I..

COV_CXXFLAGS  = -std=c++14 -g -O1 -fsanitize=fuzzer \
                -fprofile-instr-generate -fcoverage-mapping
LLVM_PROFDATA ?= llvm-profdata
LLVM_COV      ?= llvm-cov
CORPUS        ?= corpus

mythes_fuzzer: mythes_fuzzer.cc ../mythes.cxx ../mythes.hxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ mythes_fuzzer.cc ../mythes.cxx

mythes_fuzzer_cov: mythes_fuzzer.cc ../mythes.cxx ../mythes.hxx
	$(CXX) $(COV_CXXFLAGS) $(CPPFLAGS) -o $@ mythes_fuzzer.cc ../mythes.cxx

coverage: mythes_fuzzer_cov
	rm -f coverage.profraw coverage.profdata
	LLVM_PROFILE_FILE=coverage.profraw \
	    ./mythes_fuzzer_cov -runs=0 -max_len=4194304 $(CORPUS)
	$(LLVM_PROFDATA) merge -sparse coverage.profraw -o coverage.profdata
	$(LLVM_COV) report ./mythes_fuzzer_cov \
	    -instr-profile=coverage.profdata ../mythes.cxx ../mythes.hxx

coverage-html: coverage
	$(LLVM_COV) show ./mythes_fuzzer_cov \
	    -instr-profile=coverage.profdata \
	    -format=html -output-dir=coverage-html \
	    ../mythes.cxx ../mythes.hxx
	@echo "Open coverage-html/index.html"

clean:
	rm -f mythes_fuzzer mythes_fuzzer_cov coverage.profraw coverage.profdata
	rm -rf coverage-html

.PHONY: clean coverage coverage-html
