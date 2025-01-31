/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KEYBOARD_DEF_H
#define KEYBOARD_DEF_H
#if defined (__ARMCC_VERSION) /* ARM Compiler */
  #ifndef __WEAK
    #define __WEAK  __attribute__((weak))
  #endif
  #ifndef __packed
    #define __packed  __attribute__((packed))
  #endif
#elif defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
  #ifndef __WEAK
    #define __WEAK   __attribute__((weak))
  #endif /* __WEAK */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */

#if !defined(UNUSED)
#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#endif /* UNUSED */

#ifndef BIT
#define BIT(x)  (1UL << (x))
#endif

#endif //KEYBOARD_DEF_H
