#include "allocator.h"
namespace MiniSTL {
	/*
	初始化 oom_malloc操作失败时调用的处理函数
	*/
	void (*allocator::_malloc_alloc_oom_handler)() = 0;
	
	/*
	malloc操作失败时，调用
	*/
	void* allocator::oom_malloc(size_t n) {
		void (*my_malloc_handler)();
		void* result;

		for (;;) {
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (0 == my_malloc_handler) {
				std::cout << "oom_malloc使用默认_malloc_alloc_oom_handler，输出错误信息并推出" << std::endl;
				_THROW_BAD_ALLOC; 
			}
			(*my_malloc_handler)();
			result = malloc(n);
			if (result) return result;
		}
	}

	/*
	realloc操作失败时，调用
	*/
	void* allocator::oom_realloc(void* p, size_t n) {
		void (*my_malloc_handler)();
		void* result;

		for (;;) {
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (0 == my_malloc_handler) { _THROW_BAD_ALLOC; }
			(*my_malloc_handler)();
			result = realloc(p,n);
			if (result) return result;
		}
	}
}