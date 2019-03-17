ACLOCAL_AMFLAGS = -I m4

version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

AM_CXXFLAGS = \
	$(HUNSPELL_CFLAGS) \
	$(DEBUG_CXXFLAGS)

check_PROGRAMS = example
example_SOURCES = example.cxx
example_LDADD = \
	libmythes-@MYTHES_MAJOR_VERSION@.@MYTHES_MINOR_VERSION@.la \
	$(HUNSPELL_LIBS)

dist_bin_SCRIPTS = th_gen_idx.pl

lib_LTLIBRARIES = libmythes-@MYTHES_MAJOR_VERSION@.@MYTHES_MINOR_VERSION@.la
libmythes_@MYTHES_MAJOR_VERSION@_@MYTHES_MINOR_VERSION@_la_SOURCES = mythes.cxx
libmythes_@MYTHES_MAJOR_VERSION@_@MYTHES_MINOR_VERSION@_includedir = $(includedir)
libmythes_@MYTHES_MAJOR_VERSION@_@MYTHES_MINOR_VERSION@_include_HEADERS = mythes.hxx
libmythes_@MYTHES_MAJOR_VERSION@_@MYTHES_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined

pkgconfdir = $(libdir)/pkgconfig
pkgconf_DATA = mythes.pc

EXTRA_DIST = \
	th_en_US_new.idx \
	th_en_US_new.dat \
	checkme.lst \
	morph.idx \
	morph.dat \
	morph.lst \
	morph.aff \
	morph.dic \
	mythes.pc.in \
	data_layout.txt \
	makefile.mk \
	WordNet_license.txt \
	WordNet_readme.txt

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae> [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog

