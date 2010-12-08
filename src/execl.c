/*
    libfakechroot -- fake chroot environment
    Copyright (c) 2010 Piotr Roszatycki <dexter@debian.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/


#include <config.h>

#define _GNU_SOURCE
#include <stdarg.h>
#include <alloca.h>
#include <stddef.h>
#include <unistd.h>
#include "libfakechroot.h"

#ifndef __GLIBC__
extern char **environ;
#endif


/*
   execl function taken from GNU C Library.
   Copyright (C) 1991,92,94,97,98,99,2002,2005 Free Software Foundation, Inc.
 */
wrapper(execl, int, (const char * path, const char * arg, ...))
{
    size_t argv_max = 1024;
    const char **argv = alloca(argv_max * sizeof(const char *));
    unsigned int i;
    va_list args;

    debug("execl(\"%s\", \"%s\", ...)", path, arg);
    argv[0] = arg;

    va_start(args, arg);
    i = 0;
    while (argv[i++] != NULL) {
        if (i == argv_max) {
            const char **nptr = alloca((argv_max *= 2) * sizeof(const char *));

            if ((char *) argv + i == (char *) nptr)
                /* Stack grows up.  */
                argv_max += i;
            else
                /* We have a hole in the stack.  */
                argv = (const char **) memcpy(nptr, argv, i
                        * sizeof(const char *));
        }

        argv[i] = va_arg(args, const char *);
    }
    va_end(args);

    return execve(path, (char * const *) argv, environ);
}