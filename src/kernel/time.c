// Copyright (c) 2021 Steven Giacomelli. All rights reserved.
//
// Derived from the Simba project.
// Copyright (c) 2014-2018, Erik Moqvist
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "simba.h"
#include "time_port.i"

struct module_t {
  struct time_t uptime_offset;
};

static struct module_t module;

static void adjust_result(struct time_t *res_p) {
  /* abs(nanoseconds) must be less than 1000000000. */
  if (res_p->nanoseconds < -999999999L) {
    res_p->seconds--;
    res_p->nanoseconds += 1000000000L;
  } else if (res_p->nanoseconds > 999999999L) {
    res_p->seconds++;
    res_p->nanoseconds -= 1000000000L;
  }

  /* `seconds` and `nanoseconds` must have the same sign. */
  if ((res_p->seconds > 0) && (res_p->nanoseconds < 0)) {
    res_p->seconds--;
    res_p->nanoseconds += 1000000000L;
  } else if ((res_p->seconds < 0) && (res_p->nanoseconds > 0)) {
    res_p->seconds++;
    res_p->nanoseconds -= 1000000000L;
  }
}

int time_get(struct time_t *now_p) {
  ASSERTN(now_p != NULL, EINVAL);

  if (sys_uptime(now_p) != 0) {
    return (-1);
  }

  return (time_add(now_p, now_p, &module.uptime_offset));
}

int time_set(struct time_t *new_p) {
  ASSERTN(new_p != NULL, EINVAL);

  struct time_t uptime;

  if (sys_uptime(&uptime) != 0) {
    return (-1);
  }

  return (time_subtract(&module.uptime_offset, new_p, &uptime));
}

int time_add(struct time_t *res_p, struct time_t *left_p,
             struct time_t *right_p) {
  ASSERTN(res_p != NULL, EINVAL);
  ASSERTN(left_p != NULL, EINVAL);
  ASSERTN(right_p != NULL, EINVAL);

  res_p->seconds = (left_p->seconds + right_p->seconds);
  res_p->nanoseconds = (left_p->nanoseconds + right_p->nanoseconds);

  adjust_result(res_p);

  return (0);
}

int time_subtract(struct time_t *res_p, struct time_t *left_p,
                  struct time_t *right_p) {
  ASSERTN(res_p != NULL, EINVAL);
  ASSERTN(left_p != NULL, EINVAL);
  ASSERTN(right_p != NULL, EINVAL);

  res_p->seconds = (left_p->seconds - right_p->seconds);
  res_p->nanoseconds = (left_p->nanoseconds - right_p->nanoseconds);

  adjust_result(res_p);

  return (0);
}

enum time_compare_t time_compare(struct time_t *left_p,
                                 struct time_t *right_p) {
  if ((left_p->seconds == right_p->seconds) &&
      (left_p->nanoseconds == right_p->nanoseconds)) {
    return (time_compare_equal_t);
  } else if ((left_p->seconds < right_p->seconds) ||
             ((left_p->seconds == right_p->seconds) &&
              (left_p->nanoseconds < right_p->nanoseconds))) {
    return (time_compare_less_than_t);
  } else {
    return (time_compare_greater_than_t);
  }
}

#if CONFIG_TIME_UNIX_TIME_TO_DATE == 1
#include "time/unix_time_to_date.i"
#endif

void time_busy_wait_us(int microseconds) {
  time_port_busy_wait_us(microseconds);
}

int time_micros() { return (time_port_micros()); }

int time_micros_elapsed(int start, int stop) {
  if (stop >= start) {
    return (stop - start);
  } else {
    return (time_port_micros_maximum() - (start - stop));
  }
}

int time_micros_resolution() { return (time_port_micros_resolution()); }

int time_micros_maximum() { return (time_port_micros_maximum()); }
