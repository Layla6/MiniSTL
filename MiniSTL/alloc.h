#ifndef _ALLOC_H
#define _ALLOC_H

namespace MiniSTL {
	class alloc
	{
	private:
		enum EAlign { _ALIGN = 8 };//С��������ϵ��߽�
		enum EMaxBytes { _MAXBYTES = 128 };//С����������ޣ�������������malloc����
		enum ENFreeLists { _NFREELISTS = (EMaxBytes::_MAXBYTES / EAlign::_ALIGN) };//free-lists�ĸ���
		enum ENObjs { NOBJS = 20 };//ÿ�����ӵĽڵ���

	private:
		union obj{
			union obj* free_list_link;
			char client_data[1];
		};
		static obj* free_list[ENFreeLists::_NFREELISTS];

	private:
		static char* start_free;//�ڴ����ʼλ��
		static char* end_free;//�ڴ�ؽ���λ��
		static size_t heap_size;

	private:
		//��bytes�ϵ���_ALIGH�ı���
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes+EAlign::_ALIGN-1)& ~(EAlign::_ALIGN - 1));
		}
		//���������С������ʹ�õ�n��free_list,n��0��ʼ
		static size_t FREE_LIST_INDEX(size_t bytes) {
			return (((bytes)+EAlign::_ALIGN - 1) / EAlign::_ALIGN - 1);
		}
		//����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free-list
		static void* refill(size_t n);
		//����һ���ռ䣬������nobjs����СΪsize������
		//�������nobjs�������������㣬nobjs���ܻή��
		static char* chunk_alloc(size_t size, size_t& nobjs);

	public:
		static void* allocate(size_t bytes);
		static void deallocate(void* ptr, size_t bytes);
		static void* reallocate(void* ptr, size_t old_sz, size_t new_sz);
		static void print_alloc_state();
	};
}

#endif

/*
effective c++  clause 02 prefer consts,enums,and inlines to #defines.
(1)Replace #define with const,enums
	reason1:No type checking
(2)Replace const with enums
	reason1:const����static��Ա��������ʽ�ϻ�ó�ֵ
	reason2�������ñ��˻��pointer��referenceָ�����ĳ����������
*/
