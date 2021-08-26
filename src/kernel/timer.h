// Copyright (c) 2021 Steven Giacomelli. All rights reserved.
//
// Derived from the Simba project.
// Copyright (c) 2014-2018, Erik Moqvist
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef __KERNEL_TIMER_H__
#define __KERNEL_TIMER_H__

#include "simba.h"

/**
 * A timer is "single shot" per default. Initialize a timer with this
 * flag set in the ``flags`` argument to configure it as periodic.
 *
 * A periodic timer will call the function callback periodically. This
 * continues until the timer is stopped.
 */
#define TIMER_PERIODIC (1 << 0)

/** Timer callback prototype. */
typedef void (*timer_callback_t)(void *arg_p);

/* Timer. */
struct timer_t {
  struct timer_t *next_p;
  uint32_t delta;
  uint32_t timeout;
  int flags;
  timer_callback_t callback;
  void *arg_p;
};

/**
 * Initialize the timer module. This function must be called before
 * calling any other function in this module.
 *
 * The module will only be initialized once even if this function is
 * called multiple times.
 *
 * @return zero(0) or negative error code.
 */
int timer_module_init(void);

/**
 * Initialize given timer object with given timeout and expiry
 * callback. The timer resolution directly depends on the system tick
 * frequency and is rounded up to the closest possible value. This
 * applies to both single shot and periodic timers. Some ports support
 * high resolution timers, which often have higher resolution than the
 * system tick.
 *
 * @param[in] self_p Timer object to initialize with given parameters.
 * @param[in] timeout_p The timer timeout value.
 * @param[in] callback Functaion called when the timer expires. Called
 *                     from interrupt context.
 * @param[in] arg_p Function callback argument. Passed to the callback
 *                  when the timer expires.
 * @param[in] flags Set TIMER_PERIODIC for periodic timer.
 *
 * @return zero(0) or negative error code.
 */
int timer_init(struct timer_t *self_p, const struct time_t *timeout_p,
               timer_callback_t callback, void *arg_p, int flags);

/**
 * Start given initialized timer object.
 *
 * @param[in] self_p Timer object to start.
 *
 * @return zero(0) or negative error code.
 */
int timer_start(struct timer_t *self_p);

/**
 * See `timer_start()` for a description.
 *
 * This function may only be called from an isr or with the system
 * lock taken (see `sys_lock()`).
 */
int timer_start_isr(struct timer_t *self_p);

/**
 * Stop given timer object. This has no effect on a timer that already
 * expired or was never started. A stopped timer may be restarted with
 * the initial timeout by calling `timer_start()`.
 *
 * @param[in] self_p Timer object to stop.
 *
 * @return true(1) if the timer was stopped, false(0) if the timer
 *         already expired or was never started, and otherwise
 *         negative error code.
 */
int timer_stop(struct timer_t *self_p);

/**
 * See `timer_stop()` for description.
 *
 * This function may only be called from an isr or with the system
 * lock taken (see `sys_lock()`).
 */
int timer_stop_isr(struct timer_t *self_p);

#endif
