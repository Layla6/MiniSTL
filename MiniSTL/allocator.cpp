#include "allocator.h"
namespace MiniSTL {
	/*
	��ʼ�� oom_malloc����ʧ��ʱ���õĴ�����
	*/
	void (*allocator::_malloc_alloc_oom_handler)() = 0;
	
	/*
	malloc����ʧ��ʱ������
	*/
	void* allocator::oom_malloc(size_t n) {
		void (*my_malloc_handler)();
		void* result;

		for (;;) {
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (0 == my_malloc_handler) {
				std::cout << "oom_mallocʹ��Ĭ��_malloc_alloc_oom_handler�����������Ϣ���Ƴ�" << std::endl;
				_THROW_BAD_ALLOC; 
			}
			(*my_malloc_handler)();
			result = malloc(n);
			if (result) return result;
		}
	}

	/*
	realloc����ʧ��ʱ������
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