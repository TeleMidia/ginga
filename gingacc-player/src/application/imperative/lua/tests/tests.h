/* tests.h -- Common declarations used by NCLua tests.
   Copyright (C) 2012 PUC-Rio/Laboratorio TeleMidia

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef TESTS_H
#define TESTS_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* ANSI C headers.  */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* C99 headers.  */
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

/* NCLua public headers.  */
#include "nclua.h"

/* Predefined sizes for test samples.  */
#define SMALL_SAMPLE    1024            /* 1K */
#define MEDIUM_SAMPLE   65536           /* 64K */
#define BIG_SAMPLE      1048576         /* 1M */
#define HUGE_SAMPLE     67108864        /* 64M */

/* Default sample size.  */
#define DEFAULT_SAMPLE  BIG_SAMPLE

#endif /* TESTS_H */
