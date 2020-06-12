#include <cstddef>
#ifndef _ITERATOR_H
#define _ITERATOR_H
using namespace MiniSTL;
namespace MiniSTL {

	struct input_iterator_tag { input_iterator_tag() { std::cout << " I am input" << std::endl; } };
	struct output_iterator_tag { output_iterator_tag() { std::cout << " I am out" << std::endl; } };
	struct forward_iterator_tag :public input_iterator_tag { forward_iterator_tag() { std::cout << " I am forwoard" << std::endl; } };
	struct bidirectional_iterator_tag :public forward_iterator_tag { bidirectional_iterator_tag() { std::cout << " I am bi" << std::endl; } };
	struct random_access_iterator_tag :public bidirectional_iterator_tag { random_access_iterator_tag() { std::cout << " I am random" << std::endl; } };



	template<class Category, class T, class Distance = ptrdiff_t,class Pointer = T*, class Reference = T&>
	struct iterator
	{
		typedef Category	iterator_category;
		typedef T			value_type;
		typedef Distance	difference_type;
		typedef Pointer		pointer;
		typedef Reference	reference;
	};

	template<class Iterator>
	struct iterator_traits
	{
		typedef typename Iterator::iterator_category	iterator_category;
		typedef typename Iterator::value_type			value_type;
		typedef typename Iterator::difference_type		difference_type;
		typedef typename Iterator::pointer				pointer;
		typedef typename Iterator::reference 			reference;
	};
	template<class T>
	struct iterator_traits<T*>
	{
		typedef random_access_iterator_tag 	iterator_category;
		typedef T 							value_type;
		typedef ptrdiff_t 					difference_type;
		typedef T* pointer;
		typedef T& reference;
	};
	template<class T>
	struct iterator_traits<const T*>
	{
		typedef random_access_iterator_tag 	iterator_category;
		typedef T 							value_type;
		typedef ptrdiff_t 					difference_type;
		typedef const T* pointer;
		typedef const T& reference;
	};

	template<class Iterator>
	inline typename iterator_traits<Iterator>::iterator_category
		iterator_category(const Iterator&) {
		typedef typename iterator_traits<Iterator>::iterator_category category;
		return category();//返回一个临时对象，只为区别类型
	}

	template<class Iterator>
	inline typename iterator_traits<Iterator>::value_type*
		value_type(const Iterator&) {
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

	template<class Iterator>
	inline typename iterator_traits<Iterator>::difference_type*
		distance_type(const Iterator&) {
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}
	//*************** distance *************************
	template<class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		_distance(InputIterator first, InputIterator last, input_iterator_tag) {
		typename iterator_traits<InputIterator>::difference_type dist = 0;
		while (first++ != last) {
			++dist;
		}
		return dist;
	}

	template<class RandomIterator>
	typename iterator_traits<RandomIterator>::difference_type
		_distance(RandomIterator first, RandomIterator last, random_access_iterator_tag) {
		auto dist = last - first;
		return dist;
	}
	template<class Iterator>
	typename iterator_traits<Iterator>::difference_type
		distance(Iterator first, Iterator last) {
		typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
		return _distance(first, last, iterator_category());
	}
	//*************** advance *************************
	template<class InputIterator, class Distance>
	void _advance(InputIterator& it, Distance n, input_iterator_tag) {
		assert(n >= 0);
		while (n--) {
			++it;
		}
	}
	template<class BidirectionIterator, class Distance>
	void _advance(BidirectionIterator& it, Distance n, bidirectional_iterator_tag) {
		if (n < 0) {
			while (n++) {
				--it;
			}
		}
		else {
			while (n--) {
				++it;
			}
		}
	}
	template<class RandomIterator, class Distance>
	void _advance(RandomIterator& it, Distance n, random_access_iterator_tag) {
		if (n < 0) {
			it -= (-n);
		}
		else {
			it += n;
		}
	}
	template <class InputIterator, class Distance>
	void advance(InputIterator& it, Distance n) {
		typedef iterator_traits<InputIterator>::iterator_category iterator_category;
		_advance(it, n, iterator_category());
	}
}

#endif // !_ITERATOR_H

