#ifndef _FUNCTION_H
#define _FUNCTION_H

#include <stdint.h> 

namespace MiniSTL {
	//仿函数 
	template <class Arg1, class Arg2, class Result>
	struct binary_function {
		typedef Arg1 first_argument_type; // 参数一型别
		typedef Arg2 second_argument_type; // 参数二型别
		typedef Result result_type; // 返回值型别
	};
	template <class Arg, class Result>
	struct unary_function {
		typedef Arg argument_type; // 参数型别
		typedef Result result_type; // 返回值型别
	};

	// 选择函数(select)，接受一个pair返回其第一个元素，用于map实现中，用来指定RB-tree所需KeyOfValue op，
	// 因为map以pair的第一个元素作为键值
	template <class Pair>
	struct Select1st : public unary_function<Pair, typename Pair::first_type> {
		const typename Pair::first_type& operator()(const Pair& x) const {
			return x.first;
		}
	};

	//加法
	template<class T>
	struct plus :public binary_function <T,T,T>{
		T operator()(const T& x, const T& y) const { return x + y; }
	};
	//减法
	template <class T>
	struct minus :public binary_function<T, T, T>
	{
		T operator()(const T& x, const T& y) const { return x - y; }
	};

	//乘法
	template <class T>
	struct multiplies :public binary_function<T, T, T>
	{
		T operator()(const T& x, const T& y) const { return x * y; }
	};

	//除法
	template <class T>
	struct divides :public binary_function<T, T, T>
	{
		T operator()(const T& x, const T& y) const { return x / y; }
	};
	// 等于
	template <class T>
	struct equal_to :public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x == y; }
	};

	//不等于
	template <class T>
	struct not_equal_to :public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x != y; }
	};

	// 大于
	template <class T>
	struct greater :public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x > y; }
	};

	//小于
	template <class T>
	struct less :public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x < y; }
	};

	// 大于等于
	template <class T>
	struct greater_equal :public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x >= y; }
	};

	// 小于等于
	template <class T>
	struct less_equal :public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x <= y; }
	};

	// 逻辑与
	template <class T>
	struct logical_and :public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x && y; }
	};

	// 逻辑或
	template <class T>
	struct logical_or :public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x || y; }
	};

	// 逻辑非
	template <class T>
	struct logical_not :public unary_function<T, bool>
	{
		bool operator()(const T& x) const { return !x; }
	};

	//证同函数：不会改变元素，返回本身
	//指定rb_tree所需的keyofvalue op,set元素的键值即实值，所以采用identity
	template<class T>
	struct identity:public unary_function<T,T>{
		const T& operator()(const T& x) const { return x; }
	};

	// 选择函数：接受一个 pair，返回第一个元素
	//指定rb_tree所需的keyofvalue op,map元素的键值即pair的第一元素，所以采用select1st
	template <class Pair>
	struct select1st :public unary_function<Pair, typename Pair::first_type>
	{
		const typename Pair::first_type& operator()(const Pair& x) const
		{
			return x.first;
		}
	};

	// 选择函数：接受一个 pair，返回第二个元素
	template <class Pair>
	struct select2nd :public unary_function<Pair, typename Pair::second_type>
	{
		const typename Pair::second_type& operator()(const Pair& x) const
		{
			return x.second;
		}
	};

	// 投射函数：返回第一参数
	template <class Arg1, class Arg2>
	struct project1st :public binary_function<Arg1, Arg2, Arg1>
	{
		Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
	};

	// 投射函数：返回第二参数
	template <class Arg1, class Arg2>
	struct project2nd :public binary_function<Arg1, Arg2, Arg1>
	{
		Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
	};


}

#endif // !_FUNCTION_H

