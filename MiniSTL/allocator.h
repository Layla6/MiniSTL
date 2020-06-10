#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H
/*
	�ڴ�ռ䣺һ��������
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
			std::cout << "**********����һ����������allocate***********" << std::endl;
			void* result = malloc(n);
			if (result == 0) {
				std::cout << "allocate�����ڴ�ʧ�ܣ�����oom_malloc" << std::endl;
				result = oom_malloc(n);
			}
			std::cout << "�ɹ������ڴ�" << std::endl;
			return result;
		}
		static void deallocate(void* p, size_t /*n*/) {
			std::cout << "**********����һ����������deallocate***********" << std::endl;
			free(p);
			std::cout << "�ɹ�ɾ���ڴ�" << std::endl;
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
(1)if 0 �е�code�ж������һЩ���԰汾�Ĵ��룬��ʱcode��ȫ�����������ԡ��������code��Ч��ֻ���#if 0�ĳ�#if 1
(2)void *realloc(void *ptr, size_t size) �������µ���֮ǰ���� malloc �� calloc ������� ptr ��ָ����ڴ��Ĵ�С��
	https://www.cnblogs.com/heyonggang/archive/2012/12/21/2827826.html
	1.ptr����ΪNULL������Ϊmalloc��realloc����calloc�ķ���ֵ��������realloc invalid pointer����
	2.new_size���С��old_size��ֻ��new_size��С�����ݻᱻ���棬���ܻᷢ�����ݶ�ʧ������ʹ�á�
	3.���new_size����old_size�����ܻ����һ���µ��ڴ棬��ʱ��ptrָ����ڴ�ᱻ�ͷţ�ptr��ΪҰָ�룬�ٷ��ʵ�ʱ��ᷢ������
	4.���Ҫ�����ؽ���ٸ�ֵ��ptr����ptr=realloc(ptr,new_size)�ǲ�����ʹ�õģ���Ϊ����ڴ����ʧ�ܣ�ptr���ΪNULL��
	���֮ǰû�н�ptr���ڵ�ַ��������ֵ�Ļ����ᷢ���޷����ʾ��ڴ�ռ����������Խ���ʹ��temp=realloc(ptr,new_size)��
*/
