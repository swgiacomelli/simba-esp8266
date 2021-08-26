// Copyright (c) 2021 Steven Giacomelli. All rights reserved.
//
// Derived from the Simba project.
// Copyright (c) 2014-2018, Erik Moqvist
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "esp_misc.h"
#include "esp_system.h"

static void time_port_busy_wait_us(int microseconds) {
  os_delay_us(microseconds);
}

static int time_port_micros(void) { return (system_get_time()); }

static int time_port_micros_maximum(void) { return (-ENOSYS); }

static int time_port_micros_resolution(void) { return (-ENOSYS); }
