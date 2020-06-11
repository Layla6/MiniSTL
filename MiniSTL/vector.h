#ifndef _VECTOR_H
#define _VECTOR_H

#include "algorithm.h"
#include "type_traits.h"
#include "alloc.h"
#include "iterator.h"
#include "uninitialized_imp.h"
#include "simple_alloc.h"
#include "construct.h"
using namespace MiniSTL;
namespace MiniSTL {
	
	template<class T,class Alloc=alloc>
	class vector {

	public:
		//vector的嵌套型别定义
		typedef T value_type;
		typedef value_type* pointer;
		typedef value_type* iterator;
		typedef value_type& reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

	protected:
		typedef simple_alloc<value_type, Alloc> data_allocate;
		iterator start;
		iterator finish;
		iterator end_of_storage;

		void deallocate() {
			if (start)
				data_allocate::deallocate(start, end_of_storage - start);
		}

		void fill_initialize(size_type n, const T& value) {
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
		}

		iterator allocate_and_fill(size_type n, const T& value) {
			iterator result = data_allocate::allocate(n);
			uninitialized_fill_n(result, n, value);
			return result;
		}
	public: 

		iterator begin() { return start; }
		iterator end() { return finish; }
		size_type size() { return size_type(end() - begin()); }
		size_type capacity(){ return size_type(end_of_storage - begin()); }
		bool empty() const { return begin() == end(); }
		reference operator[](size_type n) { return *(begin() + n); }

		//构造函数与析构函数   ***后续可以增加
		vector():start_(0), finish_(0), endOfStorage_(0) {}
		explicit vector(const size_type n) { fill_initialize(n, T()); }
		vector(const size_type n, const value_type& value) { fill_initialize(n, value); }
		~vector() {
			destroy(start, finish);
			deallocate();  //是个成员函数
		}


		//vector 基本操作
		reference front() { return *(begin()); }
		reference back() { return *(end() - 1); }

		void push_back(const T& value) {
			if (finish != end_of_storage) {
				construct(finish, value);
				++finish;
			}
			else
				insert_aux(end(), value);
		}

		void pop_back() {
			--finish;
			destroy(finish);
		}

		iterator erase(iterator position) {
			if (position + 1 != end())
				copy(position + 1, finish, position);
			--finish;
			destroy(finish);
			return position;
		}
		
		iterator erase(iterator it1,iterator it2) {
			iterator old_finish = finish;
			copy(it2, finish, it1);//注意这里是左开右闭
			finish = finish - (it2 - it1);
			destroy(finish,old_finish);
			return it1;
		}
		void claer() { erase(begin(), end()); }
		void resize(size_type new_size, const T& x) {
			if (new_size < size())
				erase(begin() + new_size, end());
			else
				insert(end(), new_size-size(),x);
		}
		void resize(size_type new_size) { resize(new_size, T()); }
		void insert_aux(iterator position, const T& x) {
			if (finish != end_of_storage) {
				//还有位置
				construct(finish, x);
				finish++;
				copy_backward(position, finish - 2, finish - 1);
				*position = x;
			}
			else {
				//无位置存放一个元素，因此重新分配vectoe空间
				const size_type old_size = size();
				const size_type new_size = old_size != 0 ? 2 * old_size : 1;
				iterator new_start = data_allocate::allocate(new_size);
				iterator new_finish = new_start;
				new_finish = uninitialized_copy(start, position, new_start);
				construct(new_finish, x);
				++new_finish;
				new_finish = uninitialized_copy(position, finish, new_finish);
				//析构并释放原vector
				destroy(begin(), end());
				deallocate();//旧的参数
				//更新新参数
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + new_size;
			}
		}
		void insert(iterator position, size_type n, const T& x) {
			if (n <= 0) return;
			if (size_type(end_of_storage-finish)>=n) {
				//还有位置
				T x_copy = x;
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				//如果插入点之后的现有元素个数大于新增元素个数，则会出现重叠，因此需要copy_backward
				if (elems_after > n) {
					uninitialized_copy(finish - n, finish, finish);
					finish += n;
					copy_backward(position, old_finish - n,old_finish);
					fill(position, position + n, x_copy);
				}
				else {
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n-elems_after;
					uninitialized_copy(position, old_finish, finish);
					finish += elems_after;
					fill(position,old_finish, x_copy);
				}
			}
			else {
				//无位置存放一个元素，因此重新分配vectoe空间
				const size_type old_size = size();
				const size_type new_size = old_size+(old_size>n?old_size:n);
				iterator new_start = data_allocate::allocate(new_size);
				iterator new_finish = new_start;
				new_finish = uninitialized_copy(start, position, new_start);
				new_finish = uninitialized_fill_n(new_finish, n, x);
				new_finish = uninitialized_copy(position, finish, new_finish);
				//析构并释放原vector
				destroy(begin(), end());
				deallocate();//旧的参数
				//更新新参数
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + new_size;
			}
		}

	};

}

#endif // !_VECTOR_H

