# Threat model

## What this project does and where untrusted input enters

MyThes is a small C++ thesaurus library. It looks words up in a thesaurus made of two files: an
index (.idx) and a data file (.dat), whose format is in data_layout.txt. Office suites embed it.

Two inputs are untrusted:

- The thesaurus files. Users install thesaurus dictionaries from third-party extensions, so the
  .idx and .dat content can be written by an attacker. The MyThes constructor reads the whole
  index, and MyThes::Lookup reads records from the data file at offsets that the index gives.
- The word passed to MyThes::Lookup, with its length. It comes from a document.

The paths passed to the constructor are chosen by the calling program and are trusted.

## Components that matter most and least

- Matters most: mythes.cxx and mythes.hxx, the library.
- Lower: example.cxx, a demonstration program that looks up a word list.
- Out of scope: th_gen_idx.pl, which builds an index at packaging time, and the sample thesaurus
  files.

## How to exercise it

- /src/fuzz/mythes_fuzzer is the libFuzzer target, built with AddressSanitizer and
  UndefinedBehaviorSanitizer. Its input is a 16-bit index length, the index, then the data file.
  It looks up every word that the index names. It runs the files named on its command line, or
  fuzzes when given a corpus directory such as /src/fuzz/corpus.
- /src/build-asan/example is the example program built with the same sanitizers:
  "example <file.idx> <file.dat> <file of words> [<file.aff> <file.dic>]". The optional Hunspell
  files add stemming.
- /src/th_en_US_new.idx and /src/th_en_US_new.dat are a full real-world thesaurus, with
  /src/checkme.lst as a word list. The morph.* files are a small thesaurus with Hunspell files.

## How we rate severity

- An out of bounds write, use after free or double free reached from a thesaurus file or a word:
  critical.
- An out of bounds read reached the same way: high.
- A null dereference, assertion failure or other clean crash: medium.
- Slow lookups, high memory use, running out of memory and timeouts: out of scope.
- Missing or wrong synonyms: not security issues.

## Anything to leave alone

- MAX_MEANINGS in mythes.hxx caps the meanings read for one word, far above any real thesaurus.
  Raising it is not a fix.
- The caller frees the result of Lookup with CleanUpAfterLookup, passing the count that Lookup
  returned. A bug that needs a caller to pass a different count is a bug in the caller.

## How reports and patches should look

A reproducer file and the sanitizer output are the most useful parts. A patch should be the
smallest change that fixes the bug, on master, in the style of the surrounding code.
