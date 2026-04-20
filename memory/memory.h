#pragma once
#include<baseheadfiles/basehead.h>

typedef struct MemBlock* mhandle;//内部隐藏内存权限句柄

void mem_init(void);//全局初始化内存管理器
void mem_shutdown(void);//释放/关闭内存管理器
void* mem_alloc(uint32_t counts, uint32_t elem_size);//申请内存
void* mem_data(void* h);//获取数据指针
void mem_retain(void* h);//引用计数
void mem_release(void* h);//释放内存