#!/bin/sh

PROJECT_ROOT=$(dirname $(dirname $(readlink -fm $0)))

touch $PROJECT_ROOT/aclocal.m4 $PROJECT_ROOT/config.h.in                       \
      $PROJECT_ROOT/configure  $PROJECT_ROOT/Makefile.in

echo "Fixed!"