#ifndef _DEQUE_H
#define _DEQUE_H
#include "algorithm.h"
#include "type_traits.h"
#include "alloc.h"
#include "iterator.h"
#include "uninitialized_imp.h"
#include "simple_alloc.h"
#include "construct.h"
using namespace MiniSTL;

namespace MiniSTL {
	inline size_t _deque_buf_size(size_t sz) {
		return sz < 512 ? size_t(512 / sz) : size_t(1);
	}

	template <class T, class Ref, class Ptr>
	class _deque_iterator{
	public:
		typedef random_access_iterator_tag	iterator_category;
		typedef T value_type;
		typedef ptrdiff_t difference_type;
		typedef Ptr	pointer;
		typedef Ref	reference;
		
		typedef T** map_pointer;
		typedef _deque_iterator self;
		typedef _deque_iterator<T, T&, T*> iterator;
		typedef _deque_iterator<const T, const T&, const T*> const_iterator;
		
	public:
		T* cur;
		T* first;
		T* last;
		map_pointer node;

	public:
		static size_t buffer_size() { return _deque_buf_size(sizeof(T)); }

		_deque_iterator(T* x,map_pointer y):cur(x),first(*y),last(*y+buffer_size()),node(y){}
		_deque_iterator() :cur(0), first(0), last(0), node(0) {}
		_deque_iterator(const iterator& x) :cur(x.cur), first(x.first), last(x.last), node(x.node) {}
		
		void set_node(map_pointer new_node) {
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}
		reference operator*()const { return *cur; }
		pointer operator->()const{return &(operator*());}
		difference_type operator-(const self& x) const {
			return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);//x一定在this前面
		}
		self& operator++() {
			++cur;
			if (cur == last) {
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}
		self operator++(int) {  //++i返回引用，++i返回值，因为++i函数返回临时对象tmp，（临时对象不能作为引用（内存泄漏））
			self tmp = *this;
			++*this;
			return tmp;
		}
		/* ????什么问题 diff*/
		self& operator--() {
			if (cur == first) {
				set_node(node - 1);
				cur = last;
			}
			--cur;
			return *this;
		}
	
		self operator--(int) {
			self tmp = *this;
			--* this;
			return tmp;
		}

		self& operator+=(difference_type n) {
			difference_type offset = cur - first + n;
			if (offset >= 0 && offset < difference_type(buffer_size()))
				cur += n;
			else {
				difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()):
					-difference_type((-offset - 1) / buffer_size()) - 1;
				set_node(node + node_offset);
				cur = first + offset - node_offset * difference_type(buffer_size());
			}
			return *this;
		}
		self operator+(difference_type n)const {
			self tmp = *this;
			return tmp += n;
		}
		self& operator-=(difference_type n) {return *this += -n;}
		self operator-(difference_type n)const {
			self tmp = *this;
			return tmp -= n;
		}

		reference operator[](difference_type n)const { return *(*this + n); }
		bool operator==(const self& x)const { return x.cur == cur; }
		bool operator!=(const self& x)const { return !(x == *this); }
		bool operator<(const self& x)const {
			//判断是否在同一块buffer，若在直接判断T*,否则判断map_pointer
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}
	};

	template<class T,class Alloc=alloc,size_t Bufsiz=0>
	class deque {
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef size_t size_type;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef ptrdiff_t difference_type;

		typedef _deque_iterator<T, T&, T*> iterator;
		typedef pointer* map_pointer;

		typedef simple_alloc<value_type, alloc> data_allocator;
		typedef simple_alloc<pointer, alloc> map_allocator;
	public:
		//容器的首尾
		iterator start;
		iterator finish;
		//指向map，map里面的元素都是指针，指向缓冲区的第一个节点
		map_pointer map;
		size_type map_size;
		enum { _initial_map_size = 8 };
		//返回的是buffer=内元素个数，不是总buffer的bytes
		static size_t buffer_size() { return _deque_buf_size(sizeof(T)); }

	public:
		iterator begin() { return start; }
		iterator end() { return finish; }
		//(*this+n),deque[n]这里是*（start+n）
		reference operator[](size_type n) { return start[difference_type(n)]; }
		reference front() { return *start; }
		reference back() {
			//return *(finish - 1);
			iterator tmp = finish;
			--tmp;
			return *tmp;
		}
		size_type size()const { return finish - start; }
		size_type max_size()const { returnn size_type(-1); }
		//size_type max_size()const { returnn map_size*(_deque_buf_size(BufSiz, sizeof(T))); }
		bool empty() { return finish == start; }
		void print_info() {
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << finish.node << "  " << start.node << std::endl;
			std::cout << "map size:  " << map_size <<" cur_map_size:  "<<size_t(finish.node-start.node+1)<< std::endl;
			std::cout << "buffer size:  " << buffer_size() << " cur_buffer_size:  "<<size()<< std::endl;
			for (auto it = begin();it != end();++it)
				std::cout << *it << "  ";
			std::cout << std::endl;
		}
	public:
		//构造函数
		deque(int n,const value_type& value) :start(), finish(), map(0), map_size(0) {
			fill_initialize(n, value);
		}
		deque(int n) :start(), finish(), map(0), map_size(0) {
			fill_initialize(n, value_type());
		}
		deque() :start(), finish(), map(0), map_size(0) {
			//注意默认构造函数必须声明，且必须调用fill_initialize初始化
			fill_initialize(0, value_type());
		}
		~deque() {
			if (map != nullptr) {
				clear(); //clear清除
				data_allocator::deallocate(*start.node,buffer_size());
				*start.node = nullptr;
				map_allocator::deallocate(map, map_size);
				map = nullptr;
			}
		}
		//析构函数
		//notify***********   n
		T* allocate_node() { return data_allocator::allocate(buffer_size()); }
		void deallocate_node(T* p) { data_allocator::deallocate(p,buffer_size()); }		
		
		
		void fill_initialize(size_type n, const value_type& value) {
			create_map_and_node(n);
			map_pointer cur;
			for (cur = start.node;cur < finish.node;++cur)
				uninitialized_fill(*cur, *cur + buffer_size(), value);
			uninitialized_fill(finish.first, finish.cur, value);
		}
		void create_map_and_node(size_type num_elements) {
			size_type num_nodes = num_elements / buffer_size() + 1;  //所需map中所需节点数
			map_size = _initial_map_size > (num_nodes + 2) ? _initial_map_size:(num_nodes + 2);
			//为map分配内存空间
			map = map_allocator::allocate(map_size);
			//居中map
			map_pointer nstart = map + (map_size - num_nodes) / 2;
			map_pointer nfinish = nstart + num_nodes - 1;
			//为buffer分配空间
			map_pointer cur;
			for (cur = nstart;cur <= nfinish;++cur) {
				*cur=allocate_node();
			}
			start.set_node(nstart);
			finish.set_node(nfinish);
			start.cur = start.first;
			finish.cur = finish.first+num_elements%buffer_size();//注意此处

		}

		void push_back(const T& value) {
			if (finish.cur != finish.last-1) {
				construct(finish.cur,value);
				++finish.cur;

			}
			else {
				push_back_aux(value);
			}
		}
		void push_back_aux(const T& value) {
			value_type v_copy = value;
			reserve_map_at_back();
			*(finish.node + 1) = allocate_node();
			construct(finish.cur, v_copy);
			finish.set_node(finish.node + 1);
			finish.cur = finish.first;
		}

		void push_front(const T& value) {
			if (start.cur != start.first) {
				construct(finish.cur-1, value);
				--start.cur;

			}
			else {
				push_front_aux(value);
			}
		}
		void push_front_aux(const T& value) {
			value_type v_copy = value;
			reserve_map_at_front();
			*(start.node - 1) = allocate_node();
			start.set_node(start.node - 1);
			//注意 cur初始在last
			start.cur = start.last - 1;
			construct(start.cur, v_copy);
		}
		void reserve_map_at_back(size_type node_to_add=1) {
			if (node_to_add + 1 > map_size - (finish.node - map))
				reallocate_map(node_to_add, false);
		}
		void reserve_map_at_front(size_type node_to_add = 1) {
			if (node_to_add > start.node-map)
				reallocate_map(node_to_add, true);
		}
		void reallocate_map(size_type node_to_add, bool add_at_front) {
			size_type old_num_nodes = finish.node - start.node + 1;
			size_type new_num_node = old_num_nodes + node_to_add;
			
			map_pointer new_nstart;
			if (map_size > 2 * new_num_node) {
				//map左右不均衡情况
				new_nstart = map + (map_size - new_num_node) / 2 + (add_at_front ? node_to_add : 0);
				if (new_nstart < start.node)
					copy(start.node, finish.node + 1, new_nstart);
				else
					copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
			}
			else {
				size_type new_map_size = map_size + (map_size > node_to_add ? map_size : node_to_add) + 2;
				map_pointer new_map = map_allocator::allocate(new_map_size);
				new_nstart = new_map + (new_map_size - new_num_node) / 2 + (add_at_front ? node_to_add : 0);
				copy(start.node, finish.node, new_nstart);
				map_allocator::deallocate(map, map_size);
				map = new_map;
				map_size = new_map_size;

			}
			start.set_node(new_nstart);
			finish.set_node(new_nstart + old_num_nodes - 1);

		}

		void pop_back() {
			if (finish.cur != finish.first) {
				--finish.cur;
				destroy(finish.cur);
			}
			else
				pop_back_aux();
		}
		void pop_back_aux() {
			deallocate_node(finish.first);
			finish.set_node(finish.node - 1);
			finish.cur = finish.last - 1;
			destroy(finish.cur);
		}
		void pop_front(){
			if (start.cur != start.last - 1) {
				destroy(start.cur);
				++start.cur;
			}
			else
				pop_front_aux();
		}
		void pop_front_aux() {
			destroy(start.cur);
			deallocate_node(start.first);
			start.set_node(start.node + 1);
			start.cur = start.first;
		}
		//clear all (but reserve one buffer)
		void clear() {
			for (map_pointer nd = start.node + 1; nd < finish.node; ++nd) {
				destroy(*nd, *nd + buffer_size());
				data_allocator::deallocate(*nd, buffer_size());
			}
			//head and tail buffer
			if (start.node != finish.node) {
				destroy(start.cur, start.last);
				destroy(finish.first, finish.cur);
				//deallocate tail   reserve head(reserve one buffer)
				data_allocator::deallocate(finish.first, buffer_size());
			}
			else
				destroy(start.cur, finish.cur);  //(reserve one buffer)
			finish = start;
		}

		iterator erase(iterator pos) {
			iterator next = pos;//next=pos+1
			++next;   
			difference_type index = pos - start;//清除点前的元素个数
			if (index < (size() >> 1)) {
				//前面元素比后面少,所以前面的向后移动一个单位
				copy_backward(start, pos, next);
				pop_front();
			}
			else {
				copy(next, finish, pos);
				pop_back();
			}
			return start + index;
		}
		iterator erase(iterator first, iterator last) {
			if (first == start && last == finish) {
				clear();
				return finish;
			}
			else {

				difference_type n = last - first;
				difference_type elem_before = first - start;
				if (elem_before < (size() - n) / 2) {
					//前面元素少
					copy_backward(start, first, last);
					iterator new_start = start + n;
					destroy(start, new_start);
					for (map_pointer cur = start.node;cur < new_start.node;++cur)
						data_allocator::deallocate(*cur, buffer_size());
					start = new_start;
				}
				else {
					copy(last, finish, first);
					iterator new_finish = finish - n;
					destroy(new_finish, finish);
					for(map_pointer cur=new_finish.node+1;cur<=finish.node;++cur)
						data_allocator::deallocate(*cur, buffer_size());
					finish = new_finish;
				}
				return start + elem_before;
			}		
		}

		iterator insert(iterator pos, const value_type& x) {
			if (pos.cur == start.cur) {
				push_front(x);
				return start;
			}
			else if (pos.cur == finish.cur) {
				push_back(x);
				iterator tmp = finish;
				--tmp;
				return tmp;  //return finish-1;
			}
			else
				return insert_aux(pos, x);
		}

		iterator insert_aux(iterator pos, const value_type& x) {
			difference_type index = pos - start; //插入点之前的元素个数
			value_type x_copy = x;
			if (index < size() / 2) {
				//前面元素少
				push_front(front());
				iterator front1 = start;
				++front1;
				iterator front2 = front1;
				++front2;
				pos = start + index;
				iterator pos1 = pos;
				++pos1;
				copy(front2, pos1, front1);
			}
			else {
				push_back(back());
				iterator back1 = finish;
				--back1;
				iterator back2 = back1;
				--back2;
				pos = start + index;
				copy_backward(pos, back2, back1);
			}
			*pos = x_copy;
			return pos;
		}
	};



}

#endif
