#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Autoconf initialization.
AC_PREREQ([2.69])
AC_INIT([templimiter], [0.1.0], [jpcxist@gmail.com])
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

# Automake initialization.
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for types.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for headers.
AC_CHECK_FUNCS([mkdir stime utime])
AC_CHECK_HEADER_STDBOOL

# Set pkgconf program variable.
PKG_PROG_PKG_CONFIG

# Check for --with-systemdsystemunitdir flag; provide help text
AC_ARG_WITH([systemdsystemunitdir],
  [
    AS_HELP_STRING(
      [--with-systemdsystemunitdir=DIR],
      [Directory for systemd service files]
    )
  ],
  [],
  [with_systemdsystemunitdir=auto])

# Checks for systemd enabled status
AS_IF([test "x$with_systemdsystemunitdir" = "xyes" -o "x$with_systemdsystemunitdir" = "xauto"],
  [
    # Define pkgconf systemdsystemunitdir check for systemd
    def_systemdsystemunitdir=$(
      $PKG_CONFIG --variable=systemdsystemunitdir systemd
    )

    # Check if systemd is NOT defined
    AS_IF([test "x$def_systemdsystemunitdir" = "x"],
      [
        # Check if systemd is enabled with ./configure
        AS_IF([test "x$with_systemdsystemunitdir" = "xyes"],
          [
            # Throw error if attempted enable
            AC_MSG_ERROR(
              [systemd support requested but pkg-config unable to query systemd package]
            )
          ])
        # Disable
        with_systemdsystemunitdir=no
      ],
      [
        # If defined, set the intermediate path
        with_systemdsystemunitdir="$def_systemdsystemunitdir"
      ])
  ])

# If systemd is selected, define the systemdsystemunitdir variable with the
#   resulting systemd path
AS_IF([test "x$with_systemdsystemunitdir" != "xno"],
  [AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])])

# Set systemd condition
AM_CONDITIONAL([HAVE_SYSTEMD], [test "x$with_systemdsystemunitdir" != "xno"])

# Check for --with-templimiterconfdir flag; provide help text
AC_ARG_WITH([templimiterconfdir],
  [
    AS_HELP_STRING(
      [--with-templimiterconfdir=DIR],
      [Directory for configuration file]
    )
  ],
  [],
  [with_templimiterconfdir=auto])

AS_IF([test "x$with_templimiterconfdir" = "xauto"],
  [with_templimiterconfdir=$sysconfdir/conf.d])

AC_DEFINE_UNQUOTED([TEMPLIMITER_CONFIG_PATH],
  [$with_templimiterconfdir],
  [Define the templimiter config path macro])

AC_SUBST([templimiterconfdir], [$with_templimiterconfdir])

# Create configuration files.
AC_CONFIG_FILES([Makefile])

# Finished; output
AC_OUTPUT