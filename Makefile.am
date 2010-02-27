ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = example

dist_bin_SCRIPTS = th_gen_idx.pl

INCLUDES = @HUNSPELL_CFLAGS@

example_SOURCES = example.cxx

lib_LTLIBRARIES = libmythes-1.2.la

libmythes_1_2_includedir = $(includedir)

libmythes_1_2_la_SOURCES = mythes.cxx

libmythes_1_2_include_HEADERS = mythes.hxx

example_LDADD = libmythes-1.2.la @HUNSPELL_LIBS@

pkgconfdir = $(libdir)/pkgconfig
pkgconf_DATA = mythes.pc

EXTRA_DIST = th_en_US_new.idx th_en_US_new.dat checkme.lst \
  morph.idx morph.dat morph.lst morph.aff morph.dic \
  mythes.pc.in data_layout.txt makefile.mk \
  WordNet_license.txt WordNet_readme.txt
