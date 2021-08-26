// Copyright (c) 2021 Steven Giacomelli. All rights reserved.
//
// Derived from the Simba project.
// Copyright (c) 2014-2018, Erik Moqvist
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef __KERNEL_COMPILER_H__
#define __KERNEL_COMPILER_H__

#include "far.h"

typedef const FAR char *far_string_t;

/**
 * @brief No special storage for string literals.
 */
#define FSTR(s) s

#define RAM_CODE

#define PACKED __attribute__((packed))

char *strsep(char **string_pp, const char *delim_p);

#endif
