/*
 * lefl_datastruct.c
 */
#include "lefl.h"

void lefl_loop_array_init(lefl_loop_array_t *arr,lefl_array_elm_t *data,uint16_t len)
{
    arr->data = data;
    arr->len = len;
    arr->index = 0;
}

lefl_array_elm_t lefl_loop_array_get(lefl_loop_array_t *arr,int16_t j)
{
    if(j>=0&&j<arr->len)
    {
        return arr->data[arr->index-j>=0?arr->index-j:arr->index-j+arr->len];
    }
    else
    {
        return 0;
    }
}

void lefl_loop_array_push_back(lefl_loop_array_t *arr,lefl_array_elm_t t)
{
    arr->index++;
    if(arr->index>=arr->len)
    {
        arr->index=0;
    }
    arr->data[arr->index]=t;
}

lefl_array_elm_t lefl_loop_array_max(lefl_loop_array_t *arr)
{
    lefl_array_elm_t max = 0;
    for(uint8_t i=0;i<arr->len;i++)
    {
        if(arr->data[i]>max)
            max=arr->data[i];
    }
    return max;
}

void lefl_bit_array_init(lefl_bit_array_t* arr, lefl_bit_array_unit_t *data, uint16_t len)
{
    arr->data = data;
    arr->len = len;
}

void lefl_bit_array_set(lefl_bit_array_t* arr, int16_t n,bool b)
{
    if(n>=0&&n<arr->len)
    {
        arr->data[n/LEFL_BIT_ARRAY_UNIT_WIDTH]&=
            (~(1<<(n%LEFL_BIT_ARRAY_UNIT_WIDTH)));
        arr->data[n/LEFL_BIT_ARRAY_UNIT_WIDTH]|=
            (b<<(n%LEFL_BIT_ARRAY_UNIT_WIDTH));
    }
}

void lefl_bit_array_set_or(lefl_bit_array_t* arr, int16_t n,bool b)
{
    if(n>=0&&n<arr->len)
    {
        arr->data[n/LEFL_BIT_ARRAY_UNIT_WIDTH]|=
            (b<<(n%LEFL_BIT_ARRAY_UNIT_WIDTH));
    }
}

bool lefl_bit_array_get(lefl_bit_array_t* arr, int16_t n)
{
    if(n>=0&&n<arr->len)
        return 1&
            (arr->data[n/LEFL_BIT_ARRAY_UNIT_WIDTH]>>(n%LEFL_BIT_ARRAY_UNIT_WIDTH));
    else
        return false;
}

void lefl_bit_array_shift(lefl_bit_array_t* arr, int16_t n)
{
    if(n>0)
    {
        for(int16_t i=(arr->len-1)/LEFL_BIT_ARRAY_UNIT_WIDTH;i>0;i--)
        {
            arr->data[i]<<=n;
            arr->data[i]|=(arr->data[i-1]>>(LEFL_BIT_ARRAY_UNIT_WIDTH-n));
        }
        arr->data[0]<<=n;
    }
    if(n<0)
    {
        for(int16_t i=0;i<(arr->len-1)/LEFL_BIT_ARRAY_UNIT_WIDTH;i++)
        {
            arr->data[i]>>=(-n);
            arr->data[i]|=arr->data[i+1]<<(LEFL_BIT_ARRAY_UNIT_WIDTH-(-n));
        }
        arr->data[(arr->len-1)/LEFL_BIT_ARRAY_UNIT_WIDTH]>>=(-n);
    }
}

void lefl_bit_array_copy(lefl_bit_array_t* to, int16_t x,lefl_bit_array_t* from, int16_t y, int16_t len)
{
    for (int16_t i = 0; i < len; i++)
    {
        lefl_bit_array_set(to, x+i, lefl_bit_array_get(from, y+i));
    }
}

lefl_stack_elm_t lefl_stack_get(lefl_stack_t* stack, lefl_stack_elm_t j)
{
    if(j>0 && j < stack->top)
        return stack->data[j];
    else
        return 0;
}
void lefl_stack_push(lefl_stack_t* stack, lefl_stack_elm_t t)
{
    if(stack->top+1<stack->len)
    {
        stack->data[stack->top]=t;
        stack->top++;
    }
}
lefl_stack_elm_t lefl_stack_pop(lefl_stack_t* stack, lefl_stack_elm_t *t)
{
    if(stack->top>0)
    {
        stack->top--;
        *t = stack->data[stack->top];
    }
    return *t;
}

void lefl_loop_queue_init(lefl_loop_queue_t* q, lefl_loop_queue_elm_t*data, uint16_t len)
{
    q->data = data;
    q->front = 0;
    q->rear = 0;
    q->len = len;
}

lefl_loop_queue_elm_t lefl_loop_queue_dequeue(lefl_loop_queue_t* q)
{
    if(q->front==q->rear)
        return 0.0;
    q->front = (q->front+1)%(q->len);
    return q->data[(q->front+q->len-1)%(q->len)];
}

void lefl_loop_queue_enqueue(lefl_loop_queue_t* q, lefl_loop_queue_elm_t t)
{
    if(((q->rear+1)%(q->len))==q->front)
        return;
    q->data[q->rear]=t;
    q->rear = (q->rear+1)%(q->len);
}
