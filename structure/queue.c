//包含数据结构的头文件（声明Queue结构体和函数原型）
#include "structure.h"



Queue* queue_create(int capacity) {
    //合法性检查：容量必须大于0，否则无法创建队列
    if (capacity <= 0)
        return NULL;
    //分配队列结构体本身的内存
    Queue* q = (Queue*)malloc(sizeof(Queue));
    //分配存储元素的内存：元素是 void* 类型（可以存任意类型指针）
    q->data = (void**)malloc(sizeof(void*) * capacity);
    //初始化队头、队尾下标
    //循环队列：front == rear 时表示队列为空
    q->front = 0;
    q->rear = 0;
    // 4. 记录队列最大容量
    q->capacity = capacity;

    //返回创建好的队列
    return q;
}
void queue_push(Queue* q, void* element) {
    //安全性检查：队列不存在 或 队列已满 → 不操作
    if (!q || queue_is_full(q))
        return;
    //把新元素放到当前队尾的位置
    q->data[q->rear] = element;

    //队尾下标向后移动一位
    //取余 % 实现循环：到最后一个位置后自动回到 0
    q->rear = (q->rear + 1) % q->capacity;
}
void* queue_pop(Queue* q) {
    //安全性检查：队列不存在 或 队列为空 → 无法出队
    if (!q || queue_is_empty(q))
        return NULL;
    //先保存队头元素（要返回给用户）
    void* temp = q->data[q->front];
    //队头下标向后移动一位
    //取余实现循环
    q->front = (q->front + 1) % q->capacity;
    //返回出队元素
    return temp;
}
void* queue_front(Queue* q) {
    //队列不存在 或 为空 → 返回 NULL
    if (!q || queue_is_empty(q))
        return NULL;

    //直接返回队头位置的元素
    return q->data[q->front];
}
int queue_is_empty(Queue* q) {
    //队列指针无效 或者 队头 == 队尾 → 空队列
    return !q || q->front == q->rear;
}
int queue_is_full(Queue* q) {
    //队列无效 → 视为满
    if (!q)
        return 1;
    //循环队列经典判满公式
    return (q->rear + 1) % q->capacity == q->front;
}
void queue_clear(Queue* q) {
    //队列存在才清空
    if (q) {
        //重置队头、队尾下标 → 队列逻辑变空
        q->front = 0;
        q->rear = 0;
    }
}
void queue_destroy(Queue* q) {
    if (q) {
        free(q->data);
        free(q);
    }
}