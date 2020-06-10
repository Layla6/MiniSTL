#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include "allocator.h"
#include "iterator.h"
#include "type_traits.h"
using namespace MiniSTL;

namespace MiniSTL {
	//********* [fill] ********************
	template<class ForwardIterator, class T>
	void fill(ForwardIterator first, ForwardIterator last, const T& value)
	{
		for (; first != last; ++first)
			*first = value;
	}
	inline void fill(char* first, char* last, const char& value)
	{
		memset(first, static_cast<unsigned char>(value), last - first);
	}
	inline void fill(wchar_t* first, wchar_t* last, const wchar_t& value)
	{
		memset(first, static_cast<unsigned char>(value), (last - first) * sizeof(wchar_t));
	}
	//********* [fill_n] ********************
	template<class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value)
	{
		for (; n > 0; --n, ++first)
			*first = value;
		return first;
	}
	template<class Size>
	char* fill_n(char* first, Size n, const char& value)
	{
		memset(first, static_cast<unsigned char>(value), n);
		return first + n;
	}
	template<class Size>
	wchar_t* fill_n(wchar_t* first, Size n, const wchar_t& value)
	{
		memset(first, static_cast<unsigned char>(value), n * sizeof(wchar_t));
		return first + n;
	}


	//********** [copy] ******************************
	//完全泛化版本
	template <class InputIterator, class OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
		return __copy_dispatch<InputIterator, OutputIterator>()
			(first, last, result);
	}
	//重载形式
	inline char* copy(char* first, char* last, char* result) {
		auto dist = last - first;
		memmove(result, first, sizeof(*first) * dist);
		return result + dist;
	}
	//重载形式
	inline wchar_t* copy(wchar_t* first, wchar_t* last, wchar_t* result) {
		auto dist = last - first;
		memmove(result, first, sizeof(*first) * dist);
		return result + dist;
	}

	//__copy InputIterator版本
	template<class InputIterator, class OutputIterator>
	inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
		//std::cout << " //__copy InputIterator版本" << std::endl;
		while (first != last) {
			*result = *first;
			++result;
			++first;
		}
		return result;
	}
	//__copy random_access_iterator版本
	template<class InputIterator, class OutputIterator>
	inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, random_access_iterator_tag) {
		//std::cout << " //__copy random_access_iterator版本" << std::endl;
		//iterator.h distance
		auto dist = distance(first, last);
		for (;dist > 0;--dist, ++result, ++first)
			*result = *first;
		return result;
	}


	template <class T>
	inline T* __copy_tri_assign(T* first, T* last, T* result,_true_type) {
		std::cout << " true:__copy_tri_assign版本" << std::endl;
		memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}
	template <class T>
	inline T* __copy_tri_assign(const T* first, const T* last, T* result, _false_type) {
		std::cout << "false: __copy_tri_assign版本" << std::endl;
		auto dist = distance(first, last);
		for (;dist > 0;--dist, ++result, ++first)
			*result = *first;
		return result;
	}

	//完全泛化版本
	template <class InputIterator, class OutputIterator>
	struct __copy_dispatch {
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
			return __copy(first, last, result,iterator_category(first));
		}
	};
	//偏特化版本
	template <class T>
	struct __copy_dispatch <T*,T*>{
		T* operator()(T* first, T* last, T* result) {
			typedef typename MiniSTL::_type_traits<T>::has_trivial_assignment_operator t;
			return __copy_tri_assign(first, last, result, t());
		}
	};
	template <class T>
	struct __copy_dispatch <const T*, const T*> {
		T* operator()(const T* first, const  T* last, T* result) {
			typedef typename MiniSTL::_type_traits<T>::has_trivial_assignment_operator t;
			return __copy_tri_assign(first, last, result, t());
		}
	};

	//********** copy_backward ******************************
	template <class BidirectIterator, class _Distance>
		inline BidirectIterator __copy_backward(BidirectIterator first,BidirectIterator last,BidirectIterator result,bidirectional_iterator_tag,_Distance*){
		while (first != last)
			*--result = *--last;
		return result;
	}
	template <class RandomIterator, class BidirectIterator, class _Distance>
	inline BidirectIterator __copy_backward(RandomIterator first,RandomIterator last,BidirectIterator result,random_access_iterator_tag,_Distance*){
		for (_Distance n = last - first; n > 0; --n)
			*--result = *--last;
		return result;
	}
	template <class BidirectIterator>
	inline BidirectIterator copy_backward(BidirectIterator first, BidirectIterator last, BidirectIterator result) {
		typedef typename iterator_traits<BidirectIterator>::iterator_category _Cat;
		typedef typename iterator_traits<BidirectIterator>::difference_type _Distance;
		return __copy_backward(first, last, result, _Cat(), (_Distance*)0);
	}







}
#endif