#include "allocator.h"
#include "alloc.h"
#include "construct.h"
#include <iostream>
#include <vector>
#include "vector.h"
#include "list.h"
#include "algorithm.h"
#include "deque.h"
#include "stack.h"
#include "queue.h"
#include "set.h"
using namespace MiniSTL;
// ^b*[^:b#/]+.*$

//��ӡ������Ԫ��          *****ע������������������ͣ���Ϊ��Щ����û�ж��帴�ƹ��캯�������ʹ��Ĭ�� ���ܻ�����ڴ�й©��eg list������
template<class cont>
void print_container(cont& a) {
	for (auto it = a.begin();it != a.end();++it)
		std::cout << *it << "  ";
	std::cout << std::endl;
}
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
	print_container<vector<int>>(vec);

	//����insert_aux
	vec.insert_aux(vec.begin()+2,7);
	print_container<vector<int>>(vec);

	//����insert, �Լ�capacity �����仯
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	vec.insert(vec.begin() + 1, 6, 6);
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	print_container<vector<int>>(vec);
	
	//����resize
	//new_size < size()
	vec.resize(5,0);
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	print_container<vector<int>>(vec);
	//new_size > size()
	vec.resize(15, 4);
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	print_container<vector<int>>(vec);

	//����push_back pop_back,back front
	vec.push_back(3);vec.push_back(1);vec.pop_back();
	std::cout << " back: " << vec.back() << "   front: " << vec.front() << std::endl;
	std::cout << "capacity:  " << vec.capacity() << "  size: " << vec.size() << " " << std::endl;
	print_container<vector<int>>(vec);

	//����find,sort    ע���ͱ�����vector<int>::pointer
	vector<int>::pointer ind=find(vec.begin(), vec.end(), 4);
	std::cout << " index: " << ind - vec.begin() << std::endl;

	for_each(vec.begin(), vec.end(), Compare_N(4)); //����4 ��vec[i]=-1,����vec[i]=1;
	print_container<vector<int>>(vec);
}

void test_list() {
	//���Թ��캯��
	list<int> li;
	li.push_back(1);
	li.push_back(2);
	li.push_back(3);
	//li.pop_back();
	print_container<list<int>>(li);
	std::cout << "test front() " <<li.front() << "test back() " << li.back() << " test size() "<<li.size()<< std::endl;
	li.front() = 9;
	li.back() = 4;
	li.insert((li.begin().node->next->next), 66);
	print_container<list<int>>(li);
	li.erase(li.begin());
	print_container<list<int>>(li);
	li.clear();
	print_container<list<int>>(li);
	if (li.empty())
		std::cout << " list is empty." << std::endl;
	else
		std::cout << " list is not empty." << std::endl;
}

void test_deque() {
	//deque<int> dq(126,6);//552
	deque<int> dq;//552
	std::cout << " =======test construct:   =======" << std::endl;
	dq.print_info();

	//test front back; push and pop
	std::cout << " =======test front back; push and pop   =======" << std::endl;
	dq.push_front(1);
	dq.print_info();
	dq.push_back(2);
	dq.print_info();
	dq.pop_back();
	dq.pop_front();
	dq.print_info();

	//erase
	std::cout << " =======test erase   =======" << std::endl;
	dq.push_front(1);
	dq[1] = 2;dq[2] = 3;dq[3] = 4;
	dq.erase(dq.begin() + 1);
	dq.print_info();
	dq.erase(dq.begin(),dq.begin()+3);
	dq.print_info();

	//test insert
	std::cout << " =======test insert   =======" << std::endl;
	dq.insert(dq.begin() + 2, 5);  //�жϷ�֧ ǰ��Ԫ����
	dq.insert(dq.begin() + 120, 2);  //�жϷ�֧ ����Ԫ����
	dq.print_info();

	//test sort  find
	std::cout << " =======test find and for_each   =======" << std::endl;
	deque<int>::iterator ind = find(dq.begin(), dq.end(), 2);
	std::cout << " index: " << ind - dq.begin() << std::endl;

	for_each(dq.begin(), dq.end(), Compare_N(5)); //����4 ��vec[i]=-1,����vec[i]=1;
	dq.print_info();
}

void test_stack() {	
	stack<int> st;
	std::cout << " size: " << st.size() <<" is empty: "<<st.empty()<< std::endl;
	st.push(1);
	st.push(2);
	st.push(5);
	st.push(7);
	std::cout <<" size: "<< st.size() <<" top: "<<st.top()<< std::endl;
	st.top() = -1;
	std::cout << " size: " << st.size() << " top: " << st.top() << std::endl;
	st.pop();std::cout << " size: " << st.size() << " top: " << st.top() << std::endl;
	st.pop();std::cout << " size: " << st.size() << " top: " << st.top() << std::endl;
	st.pop();std::cout << " size: " << st.size() << " top: " << st.top() << " is empty: " << st.empty() << std::endl;
	st.pop();std::cout << " size: " << st.size() << " is empty: " << st.empty() << std::endl;
	//stack<int> st1;
	//bool res1 = st == st1;
	//bool res2 = st < st1;
	//std::cout << res1 << "   <: " << res2 << std::endl;
}
void test_queue() {
	queue<int> que;
	std::cout << " size: " << que.size() << " is empty: " << que.empty() << std::endl;
	que.push(1);
	que.push(2);
	que.push(5);
	que.push(7);
	std::cout << " size: " << que.size() << " front: " << que.front() << " back: " << que.back()<<" is empty: " << que.empty() << std::endl;
	que.front() = -1;
	que.back() = 99;
	std::cout << " size: " << que.size() << " front: " << que.front() <<" back: "<<que.back()<< std::endl;
	que.pop();std::cout << " size: " << que.size() << " front: " << que.front() << " back: " << que.back() << std::endl;
	que.pop();std::cout << " size: " << que.size() << " front: " << que.front() << " back: " << que.back() << std::endl;
	que.pop();std::cout << " size: " << que.size() << " front: " << que.front() << " back: " << que.back() << std::endl;
	que.pop();std::cout << " size: " << que.size() << " is empty: " << que.empty() << std::endl;
}

void test_set() {
	int ia[5] = { 0,1,2,3,4 };
	set<int> iset(ia, ia + 5);
	std::cout << "  size:  "<<iset.size() << std::endl;
	std::cout << "3 count =" << iset.count(3) << std::endl;
	
	iset.insert(3);
	std::cout << "size=" << iset.size() << std::endl;
	std::cout << "3 count =" << iset.count(3) << std::endl;
	
	iset.insert(5);
	std::cout << "size=" << iset.size() << std::endl;
	std::cout << "3 count =" << iset.count(3) << std::endl;
	
	//iset.erase(1);
	std::cout << "size=" << iset.size() << std::endl;
	std::cout << "3 count =" << iset.count(3) << std::endl;
	std::cout << "1 count =" << iset.count(1) << std::endl;

	set<int>::iterator ite1 = iset.begin();
	set<int>::iterator ite2 = iset.end();
	for (; ite1 != ite2; ++ite1) {
		std::cout << *ite1;
	}
	std::cout << std::endl;

	// find��������Ԫ�أ������Ƽ�
	ite1 = find(iset.begin(), iset.end(), 3);
	if (ite1 != iset.end())
		std::cout << "3 found" << std::endl;

	ite1 = find(iset.begin(), iset.end(), -1);
	if (ite1 == iset.end())
		std::cout << "-1 not found" << std::endl;

	// ����ʽ����Ӧʹ��ר�õ�find������������Ч��
	ite1 = iset.find(3);
	if (ite1 != iset.end())
		std::cout << "3 found" << std::endl;

	ite1 = iset.find(-1);
	if (ite1 == iset.end())
		std::cout << "-1 not found" << std::endl;

}
int main() {
	/*
	//����һ�������ռ�������
	std::cout << "test_allocator������һ��������allocator " << std::endl;
	test_allocator();  
	std::cout << std::endl;std::cout << "test_allocator�� ���Զ���������alloc" << std::endl;
	test_alloc();  
	*/

	//���Թ������������
	//test_construct();

	//test_copy();

	//test_vector();

	//test_list();
	//test_deque();
	//test_stack();
	//test_queue();
	test_set();
	return 0;
}