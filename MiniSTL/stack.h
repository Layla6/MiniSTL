#ifndef _STACK_H
#define _STACK_H
#include "algorithm.h"
#include "deque.h"
using namespace MiniSTL;

namespace MiniSTL {


	template<class T,class Sequence=deque<T>>
	class stack {

	public:
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::size_type size_type;
		typedef typename Sequence::reference reference;

	protected:
		Sequence c;  //使用底层容器

	public:
		bool empty() { return c.empty(); }
		size_type size() { return c.size(); }
		reference top() { return c.back(); }
		void push(const value_type& x) { c.push_back(x); }
		void pop() { c.pop_back(); }
		/*deque暂时没重载 == 和 <
		bool operator==(const stack<T>& x)
		{
			return x.c == c;
		}
		// 比较两个迭代器的大小，即比较底层容器的大小
		bool operator<(const stack<T>& x)
		{
			return x.c < c;
		}
		*/
	};

}

#endif // !_STACK_H

/*

??????为什么有问题 编译不通过
friend bool operator< <T>(const stack<T>&, const stack<T>&);
		template<class T>
		friend bool operator==(const stack<T>&, const stack<T>&);
		template<class T>
		friend bool operator<(const stack<T>&, const stack<T>&);
(1)
当模板函数被声明为类模板的友元且定义在类模板之外时，在函数名之后必须紧跟模板实参表，
用来代表该友元声明指向函数模板的实例。否则友元函数会被解释为一个非模板函数，链接时无法解析。
友元模板函数的模板参数类型，并不一定要求是类模板的参数类型，也可以另外声明。
(2)双目运算符重载 声明友元
如果是重载双目操作符（即为类的成员函数），就只要设置一个参数作为右侧运算量，而左侧运算量就是对象本身。
而 >>  或<< 左侧运算量是 cin或cout 而不是对象本身，所以不满足后面一点，就只能申明为友元函数了。
*/