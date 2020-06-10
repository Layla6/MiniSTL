#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H
/*
	内存空间：一级配置器
*/

#if 0
#	include<new>
#	define _THROW_BAD_ALLOC throw
#elif !defined(_THROW_BAD_ALLOC)
#	include<iostream>
#	define _THROW_BAD_ALLOC std::cerr<<"out of memory"<<std::endl;exit(1)
#endif


namespace MiniSTL {


	class allocator {

	private:
		static void* oom_malloc(size_t);
		static void *oom_realloc(void*, size_t);
		static void (*_malloc_alloc_oom_handler)() ;
	public:
		static void* allocate(size_t n) {
			std::cout << "**********调用一级配置器：allocate***********" << std::endl;
			void* result = malloc(n);
			if (result == 0) {
				std::cout << "allocate分配内存失败，调用oom_malloc" << std::endl;
				result = oom_malloc(n);
			}
			std::cout << "成功分配内存" << std::endl;
			return result;
		}
		static void deallocate(void* p, size_t /*n*/) {
			std::cout << "**********调用一级配置器：deallocate***********" << std::endl;
			free(p);
			std::cout << "成功删除内存" << std::endl;
		}
		static void* reallocate(void* p, size_t /*old size*/, size_t new_sz) {
			void* result = realloc(p, new_sz);
			if (0 == result)
				return oom_realloc(p, new_sz);
			return result;
		}
		static void (*set_malloc_handler(void(*f)()))(){
			void (*old)() = _malloc_alloc_oom_handler;
			_malloc_alloc_oom_handler = f;
			return (old);
		}
	};


}
#endif
/*
(1)if 0 中的code中定义的是一些调试版本的代码，此时code完全被编译器忽略。如果想让code生效，只需把#if 0改成#if 1
(2)void *realloc(void *ptr, size_t size) 尝试重新调整之前调用 malloc 或 calloc 所分配的 ptr 所指向的内存块的大小。
	https://www.cnblogs.com/heyonggang/archive/2012/12/21/2827826.html
	1.ptr必须为NULL，或者为malloc，realloc或者calloc的返回值，否则发生realloc invalid pointer错误
	2.new_size如果小于old_size，只有new_size大小的数据会被保存，可能会发生数据丢失，慎重使用。
	3.如果new_size大于old_size，可能会分配一块新的内存，这时候ptr指向的内存会被释放，ptr成为野指针，再访问的时候会发生错误。
	4.最后不要将返回结果再赋值给ptr，即ptr=realloc(ptr,new_size)是不建议使用的，因为如果内存分配失败，ptr会变为NULL，
	如果之前没有将ptr所在地址赋给其他值的话，会发生无法访问旧内存空间的情况，所以建议使用temp=realloc(ptr,new_size)。
*/
