#include "allocator.h"
#include "alloc.h"
#include "construct.h"
#include <iostream>
#include <vector>
#include "vector.h"
#include "algorithm.h"
using namespace MiniSTL;
void test_allocator(){
	//size_t sz = 1000000000000000;  //test out_of_memory
	size_t sz = 10;  //test successful example
	allocator alloc_room;
	void* p = alloc_room.allocate(sz);
	alloc_room.deallocate(p, sz);
}
void test_alloc() {
	/*
	*******heap_size=free_list所有区块的和+（end_free-start_free）***********

	问题：重新申请heap时，为什么需要heap_size>>4
	size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
	*/

	//size_t sz = 1000000000000000;  //test out_of_memory
	//size_t sz = 10000;	//test successful example,大于128，转调一级配置器
	alloc alloc_room;
	alloc_room.print_alloc_state();
	/*
	（1）free_list为空，heap为空
	（2）申请10bytes,上调16bytes,内存池剩余0，因此配置对空间申请20*16到freelist,更新start_free,end_free,heap_size=640
	（3）再次申请10bytes,这次从freelist直接取
	（4）申请33，上调40bytes，freelist无40bytes，因此向内存池申请8*40（leht_size=320,所以只能配置8块）到freelist,更新start_free,end_free,heap_size
	（5）申请50bytes,上调56bytes,内存池剩余0，因此配置对空间申请20*56到freelist,更新start_free,end_free,heap_size=2920
	*/
	//test successful example,小于128
	size_t sz = 10;	
	void* p = alloc_room.allocate(sz);
	alloc_room.print_alloc_state();
	void* p2 = alloc_room.allocate(sz);
	size_t sz1 = 33;
	void* p3 = alloc_room.allocate(sz1);
	alloc_room.print_alloc_state();
	size_t sz2 = 50;
	void* p4 = alloc_room.allocate(sz2);
	alloc_room.print_alloc_state();


	alloc_room.deallocate(p, sz);
	//alloc_room.print_alloc_state();
	alloc_room.deallocate(p2, sz);
	//alloc_room.print_alloc_state();
	alloc_room.deallocate(p3, sz1);
	//alloc_room.print_alloc_state();
	alloc_room.deallocate(p4, sz2);
	alloc_room.print_alloc_state();
}
void test_construct() {
	/*
	(1)调用一级配置器分配一个sizeof(int)大小的空间（同理，二级）
	(2)此时，p所指内存并未初始化构造
	(3)调用construct构造初始化
	(4)析构，释放内存
	*/
	size_t sz = sizeof(int);  //test successful example
	allocator alloc_room;
	void* p = alloc_room.allocate(sz);
	std::cout <<p<<"  "<<*(int*)p << std::endl;
	construct((int*)p, 666);
	std::cout <<p<<"  "<<*(int*)p << std::endl;
	destroy((int*)p);
	std::cout <<p<<"  "<<*(int*)p << std::endl;
	alloc_room.deallocate(p, sz);
	
	/*
	注意 destroy(析构函数)并未free memory，需要调用 deallocate？？？
	动态内存分配（new）原生指针的析构函数不会自动释放其内存。
	*/
	void* p1 = alloc_room.allocate(sz);
	std::cout << p1 <<"  "<< *(int*)p1 << std::endl;
	alloc_room.deallocate(p1, sz);
	//alloc_room.deallocate(p, sz);
	//std::cout <<p<<*(int*)p << std::endl;
}

void test_copy() {
	
	//true:__copy_tri_assign版本
	int vec[5] = { 1,2,3,4,5 };
	int des[5] = { 6,7,8,9,10 };
	//MiniSTL::copy(vec, vec+5, des);
	MiniSTL::copy_backward(vec, vec + 5, des+5);
	for (auto i = 0;i < 5;++i)
		std::cout << des[i] << "  ";

	/*
	//该例子有错误，不能使用std::vector 因为vector封装了自己的迭代器  std::input_iterator_tag与ministl::input_iterator_tag不同
	std::vector<int> vec1 = { 1,2,3,4,5 };
	std::vector<int> des1 = { 6,7,8,9,10 };
	MiniSTL::copy(vec1.begin(), vec1.end(), des1.begin());
	for (auto i = 0;i < des1.size();++i)
		std::cout << des1[i] << "  ";
	
	*/
	//隐藏algorithm copy的//偏特化版本 测试random_iterator
	MiniSTL::vector<int> vec1(5,1);
	MiniSTL::vector<int> des1(10,2);
	//MiniSTL::copy(vec1.begin(), vec1.end(), des1.begin());
	MiniSTL::copy_backward(vec1.begin(), vec1.end(), des1.end());
	for (auto i = 0;i < des1.size();++i)
		std::cout << des1[i] << "  ";
	std::cout << std::endl;
	
	//test overlap case
	int ia[10] = { 0,1,2,3,4,5 ,6,7,8,9};
	//MiniSTL::copy_backward(ia+2, ia + 7, ia + 9);
	MiniSTL::copy_backward(ia + 2, ia + 7, ia + 5);
	for (auto i = 0;i < 10;++i)
		std::cout << ia[i] << "  ";
}

struct Compare_N
{
	const int n;
	Compare_N(const int x) :n(x) {}
	void operator () (int& i)
	{
		if (i < n)
			i = -1;
		else
			i = 1;
	}
};

void test_vector() {
	//测试构造函数
	vector<int> vec(6,1);
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;

	//测试insert_aux
	vec.insert_aux(vec.begin()+2,7);
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;

	//测试insert, 以及capacity 容量变化
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	vec.insert(vec.begin() + 1, 6, 6);
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;
	
	//测试resize
	//new_size < size()
	vec.resize(5,0);
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;
	//new_size > size()
	vec.resize(15, 4);
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;


	//测试push_back pop_back,back front
	vec.push_back(3);vec.push_back(1);vec.pop_back();
	std::cout << " back: " << vec.back() << "   front: " << vec.front() << std::endl;
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;

	//测试find,sort    注意型别声明vector<int>::pointer
	vector<int>::pointer ind=find(vec.begin(), vec.end(), 4);
	std::cout << " index: " << ind - vec.begin() << std::endl;

	for_each(vec.begin(), vec.end(), Compare_N(5));
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
}
int main() {

	/*
	//测试一级二级空间配置器
	std::cout << "test_allocator：测试一级配置器allocator " << std::endl;
	test_allocator();  
	std::cout << std::endl;std::cout << "test_allocator： 测试二级配置器alloc" << std::endl;
	test_alloc();  
	*/

	/*
	//测试构造和析构工具
	test_construct();
	*/

	/*
	//测试算法copy
	test_copy();
	*/
	
	/*
	//测试vector
	*/
	test_vector();

	return 0;
}