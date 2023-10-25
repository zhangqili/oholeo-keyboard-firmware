/*
 * lefl_macro.h
 *
 */

#ifndef LEFL_MACRO_H_
#define LEFL_MACRO_H_

#define LAMBDA(return_type, function_body) \
({ \
      return_type $this function_body \
          $this; \
})

#define ROUND(x) ((int)((x)+0.5f))

#define BIT(x) (1<<x)

#define VAR_LOOP_INCREMENT(x,min,max,delta) if((x)<(max)) (x)+=(delta);else (x)=(min);

#define VAR_LOOP_DECREMENT(x,min,max,delta) if((x)>(min)) (x)-=(delta);else (x)=(max);

#define CONVERGE_TO_ROUNDED(x,y,m) (x)=(x)+(m)*((y)+0.5f-(x))

#define CONVERGE_TO(x,y,m) (x)=(x)+(m)*((y)-(x))

#endif /* LEFL_MACRO_H_ */
