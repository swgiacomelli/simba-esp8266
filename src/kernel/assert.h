// Copyright (c) 2021 Steven Giacomelli. All rights reserved.
//
// Derived from the Simba project.
// Copyright (c) 2014-2018, Erik Moqvist
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef __KERNEL_ASSERT_H__
#define __KERNEL_ASSERT_H__

/**
 * Make an assert fatal by negating the error code.
 */
#define FATAL(n) (-n)

/**
 * Check is an error code is fatal (negative error code).
 */
#if CONFIG_ASSERT_FORCE_FATAL == 1
#define IS_FATAL(n) (1)
#else
#define IS_FATAL(n) (n < 0)
#endif

/**
 * Assert given condition and print an error message on assertion
 * failure. Call the system on fatal callback with error code
 * ``EASSERT`` on fatal error, otherwise return NULL.
 */
#define ASSERT(cond, ...) ASSERTNR(cond, EASSERT, FATAL(EASSERT), __VA_ARGS__)

/**
 * Assert given condition and print an error message on assertion
 * failure. Call the system on fatal callback with given error code
 * ``n`` on fatal error, otherwise return the error code negated.
 */
#define ASSERTN(cond, n, ...) ASSERTNR(cond, n, FATAL(n), __VA_ARGS__)

/**
 * Assert given condition and print an error message on assertion
 * failure. Call the system on fatal callback with error code
 * ``EASSERT`` on fatal error, otherwise return NULL.
 */
#define ASSERTRV(cond, ...) ASSERTNRV(cond, EASSERT, __VA_ARGS__)

/**
 * Assert given condition and print an error message on assertion
 * failure. Call the system on fatal callback with error code
 * ``EASSERT`` on fatal error, otherwise return NULL.
 */
#define ASSERTRN(cond, ...) ASSERTNR(cond, EASSERT, NULL, __VA_ARGS__)

/**
 * Assert given condition and print an error message. Call the system
 * on fatal callback with given error code ``n`` on fatal error,
 * otherwise return given error code ``res``.
 */
#if CONFIG_ASSERT == 1
#if CONFIG_ASSERT_FORCE_PANIC == 1
#define ASSERTNR(cond, n, res, ...)                                \
  if (!(cond)) {                                                   \
    sys_panic(OSTR(__FILE__ ":" STRINGIFY(                         \
        __LINE__) ": ASSERT: (" #cond ", " #n ") " #__VA_ARGS__)); \
  }
#else
#define ASSERTNR(cond, n, res, ...)                                       \
  if (!(cond)) {                                                          \
    std_printf(OSTR(__FILE__ ":" STRINGIFY(                               \
        __LINE__) ": ASSERT: (" #cond ", " #n ") " #__VA_ARGS__ "\r\n")); \
                                                                          \
    if (IS_FATAL(n)) {                                                    \
      sys.on_fatal_callback(n);                                           \
    } else {                                                              \
      return (res);                                                       \
    }                                                                     \
  }
#endif
#else
#define ASSERTNR(cond, n, ...)
#endif

/**
 * Assert given condition and print an error message on assertion
 * failure. Call the system on fatal callback with given error code
 * ``n`` on fatal error, otherwise return.
 */
#if CONFIG_ASSERT == 1
#if CONFIG_ASSERT_FORCE_PANIC == 1
#define ASSERTNRV(cond, n, ...)                                    \
  if (!(cond)) {                                                   \
    sys_panic(OSTR(__FILE__ ":" STRINGIFY(                         \
        __LINE__) ": ASSERT: (" #cond ", " #n ") " #__VA_ARGS__)); \
  }
#else
#define ASSERTNRV(cond, n, ...)                                           \
  if (!(cond)) {                                                          \
    std_printf(OSTR(__FILE__ ":" STRINGIFY(                               \
        __LINE__) ": ASSERT: (" #cond ", " #n ") " #__VA_ARGS__ "\r\n")); \
                                                                          \
    if (IS_FATAL(n)) {                                                    \
      sys.on_fatal_callback(n);                                           \
    } else {                                                              \
      return;                                                             \
    }                                                                     \
  }
#endif
#else
#define ASSERTNRV(cond, n, ...)
#endif

/**
 * Assert given condition and print an error message on assertion
 * failure. Call the system on fatal callback with given error code
 * ``n`` on fatal error, otherwise return NULL.
 */
#define ASSERTNRN(cond, n, ...) ASSERTNR(cond, n, NULL, __VA_ARGS__)

/**
 * Assert given condition and print an error message on assertion
 * failure, then call the system on fatal callback with given error
 * code ``n``.
 *
 * This assertion is not affected by ``CONFIG_ASSERT``, but instead
 * ``CONFIG_FATAL_ASSERT``.
 */
#if CONFIG_FATAL_ASSERT == 1
#define FATAL_ASSERTN(cond, n, ...)                                       \
  if (!(cond)) {                                                          \
    std_fprintf(                                                          \
        console_get_output_channel(),                                     \
        FSTR(__FILE__ ":" STRINGIFY(__LINE__) ": ASSERT: (" #cond ", " #n \
                                              ") " #__VA_ARGS__ "\r\n")); \
    sys.on_fatal_callback(n);                                             \
  }
#else
#define FATAL_ASSERTN(cond, n, ...)
#endif

/**
 * Assert given condition and print an error message on assertion
 * failure, then call the system on fatal callback with error code
 * ``EASSERT``.
 *
 * This assertion is not affected by ``CONFIG_ASSERT``, but instead
 * ``CONFIG_FATAL_ASSERT``.
 */
#define FATAL_ASSERT(cond, ...) FATAL_ASSERTN(cond, EASSERT, __VA_ARGS__)

/**
 * Assert given condition and call `sys_panic()` with given error code
 * ``n`` on assertion failure.
 *
 * This assertion is not affected by ``CONFIG_ASSERT``, but instead
 * ``CONFIG_PANIC_ASSERT``.
 */
#if CONFIG_PANIC_ASSERT == 1
#if CONFIG_PANIC_ASSERT_FILE_LINE == 1
#define PANIC_ASSERTN(cond, n, ...)                        \
  if (!(cond)) {                                           \
    sys_panic(FSTR(#n ":" __FILE__ ":" STRINGIFY(          \
        __LINE__) ": ASSERT: (" #cond ") " #__VA_ARGS__)); \
  }
#else
#define PANIC_ASSERTN(cond, n, ...)                            \
  if (!(cond)) {                                               \
    sys_panic(FSTR(#n ": ASSERT: (" #cond ") " #__VA_ARGS__)); \
  }
#endif
#else
#define PANIC_ASSERTN(cond, n, ...)
#endif

/**
 * Assert given condition and call `sys_panic()` with error code
 * ``EASSERT``.
 *
 * This assertion is not affected by ``CONFIG_ASSERT``, but instead
 * ``CONFIG_PANIC_ASSERT``.
 */
#define PANIC_ASSERT(cond, ...) PANIC_ASSERTN(cond, EASSERT, __VA_ARGS__)

#endif
