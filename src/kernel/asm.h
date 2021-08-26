// Copyright (c) 2021 Steven Giacomelli. All rights reserved.
//
// Derived from the Simba project.
// Copyright (c) 2018, Erik Moqvist
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef __KERNEL_ASM_H__
#define __KERNEL_ASM_H__

/**
 * Beginning of function with given name.
 */
.macro ASM_FUNC_BEGIN, name, align.globl  \name.type   \name,
    @function.align  \align.size   \name,
    .L_\name\() _end - \name
\name :.endm

            /**
             * Beginning of weak function with given name.
             */
            .macro ASM_FUNC_WEAK_BEGIN,
    name, align.globl  \name.type   \name,
    @function.weak   \name.align  \align.size   \name,
    .L_\name\() _end - \name
\name :.endm

            /**
             * Beginning of local function with given name.
             */
            .macro ASM_FUNC_LOCAL_BEGIN,
    name, align.type   \name, @function.align  \align.size   \name,
    .L_\name\() _end - \name
\name :.endm

            /**
             * End of function with given name.
             */
            .macro ASM_FUNC_END,
    name.L_\name\() _end
    :.endm

         /**
          * Beginning of object with given name.
          */
         .macro ASM_OBJ_BEGIN,
    name, align.globl  \name.type   \name, @object.align  \align.size   \name,
    .L_\name\() _end - \name
\name :.endm

            /**
             * End of object with given name.
             */
            .macro ASM_OBJ_END,
    name.L_\name\() _end :.endm

#endif
