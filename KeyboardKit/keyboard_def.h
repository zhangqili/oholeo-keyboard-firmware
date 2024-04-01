//
// Created by xq123 on 24-3-17.
//

#ifndef KEYBOARD_DEF_H
#define KEYBOARD_DEF_H
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
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

#define KEY_RAPPY_SNAPPY_BIND(a,b) ((a)->key.report_state=(a)->value>(b)->value&&(a)->value>(a)->upper_deadzone);\
                                   ((b)->key.report_state=!((a)->value>(b)->value)&&(b)->value>(b)->upper_deadzone); 

#endif //KEYBOARD_DEF_H
