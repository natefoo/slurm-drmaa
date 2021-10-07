#
# SYNOPSIS
#
#   AX_SLURM([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Check for SLURM libraries and headers.
#
#   This macro caslurms::
#
#     AC_SUBST(SLURM_INCLUDES)
#     AC_SUBST(SLURM_LDFLAGS)
#     AC_SUBST(SLURM_LIBS)
#
# LAST MODIFICATION
#
#   2010-09-09
#
# LICENSE
#
#   Written by:
#     Michał Matłoka <michal.matloka@student.put.poznan.pl>
#   Based on macro for LSF written by
#     Mariusz Mamoński <mamonski@man.poznan.pl>
#   Placed under Public Domain.
#

AC_DEFUN([AX_SLURM],[
AC_ARG_WITH([slurm-inc], [AS_HELP_STRING([--with-slurm-inc=<include-dir>],
		[Path to headers directory (containing slurm/slurm.h)])])
AC_ARG_WITH([slurm-lib], [AS_HELP_STRING([--with-slurm-lib=<lib-dir>],
		[Path to directory with SLURM libraries (containing libslurm.a)])])

AC_SUBST(SLURM_INCLUDES)
AC_SUBST(SLURM_LDFLAGS)
AC_SUBST(SLURM_LIBS)

AC_MSG_NOTICE([checking for SLURM])

AC_MSG_CHECKING([for SLURM compile flags])
ax_slurm_msg=""
if test x$with_slurm_inc != x; then
	SLURM_INCLUDES="-I${with_slurm_inc}"
else
	SRUN_PATH=`command -v srun`
	if test x"$SRUN_PATH" != x; then
		SRUN_DIR=`dirname $SRUN_PATH`
		SLURM_HOME=`dirname $SRUN_DIR`
		SLURM_INCLUDES="-I$SLURM_HOME/include"
	else
		ax_slurm_msg="no srun in PATH"
	fi
fi

AC_MSG_RESULT([$SLURM_INCLUDES$ax_slurm_msg])

AC_MSG_CHECKING([for SLURM library dir])
ax_slurm_msg=""
if test x$with_slurm_lib == x; then
	
	SRUN_PATH=`command -v srun`
	
	if test x"$SRUN_PATH" != x; then
		SRUN_DIR=`dirname $SRUN_PATH`
		SLURM_HOME=`dirname $SRUN_DIR`
		with_slurm_lib=$SLURM_HOME/lib
	else
		ax_slurm_msg="no srun in PATH"
	fi
fi
AC_MSG_RESULT([$with_slurm_lib$ax_slurm_msg])

SLURM_LIBS="-lslurm "
SLURM_LDFLAGS="-L${with_slurm_lib}"

CPPFLAGS_save="$CPPFLAGS"
LDFLAGS_save="$LDFLAGS"
LIBS_save="$LIBS"
CPPFLAGS="$CPPFLAGS $SLURM_INCLUDES"
LDFLAGS="$LDFLAGS -Wl,-rpath=${with_slurm_lib} $SLURM_LDFLAGS"
LIBS="$LIBS $SLURM_LIBS"

ax_slurm_ok="no"

AC_MSG_CHECKING([for usable SLURM libraries/headers])
dnl The original test just checked for a struct existence, but that was optimized out,
dnl so now the test calls a utility function that doesn't require a server
AC_RUN_IFELSE([AC_LANG_PROGRAM([[ #include "slurm/slurm.h" ]],
		[[ slurm_hostlist_create("conftest");
		   return 0;
		 ]])],
	[ ax_slurm_ok="yes"],
	[ echo "*** The SLURM test program failed to link or run. See the file config.log"
	  echo "*** for the exact error that occured."],
	[
		ax_slurm_ok=yes
		echo $ac_n "cross compiling; assumed OK... $ac_c"
	])

AC_MSG_RESULT([$ax_slurm_ok])

dnl Check if slurmdb functions have been merged into the slurm library
dnl If slurmdb has not been merged, add it to the working slurm libs
AC_CHECK_LIB(slurm, slurmdb_users_get, [], [SLURM_LIBS="$SLURM_LIBS-lslurmdb "])
AC_MSG_NOTICE(Using slurm libraries $SLURM_LIBS)

CPPFLAGS="$CPPFLAGS_save"
LDFLAGS="$LDFLAGS_save"
LIBS="$LIBS_save"

if test x"$ax_slurm_ok" = xyes; then
	ifelse([$1], , :, [$1])
else
	ifelse([$2], , :, [$2])
fi
])
