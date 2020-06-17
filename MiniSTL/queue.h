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
        // 由于queue仅支持对队头和队尾的操作, 所以不定义STL要求的
        // pointer, iterator, difference_type
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;

    protected:
        Sequence c;   // 底层容器

    public:
        // 以下操作和stack一样
        bool empty()  { return c.empty(); }
        size_type size() { return c.size(); }
        reference front() { return c.front(); }
        reference back() { return c.back(); }
        void push(const value_type& x) { c.push_back(x); }
        void pop() { c.pop_front(); }
    };
}
/*
// 重载==操作符，比较底层容器即可
template <class T>
bool operator==(const queue<T>& x, const queue<T>& y)
{
    return x.c == y.c;
}
// 同上
template <class T>
bool operator<(const queue<T>& x, const queue<T>& y)
{
    return x.c < y.c;
}
*/
#endif


