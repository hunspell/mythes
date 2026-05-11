#!/bin/bash -eu
# OSS-Fuzz build script. Invoked from the OSS-Fuzz base image with
# CC, CXX, CFLAGS, CXXFLAGS, LIB_FUZZING_ENGINE, OUT, SRC pre-set.

cd "$SRC/mythes"

$CXX $CXXFLAGS -std=c++14 -I. \
    fuzz/mythes_fuzzer.cc mythes.cxx \
    $LIB_FUZZING_ENGINE \
    -o "$OUT/mythes_fuzzer"

if [ -d fuzz/corpus ]; then
    zip -j "$OUT/mythes_fuzzer_seed_corpus.zip" fuzz/corpus/*
fi
