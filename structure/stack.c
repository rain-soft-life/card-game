#include<structure/structure.h>
#include "structure.h"



//创建栈
Stack* stack_create(int capacity) {
    //容量不合法直接返回空
    if (capacity <= 0) return NULL;
    //分配栈结构体自身内存
    Stack* s = (Stack*)malloc(sizeof(Stack));
    //分配存储元素的指针数组内存
    s->data = (void**)malloc(sizeof(void*) * capacity);
    //栈顶初始为 -1，表示空栈
    s->top = -1;
    //保存最大容量
    s->capacity = capacity;
    return s;
}
//入栈
void stack_push(Stack* s, void* element) {
    if (!s || stack_is_full(s)) return;
    s->data[++(s->top)] = element;
}
//出栈
void* stack_pop(Stack* s) {
    //栈不存在或为空则无法出栈
    if (!s || stack_is_empty(s)) return NULL;
    //返回当前栈顶元素，然后栈顶自减
    return s->data[(s->top)--];
}
//获取栈顶
void* stack_top(Stack* s) {
    if (!s || stack_is_empty(s)) return NULL;
    return s->data[s->top];
}
//判断空
int stack_is_empty(Stack* s) {
    return !s || s->top == -1;
}
//判断满
int stack_is_full(Stack* s) {
    return !s || s->top == s->capacity - 1;
}
//清空
void stack_clear(Stack* s) {
    if (s) s->top = -1;
}
//销毁
void stack_destroy(Stack* s) {
    if (s) {
        free(s->data);
        free(s);
    }
}