#ifndef _QUEUE_H
#define _QUEUE_H
#include "algorithm.h"
#include "deque.h"
using namespace MiniSTL;

namespace MiniSTL {

    template <class T, class Sequence = deque<T> >
    class queue{
        /*
        template<class T>
        friend bool operator==(const queue& x, const queue& y);
        template<class T>
        friend bool operator<(const queue& x, const queue& y);
        */
    public:
        // ����queue��֧�ֶԶ�ͷ�Ͷ�β�Ĳ���, ���Բ�����STLҪ���
        // pointer, iterator, difference_type
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;

    protected:
        Sequence c;   // �ײ�����

    public:
        // ���²�����stackһ��
        bool empty()  { return c.empty(); }
        size_type size() { return c.size(); }
        reference front() { return c.front(); }
        reference back() { return c.back(); }
        void push(const value_type& x) { c.push_back(x); }
        void pop() { c.pop_front(); }
    };
}
/*
// ����==���������Ƚϵײ���������
template <class T>
bool operator==(const queue<T>& x, const queue<T>& y)
{
    return x.c == y.c;
}
// ͬ��
template <class T>
bool operator<(const queue<T>& x, const queue<T>& y)
{
    return x.c < y.c;
}
*/
#endif


