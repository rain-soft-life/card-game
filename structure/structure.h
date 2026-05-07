#ifndef STRUCTURE_H
#define STRUCTURE_H
#include"baseheadfiles/basehead.h"

// 栈结构（通用，可存任意指针/暂时无实际用处先用无类型指针替代）
typedef struct Stack {
    void** data;
    int top;
    int capacity;
} Stack;

Stack* stack_create(int capacity); //创建栈
void stack_push(Stack* s, void* element); //入栈
void* stack_pop(Stack* s);//出栈
void* stack_top(Stack* s);//取栈顶
int stack_is_empty(Stack* s);// 否空
int stack_is_full(Stack* s);//是否满
void stack_clear(Stack* s); //清空栈
void stack_destroy(Stack* s); //销毁栈


//队列结构（通用可存任意指针）

typedef struct Queue {
    void** data;
    int front;
    int rear;
    int capacity;
} Queue;

Queue* queue_create(int capacity);//创建队列
void queue_push(Queue* q, void* element);//入队
void* queue_pop(Queue* q);//出队
void* queue_front(Queue* q);//取队头
int queue_is_empty(Queue* q);//是否空
int queue_is_full(Queue* q);//是否满
void queue_clear(Queue* q);//清空队列
void queue_destroy(Queue* q);//销毁队列

#endif