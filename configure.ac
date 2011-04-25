AC_INIT([stuff], [0.1], [stuff-reports@gcarrier.fr])

AM_INIT_AUTOMAKE(foreign dist-lzma silent-rules )
AM_SILENT_RULES

AC_USE_SYSTEM_EXTENSIONS

AC_CONFIG_SRCDIR([fs/wtfitf.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/ioctl.h sys/param.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([memmove memset munmap select strchr strerror])

AC_CONFIG_FILES([
  Makefile
  auth/Makefile
  diagnostics/Makefile
  fs/Makefile
  fun/Makefile
  mem/Makefile
  net/Makefile
  sys/Makefile
])

AC_OUTPUT
