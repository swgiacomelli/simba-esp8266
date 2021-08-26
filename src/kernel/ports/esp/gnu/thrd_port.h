// Copyright (c) 2021 Steven Giacomelli. All rights reserved.
//
// Derived from the Simba project.
// Copyright (c) 2014-2018, Erik Moqvist
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef __KERNEL_THRD_PORT_H__
#define __KERNEL_THRD_PORT_H__

#if CONFIG_PREEMPTIVE_SCHEDULER == 1
#error "This port does not support a preemptive scheduler."
#endif

#define THRD_PORT_STACK(name, size)                                         \
  uint32_t name[DIV_CEIL(sizeof(struct thrd_t) + (size), sizeof(uint32_t))] \
      __attribute__((section(".simba_other_stacks")))

struct thrd_port_context_t {
  uint32_t a0; /* Return addess from the swap function. */
  uint32_t a12;
  uint32_t a13;
  uint32_t a14;
  uint32_t a15;
};

struct thrd_port_t {
  struct thrd_port_context_t *context_p;
};

#endif
