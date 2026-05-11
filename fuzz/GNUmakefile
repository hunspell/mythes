# Standalone makefile for the MyThes libFuzzer harness.
# Use the oss-fuzz build.sh in CI; this is for local fuzzing only.
#
#     make CXX=clang++
#     ./mythes_fuzzer -max_len=65536 corpus/

CXX      ?= clang++
SANITIZE ?= fuzzer,address,undefined
CXXFLAGS ?= -std=c++14 -g -O1 -fsanitize=$(SANITIZE)
CPPFLAGS  = -I..

mythes_fuzzer: mythes_fuzzer.cc ../mythes.cxx ../mythes.hxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ mythes_fuzzer.cc ../mythes.cxx

clean:
	rm -f mythes_fuzzer

.PHONY: clean
