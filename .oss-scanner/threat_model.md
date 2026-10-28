# Threat model

## What this project does and where untrusted input enters

MyThes is a small C++ thesaurus library. It looks words up in a thesaurus made of two files: an
index (.idx) and a data file (.dat), whose format is in data_layout.txt. Office suites embed it.

The one untrusted input is the word passed to MyThes::Lookup. It comes from a document, in the
thesaurus encoding, of any length. The length passed with it is the caller's count of the bytes it
hands over, so a length longer than the caller's own buffer is a bug in the caller.

The thesaurus files are installed data, not untrusted input. They come from a distribution
package or from an extension the user chose to install, and an office suite extension can carry
code that runs as the user, so whoever can place a thesaurus on a machine can already run
programs there. The library is still hardened against malformed files and the fuzzer feeds it
malformed files, because a crash on a broken thesaurus is a bug we fix. It is not a security
boundary: a bug that needs a crafted .idx or .dat is fixed on master as an ordinary bug, credited
in the commit, with no advisory and no CVE.

The paths passed to the constructor are chosen by the calling program and are trusted. The
Hunspell affix and dictionary files the example program can take are Hunspell's input; a crash
inside Hunspell belongs to https://github.com/hunspell/hunspell.

## Components that matter most and least

- Matters most: mythes.cxx and mythes.hxx, the library. MyThes::Lookup is the only function the
  word reaches.
- Not security: example.cxx, a test driver that make check builds and nothing installs, and
  th_gen_idx.pl, which builds an index at packaging time. Bugs there are welcome as bugs.
- Out of scope: the sample thesaurus files.

## How to exercise it

- /src/fuzz/mythes_fuzzer is the libFuzzer target, built with AddressSanitizer and
  UndefinedBehaviorSanitizer. Its input is a 16-bit index length, the index, then the data file.
  It looks up the words that the index names, under a budget of synonyms copied per input, and a
  few fixed words. It runs the files named on its command line, or fuzzes when given a corpus
  directory such as /src/fuzz/corpus.
- /src/build-asan/example is the example program built with the same sanitizers:
  "example <file.idx> <file.dat> <file of words> [<file.aff> <file.dic>]". The optional Hunspell
  files add stemming.
- /src/th_en_US_new.idx and /src/th_en_US_new.dat are a full real-world thesaurus, with
  /src/checkme.lst as a word list. The morph.* files are a small thesaurus with Hunspell files.

## How we rate severity

- An out of bounds write, use after free or double free reached from a word looked up in a
  well-formed thesaurus: critical.
- An out of bounds read reached the same way: high.
- A null dereference, assertion failure or other clean crash reached the same way: medium.
- Anything reached only with a malformed thesaurus file: a bug, not a security finding, whatever
  the sanitizer prints. We still want it.
- A crash that needs an allocation to fail first, slow lookups, high memory use, running out of
  memory and timeouts: out of scope.
- Missing or wrong synonyms: not security issues.

## Anything to leave alone

- MAX_MEANINGS in mythes.hxx: a record whose meaning count is above it returns no meanings, and
  no real thesaurus comes near it. Raising it is not a fix.
- The caller frees the result of Lookup with CleanUpAfterLookup, passing the count that Lookup
  returned. A bug that needs a caller to pass a different count is a bug in the caller.

## How reports and patches should look

One report per root cause, with the input that triggers it and the sanitizer output; several
crashes with one cause are one report. A finding against code that is in no release is fixed on
master without an advisory. A variant of an issue already reported, or of a fix under review,
earns no separate credit. A patch should be the smallest change that fixes the bug, on master, in
the style of the surrounding code.
