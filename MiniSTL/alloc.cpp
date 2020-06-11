#include "alloc.h"
#include "allocator.h"
#include <stdlib.h> 
namespace MiniSTL {
	char* alloc::start_free = 0;
	char* alloc::end_free = 0;
	size_t alloc::heap_size = 0;
	alloc::obj* alloc::free_list[alloc::ENFreeLists::_NFREELISTS] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	void* alloc::allocate(size_t bytes) {
		if (bytes > alloc::EMaxBytes::_MAXBYTES) //����128����һ��������
			return allocator::allocate(bytes);
		std::cout << "**********���ö�����������allocate***********" << std::endl;
		size_t index = FREE_LIST_INDEX(bytes);
		alloc::obj* suitble_list = free_list[index];
		if (suitble_list) { //��ʾ��index���������п����ڴ�
			free_list[index] = suitble_list->free_list_link;
			return suitble_list;
		}
		else {
			return refill(ROUND_UP(bytes));
		}
	}

	void alloc::deallocate(void* ptr, size_t bytes) {
		if (bytes > alloc::EMaxBytes::_MAXBYTES) //����128����һ��������
			allocator::deallocate(ptr,bytes);   //ע�����ﲻ��return����Ϊfree����ָ��
		else {
			std::cout<< std::endl;
			std::cout << "**********���ö�����������deallocate***********" << std::endl;
			size_t index = FREE_LIST_INDEX(bytes);  //����С���鵽����
			obj* node= static_cast<obj*>(ptr);
			node->free_list_link = free_list[index];
			free_list[index] = node;
			std::cout<< "�������飺" << node << std::endl;
		}
	}

	/*
	tinystlδʵ��
	*/
	void* alloc::reallocate(void* ptr, size_t old_sz, size_t new_sz) {
		deallocate(ptr, old_sz);
		ptr = allocate(new_sz);
		return ptr;	
		//void* result=allocator::reallocate(ptr, old_sz, new_sz);return result;
	}
	void alloc::print_alloc_state() {
		//errer:char *ֱ�����char�ĵ�ַ������ std::cout <<"alloc:�ڴ��start: "<< alloc::start_free<<"   end: "<< alloc::end_free<<"  size: "<<alloc::heap_size << std::endl;
		std::cout << "  size: " << alloc::heap_size << std::endl;
		if (alloc::heap_size != 0) {
			std::cout << "alloc:�ڴ��start: " << size_t(alloc::start_free) << "   end: " << size_t(alloc::end_free) << "  size: " << alloc::heap_size << std::endl;
		
		
		//�鿴freelist
		obj* ith_list = 0;
		for (int i = 0;i <= EMaxBytes::_MAXBYTES; i += EAlign::_ALIGN) {
			std::cout << "����" << i << "�ֽڵ�����" << std::endl;
			ith_list = free_list[FREE_LIST_INDEX(i)];
			while (ith_list != 0) {
				std::cout << (ith_list) << std::endl;
				ith_list = ith_list->free_list_link;
			}
			
		}
		
		}
	}
	/*
	freelist ��û��bytes��round_up����С������
	refill�����������ռ䣬�µĿռ�ȡ���ڴ��(chunk_alloc)
	Ĭ��20�������ռ䲻������С��20
	*/
	void* alloc::refill(size_t bytes) {
		size_t nobjs = ENObjs::NOBJS;
		char* chunk = chunk_alloc(bytes, nobjs);
		obj** my_free_list = 0;
		obj* result = 0;
		obj* current_obj = 0, * next_obj = 0;
		
		if (nobjs == 1)
			return chunk;
		else {
			my_free_list = free_list + FREE_LIST_INDEX(bytes);
			result = (obj*)(chunk);
			*my_free_list=next_obj= (obj*)(chunk+bytes);
			for (int i = 1;;++i) {
				current_obj = next_obj;
				next_obj= (obj*)((char* )next_obj + bytes);
				if (nobjs - 1 == i) { //ʣ����nobjs-1���飨��һ���ó������أ�ʣ�µļ���freelist��
					current_obj->free_list_link = 0;
					break;
				}
				else {
					current_obj->free_list_link = next_obj;
				}
			}
		}
		return result;
	}
	char* alloc::chunk_alloc(size_t bytes, size_t& nobjs) {
		char* result = 0;
		size_t total_bytes = bytes * nobjs;
		std::cout << bytes << " " << nobjs << std::endl;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes) {  //��ȫ��������
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else if(bytes_left>=bytes){				//���㲿������
			nobjs = bytes_left / bytes;
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {				//�ڴ����1��Ҳ�޷��ṩ
			//size_t hpz = ROUND_UP(heap_size >> 4);
			//std::cout << "2 * total_bytes :" << 2 * total_bytes << "  heapsize: " << heap_size << "  round_hpz: " << hpz << std::endl;
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			if (bytes_left > 0) {  //���ڴ�صĲ�����գ��ָ�freelist��
				obj** my_free_list = free_list + FREE_LIST_INDEX(bytes_left);
				((obj*)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj*)start_free;
			}
			//��ʹ�ڴ���Ѿ�����
			start_free = (char*)malloc(bytes_to_get);  // mallocΪ�ڴ������ռ�

			//malloc����ʧ�ܣ���Ѱ��freelist��δʹ���Ҵ�ģ������� �����ڴ��
			if (start_free==0) {	
				obj** my_free_list = 0, * p = 0;
				for (int i = 0;i <= EMaxBytes::_MAXBYTES; i += EAlign::_ALIGN) {
					my_free_list = free_list + FREE_LIST_INDEX(i);
					p = *my_free_list;
					if (p != 0) {     //��i�����п�������
						*my_free_list = p->free_list_link;
						start_free = (char*)p;
						end_free = start_free + i;
						return chunk_alloc(bytes, nobjs);
					}
				}
				end_free = 0; //freelist �Ҳ���δʹ�õ����飬��start_free��end_free��Ϊ0
				start_free = (char*)allocator::allocate(bytes_to_get); //�쳣������ߵ����Զ��崦����
			}	
			//malloc ����ɹ� ���²���
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(bytes, nobjs);
		}

	}

}