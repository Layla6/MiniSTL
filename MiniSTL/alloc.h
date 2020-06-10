#ifndef _ALLOC_H
#define _ALLOC_H

namespace MiniSTL {
	class alloc
	{
	private:
		enum EAlign { _ALIGN = 8 };//小型区块的上调边界
		enum EMaxBytes { _MAXBYTES = 128 };//小型区块的上限，超过的区块由malloc分配
		enum ENFreeLists { _NFREELISTS = (EMaxBytes::_MAXBYTES / EAlign::_ALIGN) };//free-lists的个数
		enum ENObjs { NOBJS = 20 };//每次增加的节点数

	private:
		union obj{
			union obj* free_list_link;
			char client_data[1];
		};
		static obj* free_list[ENFreeLists::_NFREELISTS];

	private:
		static char* start_free;//内存池起始位置
		static char* end_free;//内存池结束位置
		static size_t heap_size;

	private:
		//将bytes上调至_ALIGH的倍数
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes+EAlign::_ALIGN-1)& ~(EAlign::_ALIGN - 1));
		}
		//根据区块大小，决定使用第n号free_list,n从0开始
		static size_t FREE_LIST_INDEX(size_t bytes) {
			return (((bytes)+EAlign::_ALIGN - 1) / EAlign::_ALIGN - 1);
		}
		//返回一个大小为n的对象，并可能加入大小为n的其他区块到free-list
		static void* refill(size_t n);
		//配置一大块空间，可容纳nobjs个大小为size的区块
		//如果配置nobjs个区块有所不便，nobjs可能会降低
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
	reason1:const允许static成员在其声明式上获得初值
	reason2：不想让别人获得pointer或reference指向你的某个整数常量
*/
