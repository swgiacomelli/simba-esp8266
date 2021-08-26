// Copyright (c) 2021 Steven Giacomelli. All rights reserved.
//
// Derived from the Simba project.
// Copyright (c) 2014-2018, Erik Moqvist
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef __KERNEL_SYS_PORT_H__
#define __KERNEL_SYS_PORT_H__

#define SYS_SETTINGS_APP_BASE 0x100

static inline uint32_t htonl(uint32_t v) {
  return (((v) << 24) | (((v)&0x0000ff00) << 8) | (((v)&0x00ff0000) >> 8) |
          (((v)&0xff000000) >> 24));
}

#define ntohl(v) htonl(v)

static inline uint16_t htons(uint16_t v) {
  return (((v) << 8) | (((v)&0xff00) >> 8));
}

#define ntohs(v) htons(v)

#endif
