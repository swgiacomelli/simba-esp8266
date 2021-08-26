// Copyright (c) 2021 Steven Giacomelli. All rights reserved.
//
// Derived from the Simba project.
// Copyright (c) 2014-2018, Erik Moqvist
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef __KERNEL_TYPES_H__
#define __KERNEL_TYPES_H__

/**
 * Ignore unused function argument.
 *
 * An example of a function that does not use it's first argument
 * ``a``:
 *
 * @rst
 * .. code-block:: c
 *
 *    int foo(int a, int b)
 *    {
 *        UNUSED(a);
 *
 *        return (b);
 *    }
 * @endrst
 */
#define UNUSED(v) (void)(v)

/**
 * Create a string of an identifier using the pre-processor.
 */
#define STRINGIFY(x) STRINGIFY2(x)

/**
 * Used internally by `STRINGIFY()`.
 */
#define STRINGIFY2(x) #x

/**
 * Concatenate two tokens.
 */
#define TOKENPASTE(x, y) TOKENPASTE2(x, y)

/**
 * Used internally by `TOKENPASTE()`.
 */
#define TOKENPASTE2(x, y) x##y

/**
 * Create a unique token.
 */
#define UNIQUE(x) TOKENPASTE(x, TOKENPASTE(___, __LINE__))

/**
 * Debug print of file and line.
 */
#define PRINT_FILE_LINE() std_printf(OSTR("%s:%d:\r\n"), __FILE__, __LINE__);

#if CONFIG_DEBUG == 1
#define STD_PRINTF_DEBUG(...) std_printf(__VA_ARGS__)
#else
#define STD_PRINTF_DEBUG(...)
#endif

/**
 * Get the number of elements in an array.
 *
 * As an example, the code below outputs ``number of members in foo =
 * 10``.
 *
 * @rst
 * .. code-block:: c
 *
 *    int foo[10];
 *
 *    std_printf(FSTR("number of members in foo = %d\\r\\n"),
 *               membersof(foo));
 * @endrst
 */
#define membersof(a) ((ssize_t)(sizeof(a) / sizeof((a)[0])))

/**
 * Get the index of given element in an array.
 */
#define indexof(e_p, a) (e_p - &(a)[0])

#define container_of(ptr, type, member)                \
  ({                                                   \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); \
  })

/**
 * Integer division that rounds the result up.
 */
#define DIV_CEIL(n, d) (((n) + (d)-1) / (d))

/**
 * Integer division that rounds the result to the closest integer.
 */
#define DIV_ROUND(n, d) (((n) + ((d) / 2)) / (d))

/**
 * Get the minimum value of the two.
 */
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/**
 * Get the maximum value of the two.
 */
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef BIT
#define BIT(pos) (1 << (pos))
#endif

#define BITFIELD_SET(name, value) (((value) << name##_POS) & name##_MASK)

#define BITFIELD_GET(name, value) (((value)&name##_MASK) >> name##_POS)

#if defined(SIMBAPP)
#define OSTR(string) __simbapp_fmtstr_begin__ string __simbapp_fmtstr_end__
#define CSTR(string) __simbapp_cmdstr_begin__ string __simbapp_cmdstr_end__
#else
#define OSTR(string) FSTR(string)
#define CSTR(string) FSTR(string)
#endif

typedef uint8_t u8_t;
typedef int8_t s8_t;
typedef uint16_t u16_t;
typedef int16_t s16_t;
typedef uint32_t u32_t;
typedef int32_t s32_t;

struct thrd_prio_list_elem_t {
  struct thrd_prio_list_elem_t *next_p;
  struct thrd_t *thrd_p;
};

struct thrd_prio_list_t {
  struct thrd_prio_list_elem_t *head_p;
};

/**
 * Input-output vector.
 */
struct iov_t {
  void *buf_p;
  size_t size;
};

/**
 * Input-output vector with address.
 */
struct iov_uintptr_t {
  uintptr_t address;
  size_t size;
};

static inline size_t iov_uintptr_size(struct iov_uintptr_t *iov_p,
                                      size_t length) {
  size_t i;
  size_t size;

  size = 0;

  for (i = 0; i < length; i++) {
    size += iov_p[i].size;
  }

  return (size);
}

#endif
