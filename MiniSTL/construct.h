#ifndef _CONSTRUCT_H_
#define _CONSTRUCT_H_
#include <new>
#include "type_traits.h"
using namespace MiniSTL;
/*
负责对象内容的构造和析构  参考p50
construc：构造
destroy：析构

//将对象内容和内存空间 步骤分开，每一次内存的申请，系统都要在内存中找到一块合适大小的连续的内存空间，这个过程是很慢的（相对而言)。
//极端情况下，如果当前系统中有大量的内存碎片，并且我们申请的空间很大，甚至有可能失败。
//为什么我们不能共用一块我们事先准备好的内存呢？可以使用placement new来构造对象，那么就会在我们指定的内存空间中构造对象。 
// 因此（1）大于128B 调用一级配置器  事先malloc准备好空间  （2）小于128B 使用内存池管理，free_list根据大小分配最合适的内存
*/
namespace MiniSTL {
	
	//construct 构造对象
	// placement new https://blog.csdn.net/linuxheik/article/details/80449059
	template<class T1,class T2>
	inline void construct(T1* p, const T2& value) {
		new(p) T1(value);
	}

	//destroy  version1:para is a pointer
	template<class T>
	inline void destroy(T* pointer) {
		pointer->~T();
	}

	//destroy  version2:para is two iterator 
	//*********value_type aim to extact type ************
	template<class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last) {
		_destroy(first, last, value_type(first));
	}
	//TinySTL use Iterator extract is_POD_type
	//but I perfer to use T extact trivial_destructor(p51)
	template<class ForwardIterator,class T>
	inline void _destroy(ForwardIterator first, ForwardIterator last,T*) {
		typedef typename _type_traits<T>::has_trivial_destructor trivial_destructor;
		_destroy_aux(first, last, trivial_destructor());
	}
	template<class ForwardIterator>
	inline void _destroy_aux(ForwardIterator first, ForwardIterator last,_false_type){
		for (;first < last;++first)
			destroy(&*first);
	}
	template<class ForwardIterator>
	inline void _destroy_aux(ForwardIterator first, ForwardIterator last, _true_type){}

	inline void destroy(char*, char*) {}
	inline void destroy(wchar_t*, wchar_t*) {}
	
}


#endif
