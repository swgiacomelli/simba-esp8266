/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018, Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of the Simba project.
 */

#ifndef __SYNC_EVENT_H__
#define __SYNC_EVENT_H__

#include "simba.h"

/**
 * Event channel.
 */
struct event_t {
  struct chan_t base;
  uint32_t mask;        /* Events that occured. */
  uint32_t reader_mask; /* Events the reader are waiting for. */
};

/**
 * Initialize given event channel.
 *
 * @param[in] self_p Event channel to initialize.
 *
 * @return zero(0) or negative error code
 */
int event_init(struct event_t *self_p);

/**
 * Wait for one or more events to occur in given event mask. This
 * function blocks until at least one of the events in the event mask
 * has been set. When the function returns, given event mask has been
 * overwritten with the events that actually occured. Read events are
 * automatically cleared.
 *
 * @param[in] self_p Event channel object.
 * @param[in, out] buf_p The mask of events to wait for. When the
 *                       function returns the mask contains the events
 *                       that have occured.
 * @param[in] size Size to read (always sizeof(mask)).
 *
 * @return sizeof(mask) or negative error code.
 */
ssize_t event_read(struct event_t *self_p, void *buf_p, size_t size);

/**
 * Try to read one or more events in given event mask. This function
 * returns immediately, even if no event has occured. When the
 * function returns, given mask has been overwritten with the events
 * that actually occured. Read events are automatically cleared.
 *
 * @param[in] self_p Event channel object.
 * @param[in, out] buf_p The mask of events to wait for. When the
 *                       function returns the mask contains the events
 *                       that have occured.
 * @param[in] size Size to read (always sizeof(mask)).
 *
 * @return sizeof(mask) on success, -EAGAIN if no event was read, and
 *         otherwise other negative error code.
 */
ssize_t event_try_read(struct event_t *self_p, void *buf_p, size_t size);

/**
 * Write given event(s) to given event channel.
 *
 * @param[in] self_p Event channel object.
 * @param[in] buf_p The mask of events to write.
 * @param[in] size Must always be sizeof(mask).
 *
 * @return sizeof(mask) or negative error code.
 */
ssize_t event_write(struct event_t *self_p, const void *buf_p, size_t size);

/**
 * Write given events to the event channel from isr or with the system
 * lock taken (see `sys_lock()`).
 *
 * @param[in] self_p Event channel object.
 * @param[in] buf_p The mask of events to write.
 * @param[in] size Must always be sizeof(mask).
 *
 * @return sizeof(mask) or negative error code.
 */
ssize_t event_write_isr(struct event_t *self_p, const void *buf_p, size_t size);

/**
 * Checks if there are events active on the event channel.
 *
 * @param[in] self_p Event channel object.
 *
 * @return one(1) is at least one event has occured, otherwise
 *         zero(0).
 */
ssize_t event_size(struct event_t *self_p);

/**
 * Clear given events on the event channel.
 *
 * @param[in] self_p Event channel object.
 * @param[in] mask The mask of events to clear.
 *
 * @return zero(0) or negative error code.
 */
int event_clear(struct event_t *self_p, uint32_t mask);

#endif
