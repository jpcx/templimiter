# Provide README.md in distributions
dist_doc_DATA = README.md

# List headers and extra inclusions for dist
EXTRA_DIST = scripts/fix-timestamps.sh                                         \
             src/version.h                                                     \
             src/templimiter/io/file-collection.h                              \
             src/templimiter/io/file.h                                         \
             src/templimiter/io/operations.h                                   \
             src/templimiter/error/config-error.h                              \
             src/templimiter/error/argument-error.h                            \
             src/templimiter/error/error.h                                     \
             src/templimiter/error/io-error.h                                  \
             src/templimiter/error/type-error.h                                \
             src/templimiter/error/internal-error.h                            \
             src/templimiter/tools/vector.h                                    \
             src/templimiter/tools/type-convert.h                              \
             src/templimiter/tools/string.h                                    \
             src/templimiter/daemon/pid.h                                      \
             src/templimiter/daemon/logger.h                                   \
             src/templimiter/daemon/config.h                                   \
             src/templimiter/daemon/monitor.h                                  \
             system/templimiter.conf                                           \
             system/templimiter.service                                        \
             LICENSE

# Define executables
bin_PROGRAMS = templimiter

# Define templimiter sources
templimiter_SOURCES = src/main.cc                                              \
                      src/templimiter/daemon/config.cc                         \
                      src/templimiter/daemon/logger.cc                         \
                      src/templimiter/daemon/monitor.cc                        \
                      src/templimiter/daemon/pid.cc                            \
                      src/templimiter/error/argument-error.cc                  \
                      src/templimiter/error/config-error.cc                    \
                      src/templimiter/error/error.cc                           \
                      src/templimiter/error/internal-error.cc                  \
                      src/templimiter/error/io-error.cc                        \
                      src/templimiter/error/type-error.cc                      \
                      src/templimiter/io/operations.cc                         \
                      src/templimiter/tools/string.cc                          \
                      src/templimiter/tools/vector.cc

dist_man_MANS = $(top_srcdir)/system/templimiter.8

# Provides systemd ./configure flag
AM_DISTCHECK_CONFIGURE_FLAGS =                                                 \
  --with-systemdsystemunitdir='$$(prefix)/$(systemdsystemunitdir)'             \
  --with-templimiterconfdir='$$(prefix)/$(templimiterconfdir)'

# Set templimiter.service installation if enabled
if HAVE_SYSTEMD
  systemdsystemunit_DATA = $(top_srcdir)/system/templimiter.service
endif

# Create installation parameters for configuration file
templimiterconf_DATA = $(top_srcdir)/system/templimiter.conf

# Set c++ flags
templimiter_CPPFLAGS = -I$(top_builddir)/src -I$(top_srcdir)/src -std=c++17