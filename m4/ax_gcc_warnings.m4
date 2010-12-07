# $Id$
#
# SYNOPSIS
#
#   AX_GCC_FLAGS()
#
# DESCRIPTION
#
#   Check for LSF libraries and headers.
#
#   This macro calls::
#
#			AC_SUBST(GCC_W_NO_MISSING_FIELD_INITIALIZERS)
#			AC_SUBST(GCC_W_NO_FORMAT_ZERO_LENGTH)
#
# LAST MODIFICATION
#
#   2008-10-13
#
# LICENSE
#
#   Written by Łukasz Cieśnik <lukasz.ciesnik@fedstage.com>
#   and placed under Public Domain.
#
AC_DEFUN([AX_GCC_WARNINGS], [
if test x$GCC == xyes; then
	AC_SUBST([GCC_W_NO_MISSING_FIELD_INITIALIZERS])
	AC_SUBST([GCC_W_NO_FORMAT_ZERO_LENGTH])
	CFLAGS_save="$CFLAGS"

	AC_MSG_CHECKING([whether gcc accepts -Wno-missing-field-initializers])
	CFLAGS="$CFLAGS_save -Wall -Wextra -Wno-missing-field-initializers"
	ax_gcc_warnings_src="int main(){ return 0; }"
	AC_COMPILE_IFELSE([$ax_gcc_warnings_src],
		[GCC_W_NO_MISSING_FIELD_INITIALIZERS="-Wno-missing-field-initializers"
		AC_MSG_RESULT([yes])],
		[GCC_W_NO_MISSING_FIELD_INITIALIZERS="-Wno-extra"
		AC_MSG_RESULT([no])])

	AC_MSG_CHECKING([whether gcc accepts -Wno-format-zero-length])
	CFLAGS="$CFLAGS_save -Wno-format-zero-length"
	AC_COMPILE_IFELSE([$ax_gcc_warnings_src],
		[GCC_W_NO_FORMAT_ZERO_LENGTH="-Wno-format-zero-length"
		AC_MSG_RESULT([yes])],
		[GCC_W_NO_FORMAT_ZERO_LENGTH="-Wno-format"
		AC_MSG_RESULT([no])])

	CFLAGS="$CFLAGS_save"
fi
])
