#ifndef _LIST_H
#define _LIST_H
#include "algorithm.h"
#include "type_traits.h"
#include "alloc.h"
#include "iterator.h"
#include "uninitialized_imp.h"
#include "simple_alloc.h"
#include "construct.h"
using namespace MiniSTL;

namespace MiniSTL {
   
    template<class T>
    struct _list_node{
        _list_node<T>* prev;
        _list_node<T>* next;
        T data;
    };
   
    template <class T,class Ref,class Ptr>
    struct _list_iterator {
        typedef _list_iterator<T, T&, T*> iterator;
        typedef _list_iterator<T, Ref, Ptr> self;

        typedef bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef _list_node<T>* link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        link_type node;//��ǰ������ ָ���list�Ľڵ�

        // constructor
        _list_iterator(){}
        _list_iterator(link_type x) : node(x) {}
        _list_iterator(const iterator& x) : node(x.node) {}

        // ������ dereference
        reference operator*() const { return (*node).data;}
        // member access
        pointer operator->() const {return &(operator*());}
        // prefix increment, ���ض���
        self& operator++() {
            node = (link_type)((*node).next);
            return *this;
        }
        // postfix increment, ����ֵ���¶���
        // intΪռλ������ʾ���������Ǻ�����
        self operator++(int) {
            self temp = *this;
            ++* this; // ����ǰ���ǰ����
            return temp;
        }
        self& operator--() {
            node = (link_type)((*node).prev);
            return *this;
        }
        self operator--(int) {
            self temp = *this;
            --* this; 
            return temp;
        }
        bool operator==(const self& i) const {return node==i.node;}
        bool operator!=(const self& i) const {return node != i.node;}
    };


    template<class T,class Alloc=alloc>
    class list {
        
    public: 
        typedef T value_type;
        typedef value_type* pointer;
        typedef value_type& reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef _list_iterator<T,T&,T*> iterator;
        typedef _list_node<T> list_node;
        typedef _list_node<T>* link_type;
        //ר��֮�ռ���������ÿ������һ���ڵ��С
        typedef simple_alloc<list_node, Alloc> list_node_allocator;


    protected:
        link_type node;  //ע������list_node�ڵ��ָ��

    public:
        //ʹ��_list_iterator(link_type x) : node(x) {} ���캯�� ����iterator����
        iterator begin() { return (*node).next; }
        iterator end() { return node; }
        bool empty() const { return node->next == node; }
        size_type size() {                                     //���ܼ�const  ����������������������������
            size_type result = 0;
            result = size_type(distance(begin(),end()));
            return result;
        }
        reference front() { return *begin(); }
        reference back() { return *(--end()); }



        //�ڵ���ڴ�ռ�����
        link_type get_node() { return list_node_allocator::allocate(); }
        //�ڵ���ڴ�ռ��ͷ�
        void put_node(link_type p) { list_node_allocator::deallocate(p); }
        //���ò�����һ���ڵ�
        link_type create_node(const T& x) {
            link_type p = get_node();
            construct((&(p->data)), x);
            return p;
        }
        //�ͷŲ�����һ���ڵ�
        void destroy_node(link_type p) {
            destroy(&(p->data));
            put_node(p);
        }

        //list���캯��
        list() { empty_initialize(); }
        void empty_initialize() {
            node = get_node();
            node->next = node;
            node->prev = node;
        }

        ~list(){
            if (node){
                clear();
                node = nullptr;
            }
        }


        iterator insert(iterator position, const T& x) {
            link_type tmp = create_node(x);
            // ����ָ��
            tmp->next = position.node;
            tmp->prev = position.node->prev;
            position.node->prev->next = tmp;
            position.node->prev = tmp;
            return tmp;
        }
        void push_front(const T& x) { insert(begin(), x); }
        void push_back(const T& x) { insert(end(), x); }

        iterator erase(iterator position) {
            link_type next_node = position.node->next;
            link_type prev_node = position.node->prev;
            prev_node->next = next_node;
            next_node->prev = prev_node;
            destroy_node(position.node);
            return iterator(next_node);
        }
        void pop_front() { erase(begin();) }
        void pop_back() { erase(--end()); }

        void clear() {
            link_type cur = (link_type)node->next; //��ʼ�ڵ�
            while (cur != node) {  //nodeΪ��β��dummy�ڵ�
                link_type tmp = cur;
                cur = (link_type)cur->next;
                destroy_node(cur);
            }
            node->next = node;
            node->prev = node;
        }


    };
}

#endif // !_LIST_H

