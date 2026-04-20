#include<memory/memory.h>

struct MemBlock {//内存块
	void* address;//内存地址
	uint32_t size;//内存字节大小
	int32_t use_times;//使用计数
	struct MemBlock* next;//连接内存块
};
static struct {//静态结构体内部唯一
	bool is_init;//初始化状态
	struct MemBlock* head;//头内存块
}all_mem = { false,NULL };

void mem_init(void) {//初始化内存管理器
	all_mem.is_init = true;
	all_mem.head = NULL;
}
void mem_shutdown(void) {//释放内存
	struct MemBlock* ptr, * next;
	for (ptr = all_mem.head; ptr; ptr = next) {//循环释放内存
		next = ptr->next;//移动指针
		free(ptr->address);
		ptr->address = NULL;
		free(ptr); ptr = NULL;
	}
	all_mem.head = NULL;
	all_mem.is_init = false;
}
static void mem_track(struct MemBlock* ptr) {//内部静态函数/头插法注册函数并接续在内存管理器上(不做验空/必须这一步不崩溃防止后续泄露)
	ptr->next = all_mem.head;
	all_mem.head = ptr;
}
static void mem_untrack(struct MemBlock* ptr) {//静态函数/将释放内存块从内存管理器上取消
	struct MemBlock** prev = &(all_mem.head);
	while (*prev) {
		if (*prev == ptr) {
			*prev = ptr->next;
			break;
		}
		prev = &(*prev)->next;
	}
}
void* mem_alloc(uint32_t counts, uint32_t elem_size) {//申请内存
	if (!all_mem.is_init)return NULL;//未初始化
	if (!counts || !elem_size)return NULL;//数据不合法
	uint32_t total = counts * elem_size;//总大小字节
	struct MemBlock* ptr = malloc(sizeof(struct MemBlock));
	if (!ptr) { return NULL; }
	ptr->address = malloc(total);
	if (!ptr->address) {//依次释放
		free(ptr); ptr = NULL;
		return NULL;
	}
	memset(ptr->address, 0, total);
	ptr->size = total;
	ptr->use_times = 1;//增加引用计数（后续增加不在此处）
	mem_track(ptr);
	return ptr;
}
void* mem_data(void* h) {//返回数据指针
	return h ? ((mhandle)(h))->address : NULL;
}
void mem_retain(void* h) {//复用增加引用计数
	if (h)((mhandle)(h))->use_times++;
}
void mem_release(void* h) {//释放内存
	if (!h)return;
	if (((mhandle)h)->use_times - 1 <= 0) {//引用次数为0则直接释放
		mem_untrack(((mhandle)h));
		free(((mhandle)h)->address);
		((mhandle)h)->address = NULL; free(h);
		h = NULL;
	}
}