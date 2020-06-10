#ifndef _CONSTRUCT_H_
#define _CONSTRUCT_H_
#include <new>
#include "type_traits.h"
using namespace MiniSTL;
/*
����������ݵĹ��������  �ο�p50
construc������
destroy������

//���������ݺ��ڴ�ռ� ����ֿ���ÿһ���ڴ�����룬ϵͳ��Ҫ���ڴ����ҵ�һ����ʴ�С���������ڴ�ռ䣬��������Ǻ����ģ���Զ���)��
//��������£������ǰϵͳ���д������ڴ���Ƭ��������������Ŀռ�ܴ������п���ʧ�ܡ�
//Ϊʲô���ǲ��ܹ���һ����������׼���õ��ڴ��أ�����ʹ��placement new�����������ô�ͻ�������ָ�����ڴ�ռ��й������ 
// ��ˣ�1������128B ����һ��������  ����malloc׼���ÿռ�  ��2��С��128B ʹ���ڴ�ع���free_list���ݴ�С��������ʵ��ڴ�
*/
namespace MiniSTL {
	
	//construct �������
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
