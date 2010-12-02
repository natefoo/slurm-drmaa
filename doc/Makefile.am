# $Id: Makefile.am 92 2010-04-08 15:40:49Z mamonski $
#
# PSNC DRMAA for LoadLeveler
# Copyright (C) 2010 Poznan Supercomputing and Networking Center
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

DOCUTILSFLAGS = \
 -i utf-8 \
 -o utf-8 \
 --language=en \
 --file-insertion-enabled \
 --no-generator \
 --tab-width=4
RST2HTMLFLAGS = $(DOCUTILSFLAGS) \
 --stylesheet=stylesheet.css \
 --link-stylesheet
RST2LATEXFLAGS = $(DOCUTILSFLAGS) \
 --documentclass=article \
 --documentoptions=a4paper,10pt \
 --font-encoding=OT4 \
 --graphicx-option=pdftex \
 --use-latex-footnotes \
 --footnote-references=superscript \
 --use-latex-citations \
 --use-latex-toc

readme.html: $(top_srcdir)/README
	$(RST2HTML) $(RST2HTMLFLAGS) $< $@
readme.tex: $(top_srcdir)/README
	$(RST2LATEX) $(RST2LATEXFLAGS) $< $@
.tex.pdf:
	pdflatex $< && pdflatex $<

noinst_DATA = readme.html readme.pdf
EXTRA_DIST = readme.html readme.tex readme.pdf

clean-local:
	-rm -rf *.log *.aux *.toc *.out
MAINTAINERCLEANFILES = readme.html readme.tex readme.pdf

