# OSS Scanner build for MyThes (https://github.com/hunspell/mythes).
#
# Three builds of the same checkout:
#
#   /src/build-asan   clang++, AddressSanitizer and UndefinedBehaviorSanitizer, -O1 -g. The library and
#                     the example program.
#   /src/fuzz         mythes_fuzzer, the libFuzzer target in fuzz/mythes_fuzzer.cc, built with the same
#                     sanitizers by fuzz/GNUmakefile. It runs the files named on its command line and exits.
#   /src              a plain -O2 -g build with frame pointers, installed to /usr/local.
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
      ca-certificates git make autoconf automake libtool pkg-config perl \
      clang libclang-rt-18-dev llvm g++ gdb valgrind libhunspell-dev \
    && rm -rf /var/lib/apt/lists/*

# scanner contract: the checkout lives inside the image, at /src
COPY . /src
WORKDIR /src

RUN autoreconf -vfi

# Sanitizer build, out of tree so the plain build below can configure in place.
RUN mkdir /src/build-asan && cd /src/build-asan \
    && CXX=clang++ \
       CXXFLAGS="-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=undefined" \
       LDFLAGS="-fsanitize=address,undefined" \
       ../configure --disable-shared --enable-static \
    && make -j"$(nproc)" \
    && make -j"$(nproc)" check \
    && ./example ../th_en_US_new.idx ../th_en_US_new.dat ../checkme.lst > /dev/null

RUN make -C fuzz CXX=clang++

# Plain build, as distributions build it.
RUN CXXFLAGS="-O2 -g -fno-omit-frame-pointer" ./configure \
    && make -j"$(nproc)" && make install && ldconfig \
    && make -j"$(nproc)" check
