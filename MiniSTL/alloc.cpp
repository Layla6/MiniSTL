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
		if (bytes > alloc::EMaxBytes::_MAXBYTES) //大于128调用一级配置器
			return allocator::allocate(bytes);
		std::cout << "**********调用二级配置器：allocate***********" << std::endl;
		size_t index = FREE_LIST_INDEX(bytes);
		alloc::obj* suitble_list = free_list[index];
		if (suitble_list) { //表示第index个链表中有空余内存
			free_list[index] = suitble_list->free_list_link;
			return suitble_list;
		}
		else {
			return refill(ROUND_UP(bytes));
		}
	}

	void alloc::deallocate(void* ptr, size_t bytes) {
		if (bytes > alloc::EMaxBytes::_MAXBYTES) //大于128调用一级配置器
			allocator::deallocate(ptr,bytes);   //注意这里不能return，因为free返回指针
		else {
			std::cout<< std::endl;
			std::cout << "**********调用二级配置器：deallocate***********" << std::endl;
			size_t index = FREE_LIST_INDEX(bytes);  //回收小区块到链表
			obj* node= static_cast<obj*>(ptr);
			node->free_list_link = free_list[index];
			free_list[index] = node;
			std::cout<< "回收区块：" << node << std::endl;
		}
	}

	/*
	tinystl未实现
	*/
	void* alloc::reallocate(void* ptr, size_t old_sz, size_t new_sz) {
		deallocate(ptr, old_sz);
		ptr = allocate(new_sz);
		return ptr;	
		//void* result=allocator::reallocate(ptr, old_sz, new_sz);return result;
	}
	void alloc::print_alloc_state() {
		//errer:char *直接输出char的地址，乱码 std::cout <<"alloc:内存池start: "<< alloc::start_free<<"   end: "<< alloc::end_free<<"  size: "<<alloc::heap_size << std::endl;
		std::cout << "  size: " << alloc::heap_size << std::endl;
		if (alloc::heap_size != 0) {
			std::cout << "alloc:内存池start: " << size_t(alloc::start_free) << "   end: " << size_t(alloc::end_free) << "  size: " << alloc::heap_size << std::endl;
		
		
		//查看freelist
		obj* ith_list = 0;
		for (int i = 0;i <= EMaxBytes::_MAXBYTES; i += EAlign::_ALIGN) {
			std::cout << "访问" << i << "字节的链表" << std::endl;
			ith_list = free_list[FREE_LIST_INDEX(i)];
			while (ith_list != 0) {
				std::cout << (ith_list) << std::endl;
				ith_list = ith_list->free_list_link;
			}
			
		}
		
		}
	}
	/*
	freelist 中没有bytes（round_up）大小的区块
	refill负责重新填充空间，新的空间取自内存池(chunk_alloc)
	默认20个，若空间不够，则小于20
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
				if (nobjs - 1 == i) { //剩余了nobjs-1个块（第一个拿出来返回，剩下的加入freelist）
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

		if (bytes_left >= total_bytes) {  //完全满足需求
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else if(bytes_left>=bytes){				//满足部分需求
			nobjs = bytes_left / bytes;
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {				//内存池连1块也无法提供
			//size_t hpz = ROUND_UP(heap_size >> 4);
			//std::cout << "2 * total_bytes :" << 2 * total_bytes << "  heapsize: " << heap_size << "  round_hpz: " << hpz << std::endl;
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			if (bytes_left > 0) {  //将内存池的残余清空（分给freelist）
				obj** my_free_list = free_list + FREE_LIST_INDEX(bytes_left);
				((obj*)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj*)start_free;
			}
			//此使内存池已经空了
			start_free = (char*)malloc(bytes_to_get);  // malloc为内存池申请空间

			//malloc分配失败，再寻找freelist（未使用且大的）的区块 放入内存池
			if (start_free==0) {	
				obj** my_free_list = 0, * p = 0;
				for (int i = 0;i <= EMaxBytes::_MAXBYTES; i += EAlign::_ALIGN) {
					my_free_list = free_list + FREE_LIST_INDEX(i);
					p = *my_free_list;
					if (p != 0) {     //第i区块有空余区块
						*my_free_list = p->free_list_link;
						start_free = (char*)p;
						end_free = start_free + i;
						return chunk_alloc(bytes, nobjs);
					}
				}
				end_free = 0; //freelist 找不到未使用的区块，则start_free和end_free都为0
				start_free = (char*)allocator::allocate(bytes_to_get); //异常处理或者调用自定义处理函数
			}	
			//malloc 分配成功 更新参数
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(bytes, nobjs);
		}

	}

}