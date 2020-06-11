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
	*******heap_size=free_list��������ĺ�+��end_free-start_free��***********

	���⣺��������heapʱ��Ϊʲô��Ҫheap_size>>4
	size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
	*/

	//size_t sz = 1000000000000000;  //test out_of_memory
	//size_t sz = 10000;	//test successful example,����128��ת��һ��������
	alloc alloc_room;
	alloc_room.print_alloc_state();
	/*
	��1��free_listΪ�գ�heapΪ��
	��2������10bytes,�ϵ�16bytes,�ڴ��ʣ��0��������öԿռ�����20*16��freelist,����start_free,end_free,heap_size=640
	��3���ٴ�����10bytes,��δ�freelistֱ��ȡ
	��4������33���ϵ�40bytes��freelist��40bytes��������ڴ������8*40��leht_size=320,����ֻ������8�飩��freelist,����start_free,end_free,heap_size
	��5������50bytes,�ϵ�56bytes,�ڴ��ʣ��0��������öԿռ�����20*56��freelist,����start_free,end_free,heap_size=2920
	*/
	//test successful example,С��128
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
	(1)����һ������������һ��sizeof(int)��С�Ŀռ䣨ͬ��������
	(2)��ʱ��p��ָ�ڴ沢δ��ʼ������
	(3)����construct�����ʼ��
	(4)�������ͷ��ڴ�
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
	ע�� destroy(��������)��δfree memory����Ҫ���� deallocate������
	��̬�ڴ���䣨new��ԭ��ָ����������������Զ��ͷ����ڴ档
	*/
	void* p1 = alloc_room.allocate(sz);
	std::cout << p1 <<"  "<< *(int*)p1 << std::endl;
	alloc_room.deallocate(p1, sz);
	//alloc_room.deallocate(p, sz);
	//std::cout <<p<<*(int*)p << std::endl;
}

void test_copy() {
	
	//true:__copy_tri_assign�汾
	int vec[5] = { 1,2,3,4,5 };
	int des[5] = { 6,7,8,9,10 };
	//MiniSTL::copy(vec, vec+5, des);
	MiniSTL::copy_backward(vec, vec + 5, des+5);
	for (auto i = 0;i < 5;++i)
		std::cout << des[i] << "  ";

	/*
	//�������д��󣬲���ʹ��std::vector ��Ϊvector��װ���Լ��ĵ�����  std::input_iterator_tag��ministl::input_iterator_tag��ͬ
	std::vector<int> vec1 = { 1,2,3,4,5 };
	std::vector<int> des1 = { 6,7,8,9,10 };
	MiniSTL::copy(vec1.begin(), vec1.end(), des1.begin());
	for (auto i = 0;i < des1.size();++i)
		std::cout << des1[i] << "  ";
	
	*/
	//����algorithm copy��//ƫ�ػ��汾 ����random_iterator
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
	//���Թ��캯��
	vector<int> vec(6,1);
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;

	//����insert_aux
	vec.insert_aux(vec.begin()+2,7);
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;

	//����insert, �Լ�capacity �����仯
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	vec.insert(vec.begin() + 1, 6, 6);
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;
	
	//����resize
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


	//����push_back pop_back,back front
	vec.push_back(3);vec.push_back(1);vec.pop_back();
	std::cout << " back: " << vec.back() << "   front: " << vec.front() << std::endl;
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;

	//����find,sort    ע���ͱ�����vector<int>::pointer
	vector<int>::pointer ind=find(vec.begin(), vec.end(), 4);
	std::cout << " index: " << ind - vec.begin() << std::endl;

	for_each(vec.begin(), vec.end(), Compare_N(5));
	for (auto it = vec.begin();it != vec.end();++it)
		std::cout << *it << "  ";
}
int main() {

	/*
	//����һ�������ռ�������
	std::cout << "test_allocator������һ��������allocator " << std::endl;
	test_allocator();  
	std::cout << std::endl;std::cout << "test_allocator�� ���Զ���������alloc" << std::endl;
	test_alloc();  
	*/

	/*
	//���Թ������������
	test_construct();
	*/

	/*
	//�����㷨copy
	test_copy();
	*/
	
	/*
	//����vector
	*/
	test_vector();

	return 0;
}