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
		Sequence c;  //ʹ�õײ�����

	public:
		bool empty() { return c.empty(); }
		size_type size() { return c.size(); }
		reference top() { return c.back(); }
		void push(const value_type& x) { c.push_back(x); }
		void pop() { c.pop_back(); }
		/*deque��ʱû���� == �� <
		bool operator==(const stack<T>& x)
		{
			return x.c == c;
		}
		// �Ƚ������������Ĵ�С�����Ƚϵײ������Ĵ�С
		bool operator<(const stack<T>& x)
		{
			return x.c < c;
		}
		*/
	};

}

#endif // !_STACK_H

/*

??????Ϊʲô������ ���벻ͨ��
friend bool operator< <T>(const stack<T>&, const stack<T>&);
		template<class T>
		friend bool operator==(const stack<T>&, const stack<T>&);
		template<class T>
		friend bool operator<(const stack<T>&, const stack<T>&);
(1)
��ģ�庯��������Ϊ��ģ�����Ԫ�Ҷ�������ģ��֮��ʱ���ں�����֮��������ģ��ʵ�α�
�����������Ԫ����ָ����ģ���ʵ����������Ԫ�����ᱻ����Ϊһ����ģ�庯��������ʱ�޷�������
��Ԫģ�庯����ģ��������ͣ�����һ��Ҫ������ģ��Ĳ������ͣ�Ҳ��������������
(2)˫Ŀ��������� ������Ԫ
���������˫Ŀ����������Ϊ��ĳ�Ա����������ֻҪ����һ��������Ϊ�Ҳ�����������������������Ƕ�����
�� >>  ��<< ����������� cin��cout �����Ƕ��������Բ��������һ�㣬��ֻ������Ϊ��Ԫ�����ˡ�
*/