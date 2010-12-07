# $Id$
#
# SYNOPSIS
#
#   AX_DOCUTILS([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Test for the Docutils -- Python driven reStructuredText processor.
#   This macro calls (through AC_CHECK_PROGS)::
#
#     AC_SUBST(RST2HTML)
#     AC_SUBST(RST2LATEX)
#
# LAST MODIFICATION
#
#   2007-12-14
#
# LICENSE
#
#   Written by Łukasz Cieśnik <lukasz.ciesnik@fedstage.com>
#   and placed under Public Domain
#

AC_DEFUN([AX_DOCUTILS], [
	AC_CHECK_PROGS([RST2HTML], [rst2html rst2html.py])
	AC_CHECK_PROGS([RST2LATEX], [rst2latex rst2latex.py])
	if test x$RST2HTML != x -a x$RST2LATEX != x; then
		ifelse([$1], , :, [$1])
	else
		ifelse([$2], , :, [$2])
	fi
])
