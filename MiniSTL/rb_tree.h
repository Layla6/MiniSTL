#ifndef _RB_TREE_H
#define _RB_TREE_H

#include "algorithm.h"
#include "type_traits.h"
#include "alloc.h"
#include "iterator.h"
#include "uninitialized_imp.h"
#include "simple_alloc.h"
#include "construct.h"
#include "util.h"
using namespace MiniSTL;

namespace MiniSTL {
	
	typedef bool rb_tree_color_type;
	const rb_tree_color_type rb_tree_red = false;  //��ɫΪ0
	const rb_tree_color_type rb_tree_black = true;  //��ɫΪ1

	//rb_tree�Ľڵ����

	struct rb_tree_node_base
	{
		typedef rb_tree_color_type color_type;
		typedef rb_tree_node_base* base_ptr;

		color_type color;
		base_ptr parent;
		base_ptr left;
		base_ptr right;

		static base_ptr minimum(base_ptr x) {
			while (x->left!=0)
				x = x->left;
			return x;
		}
		static base_ptr maximum(base_ptr x) {
			while (x->right != 0)
				x = x->right;
			return x;
		}

	};

	template<class T>
	struct rb_tree_node:public rb_tree_node_base
	{
		typedef rb_tree_node<T>* link_type;
		T value;
	};

	//rb_tree�ĵ��������
	struct rb_tree_base_iterator
	{
		typedef rb_tree_node_base::base_ptr base_ptr;
		typedef bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t difference_type;
		
		base_ptr node;  //����������֮�����һ�������ϵ

		void increment() {
			if (node->right != 0) {
				//case1:node�ҽڵ㲻Ϊ�գ����Ҹ��ҽڵ������ڵ�
				node = node->right;
				while (node->left != 0)
					node = node->left;
			}
			else {
				//node�ҽڵ�Ϊ��
				//case2:����Ѱ���ӽڵ㲻Ϊ���ڵ���ҽڵ�
				//case3:�ӽڵ�Ϊ���ڵ�����ӽڵ�
				//case4:Ϊ���ڵ㣨�ø��ڵ����ҽڵ㣩
				base_ptr y = node->parent;
				while (node==y->right){      //case2
					node = y;
					y = y->parent;
				}
				if (node->right != y)         //if true case3 else case4
					node = y;

			}
		}
		void decrement() {
			if (node->color == rb_tree_red && node->parent->parent == node)  //��ǰ�ڵ�Ϊheader�� header��right Ϊ mostright
				node = node->right;
			else if(node->left!=0){          //���ýڵ����ӽڵ㲻Ϊ�գ�����Ҹ����ӽڵ�������ӽڵ�
				base_ptr y = node->left;
				while (y->right != 0)
					y = y->right;
				node = y;
			}
			else {
				base_ptr y = node->parent;   //����Ѱ���ӽڵ�Ϊ���ڵ�����ӽڵ�
				while (node == y->left) {
					node = y;
					y = y->parent;
				}
				node = y;
			}

		}

	};

	template <class T,class Ref,class Ptr>
	struct rb_tree_iterator:public rb_tree_base_iterator
	{
		typedef T value_type;
		typedef Ptr    pointer;
		typedef Ref reference;
		typedef rb_tree_iterator<T,T&,T*>              iterator;
		typedef rb_tree_iterator<T,const T&,const T*>        const_iterator;
		typedef rb_tree_iterator<T,Ref,Ptr>                         self;

		typedef rb_tree_node<T>* link_type;

		rb_tree_iterator(){}
		rb_tree_iterator(link_type x) { node = x; }
		rb_tree_iterator(const iterator& it) { node = it.node; }
		 
		reference operator*()const { return link_type(node)->value; }
		pointer operator->()const { return &(operator*()); }
		self& operator++() { increment();return *this; }
		self operator++(int) {
			self tmp = *this;
			increment();
			return tmp;
		}

		self& operator--() { decrement();return *this; }
		self operator--(int) {
			self tmp = *this;
			decrement();
			return tmp;
		}
	};

	inline bool operator==(const rb_tree_base_iterator& x,const rb_tree_base_iterator& y) {
		return x.node == y.node;
	}
	inline bool operator!=(const rb_tree_base_iterator& x, const rb_tree_base_iterator& y) {
		return x.node != y.node;
	}

	template<class T_key,class T_value,class keyofvalue,class compare,class Alloc=alloc>
	class rb_tree {
	protected:
		typedef void* void_pointer;
		typedef rb_tree_node_base* base_ptr;
		typedef rb_tree_node<T_value> rb_tree_node;
		typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
		typedef rb_tree_color_type color_type;

	public:
		typedef T_key key_type;
		typedef T_value value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef rb_tree_node* link_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

	protected:
		//�����ڴ�ռ�
		link_type get_node() { return rb_tree_node_allocator::allocate(); }
		void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }
		//�����ڴ�ռ��������
		link_type create_node(const value_type& x) {
			link_type tmp = get_node();
			construct(&tmp->value, x);
			return tmp;
		}
		link_type clone_node(link_type x) {
			link_type tmp = create_node(x->value);
			tmp->color = x->color;
			tmp->left = 0;
			tmp->right = 0;
			return tmp;
		}

		void destroy_node(link_type p) {
			destroy(&p->value);  //�������ݣ�new�ģ� left��right����new��
			put_node(p);  //�ͷ��ڴ�
		}

	protected:
		// �������������ݱ��� rb tree
		link_type    header;      // ����ڵ㣬����ڵ㻥Ϊ�Է��ĸ��ڵ�
		size_type   node_count;  // �ڵ���
		compare key_comp;   //�ڵ��ļ�ֵ��С�Ƚ�׼�򣨺�������

		// ����������������ȡ�ø��ڵ㣬��С�ڵ�����ڵ�
		link_type& root()      const { return (link_type&)header->parent; }
		link_type& leftmost()  const { return (link_type&)header->left; }
		link_type& rightmost() const { return (link_type&)header->right; }

		//ȡ��node�ĳ�Ա
		static link_type& left(link_type x) { return (link_type&)(x->left); }
		static link_type& right(link_type x) { return (link_type&)(x->right); }
		static link_type& parent(link_type x) { return (link_type&)(x->parent); }
		static reference value(link_type x) { return x->value; }
		//ע��(key value)��һ���ڵ㣬set����key��value��һ��Ԫ�أ�map��Ϊpair.first��pair.second
		static const key_type& key(link_type x) { return keyofvalue()(value(x)); }
		static color_type& color(link_type x) { return (color_type)(x->color); }

		//ȡ��node�ĳ�Ա   //
		static link_type& left(base_ptr x) { return (link_type&)(x->left); }
		static link_type& right(base_ptr x) { return (link_type&)(x->right); }
		static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
		static reference value(base_ptr x) { return ((link_type)x)->value; }
		//ע��(key value)��һ���ڵ㣬set����key��value��һ��Ԫ�أ�map��Ϊpair.first��pair.second
		//��� set��keyofvalueΪidentity ��ֵ��ʵֵ
		//map��keyofvalueΪselect1st��mapԪ�صļ�ֵ��pair�ĵ�һԪ��
		static const key_type& key(base_ptr x) {return keyofvalue()(value(x)); }
		static color_type& color(base_ptr x) { return (color_type)(x->color); }


		static link_type minimum(link_type x) {return (link_type)rb_tree_node_base::minimum(x);}
		static link_type maximum(link_type x) {return (link_type)rb_tree_node_base::maximum(x);}

		
	public:
		typedef rb_tree_iterator<value_type, reference, pointer> iterator;
		typedef rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;
		
	private:
		iterator rb_insert(base_ptr x, base_ptr y, const value_type& v);
		link_type rb_copy(link_type x, link_type p);

		void init() {
			header = get_node();
			color(header) = rb_tree_red;
			root() = 0;   //�Ƚ�root�ÿգ���header->parent��
			leftmost() = header;
			rightmost() = header;
		}

	public:
		rb_tree(const compare& comp = compare()):node_count(0), key_comp(comp){init();}
		//���ƹ��캯��
		rb_tree(const rb_tree& rhs)
		{
			init();
			if (rhs.node_count != 0)
			{
				color(header) = rb_tree_red;
				root() = rb_copy(rhs.root(), header);
				leftmost() = rb_tree_min(root());
				rightmost() = rb_tree_max(root());
			}
			node_count = rhs.node_count;
			key_comp = rhs.key_comp;
		}
		~rb_tree() {
			clear();
			//put_node(header);
		}

		//��ֵ������
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>&
			operator=(const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& x);

	public:
		compare key_compare()const { return key_comp; }
		iterator begin() { return leftmost(); }
		const_iterator begin() const { return leftmost(); }
		iterator end() { return header; }
		const_iterator end() const { return header; }
		bool empty()const { return node_count == 0; }
		size_type size()const { return node_count; }
		// (size_type(-1)) should evaluate to the maximum object size on a platform)...
		size_type max_size()const { return size_type(-1); }
		void swap(rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& x) {
			std::swap(header, x.header);
			std::swap(node_count, x.node_count);
			std:;swap(key_comp, x.key_comp);
		}
	public:
		//insert/erase
		//�������ظ�
		pair<iterator, bool> insert_unique(const value_type& x);
		//�����ظ�
		iterator insert_equal(const value_type& x);

		template<class InputIterator>
		void insert_equal(InputIterator first, InputIterator last);
		template<class InputIterator>
		void insert_unique(InputIterator first, InputIterator last);

		void insert_unique(const_iterator first, const_iterator last);
		void insert_unique(const value_type* first, const value_type* last);
		void insert_equal(const_iterator first, const_iterator last);
		void insert_equal(const value_type* first, const value_type* last);
		//iterator insert_unique(iterator pos, const value_type& x);
		//iterator insert_equal(iterator pos, const value_type& x);
		void erase(link_type x);
		void erase(iterator position);
		size_type erase(const key_type& x);//ע��erase��key_type��insert��value_type������key,value��
		void erase(iterator first, iterator last);
		void erase(const key_type* first, const key_type* last);
		void clear() {
			if (node_count != 0) {
				erase(root());
				leftmost() = header;
				root() = 0;
				rightmost() = header;
				node_count = 0;
			}
		}
	public:
		//set operation
		iterator find(const key_type& x);
		const_iterator find(const key_type& x) const;
		size_type count(const key_type& x) const;
		iterator lower_bound(const key_type& x);
		const_iterator lower_bound(const key_type& x) const;
		iterator upper_bound(const key_type& x);
		const_iterator upper_bound(const key_type& x) const;
		pair<iterator, iterator> equal_range(const key_type& x);
		pair<const_iterator, const_iterator> equal_range(const key_type& x) const;
	};

	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
		inline bool
		operator==(const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& x,
			const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& y){
		return x.size() == y.size() &&
			equal(x.begin(), x.end(), y.begin());
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	inline bool
		operator!=(const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& x,
			const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& y) {return !(x==y);}

	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
		inline void swap(rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& x,
			rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& y){x.swap(y);}
	
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& 
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::operator=(const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>& x) {
		if (x != this) {
			clear();
			node_count = 0;
			key_comp = x.key_comp;
			if (x.root() == 0) {
				root() = 0;
				leftmost() = header;
				rightmost() = header;
			}
			else {
				root() = rb_copy(x.root(), header);
				leftmost() = minimum(root());
				rightmost() = maximum(root());
				node_count = x.node_count;
			}
		}
		return *this;
	}
	
	
	//������ֵ������ֵ�����ظ�������rbtree��������ָ�������ڵ�
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::iterator
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::insert_equal(const value_type& in) {
		link_type y = header;
		link_type x = root();
		while (x != 0) {
			y = x;
			x = key_comp(keyofvalue()(in), key(x)) ? left(x) : right(x);
		}
		return rb_insert(x, y, in);
	}
	//������ֵ������ֵ�������ظ�������pair first:rbtree��������ָ�������ڵ� second: �Ƿ����ɹ�
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	pair<typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::iterator,bool>
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::insert_unique(const value_type& v) {
		link_type y = header;
		link_type x = root();
		bool compb = true;
		while (x != 0) {
			y = x;
			compb = key_comp(keyofvalue()(v), key(x));
			x = compb ? left(x) : right(x);
		}
		// ����whileѭ����yΪ�����ĸ��ڵ�
		iterator j = iterator(y);   // ������jָ��y�ĵ�����
		if (compb) // ��ʾy�ļ�ֵ���ڲ����ֵ������������
			if (j == begin())     // ��������ĸ��ڵ�Ϊ����ڵ�
				return pair<iterator, bool>(rb_insert(x, y, v), true);
			else // �������j
				--j;
		// y�ļ�ֵС�ڲ����ֵ����������Ҳ�
		if (key_comp(key(j.node), keyofvalue()(v)))
			return pair<iterator, bool>(rb_insert(x, y, v), true);
		// ��ֵ�����м�ֵ�ظ������ز���ʧ��
		return pair<iterator, bool>(j, false);
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	template<class InputIterator>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::insert_equal(InputIterator first, InputIterator last){
		for (; first != last; ++first)
			insert_equal(*first);
	}

	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	template<class InputIterator>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::insert_unique(InputIterator first, InputIterator last) {
		for (; first != last; ++first)
			insert_unique(*first);
	}


	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::insert_equal(const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::value_type* first, const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::value_type* last){
		for (; first != last; ++first)
			insert_equal(*first);
	}

	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::insert_equal(const_iterator first, const_iterator last){
		for (; first != last; ++first)
			insert_equal(*first);
	}

	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::insert_unique(const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::value_type* first, const rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::value_type* last) {
		for (; first != last; ++first)
			insert_unique(*first);
	}

	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::insert_unique(const_iterator first, const_iterator last){
		for (; first != last; ++first)
			insert_unique(*first);
	}


	//���뺯��  xΪ����㣬yΪ����ڵ�ĸ��ڵ㣬vΪ��ֵ
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::iterator
	rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::rb_insert(base_ptr x_, base_ptr y_, const value_type& v){
		link_type x = (link_type)x_;
		link_type y = (link_type)y_;
		link_type z;
		if (y == header || x != 0 || key_comp(keyofvalue()(v), key(y))) {
			//key(v) < key(y)
			z = create_node(v);
			left(y)=z;
			if (y == header) {            //���y==header�� ��y�������ӽڵ㶼��z  rightmost()=z������������Ϊz��
				root()=z;
				rightmost()=z;
			}
			else if(y==leftmost())
				leftmost()=z;
		}
		else {
			//key(v) > key(y)  ���������ڵ���y���Ҳ�
			z = create_node(v);
			right(y) = z;
			if (y == rightmost())
				rightmost() = z;
		}
		//����Ҷ�ӽڵ�node
		parent(z) = y;
		left(z) = 0;
		right(z) = 0;
		//���������
		rb_tree_rebalance(z, header->parent);
		++node_count;
		return iterator(z);	
	}

	//erase
	//erase�ڵ�x��������
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::erase(link_type x) {
		while (x != 0) {
			erase(right(x));
			link_type y = left(x);
			destroy_node(x);
			x = y;
		}
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::erase(iterator first, iterator last) {
			if (first == begin() && last == end())
				clear();
			else
				while (first != last) erase(first++);
		}
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::erase(const key_type* first, const key_type* last) {
		while (first != last) erase(*first++);  // call erase(const T_key& x)
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	void rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::erase(iterator position) {
		link_type y =(link_type) rb_tree_rebalance_for_erase(position.node,header->parent,header->left,header->right);
		destroy_node(y);
		--node_count;
	}
	//ע��erase��key_type��insert��value_type������key,value��
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	size_t rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::erase(const key_type& x) {
		pair<iterator, iterator> p = equal_range(x);
		size_type n = 0;
		n=distance(p.first, p.second);
		erase(p.first, p.second);
		return n;
	}

	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::link_type 
	rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::rb_copy(link_type x, link_type p){
		link_type top = clone_node(x);  //��x�� value��color����
		top->parent = p;   //��p��������

		//�ݹ����top��left and right
		if (x->right)
			top->right = rb_copy(right(x), top);
		p = top;       //top�Ѿ�������x�������������Ƹ�p
		x = left(x); 
		while (x!=0)
		{
			//ͬ����x��������
			link_type y = clone_node(x);  //****���Ƶ�ǰ�������ڵ�x
			p->left = y;       //�˿�p�������������Ѿ�����
			y->parent = p;   
			if (x->right)
				y->right = rb_copy(right(x), y);
			p = y; //���¸���
			x = left(x);
		}
		return top;
	}


	//find
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::iterator
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::find(const key_type& x) {
		link_type y = header;
		link_type rt = root();
		while (rt != 0) {
			if (!key_comp(key(rt), x)) {    // key(rt)<x   ��ȡ��
				y = tr;
				rt = left(rt);
			}
			else
				rt = right(rt);
		}
		iterator j = iterator(y);
		//�丸�ڵ�Ϊheader   �ڶ������� k==key(j.node)
		return (j == end() || key_comp(k, key(j.node))) ? end() : j;
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::const_iterator
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::find(const key_type& x)const {
		link_type y = header;
		link_type rt = root();
		while (rt != 0) {
			if (!key_comp(key(rt), x)) {    // key(rt)<x   ��ȡ��
				y = rt;
				rt = left(rt);
			}
			else
				rt = right(rt);
		}
		const_iterator j = const_iterator(y);
		//�丸�ڵ�Ϊheader ��һ����������  �ڶ������� k==key(j.node)
		return (j == end() || key_comp(x, key(j.node))) ? end() : j;
	}

	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::size_type
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::count(const key_type& x)const {
		pair<const_iterator, const_iterator> p = equal_range(x);
		size_type n = 0;
		n=distance(p.first, p.second);
		return n;
	}
	
	//equal_range ���� key x �ĵ�������Χ
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	pair<typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::iterator, typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::iterator>
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::equal_range(const key_type& x) {
		return pair<iterator, iterator>(lower_bound(x), upper_bound(x));
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	pair<typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::const_iterator, typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::const_iterator>
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::equal_range(const key_type& x)const {
		return pair<const_iterator, const_iterator>(lower_bound(x), upper_bound(x));
	}

	//lower_bound���ҵ�һ�����ڻ����num�����֣��ҵ����ظ����ֵĵ�ַ
	//upper_bound���ҵ�һ������num�����֣��ҵ����ظ����ֵĵ�ַ
	//if (!key_comp(key(x),k))  ���key(x)>=k
	//if (!key_comp(k,key(x)))  ���k<key(x)
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::iterator
		typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::lower_bound(const key_type& k) {
		link_type y = header; 
		link_type x = root();
		while (x != 0)
			if (!key_comp(key(x), k))
				y = x, x = left(x);
			else
				x = right(x);

		return iterator(y);
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::const_iterator
		typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::lower_bound(const key_type& k)const {
			link_type y = header; 
			link_type x = root(); 
			while (x != 0)
				if (!key_comp(key(x), k))
					y = x, x = left(x);
				else
					x = right(x);

			return const_iterator(y);
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::iterator
		typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::upper_bound(const key_type& k) {
		link_type y = header;
		link_type x = root();
		while (x != 0)
			if (key_comp(k,key(x)))
				y = x, x = left(x);
			else
				x = right(x);

		return iterator(y);
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::const_iterator
		typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::upper_bound(const key_type& k)const {
		link_type y = header;
		link_type x = root();
		while (x != 0)
			if (key_comp(k, key(x)))
				y = x, x = left(x);
			else
				x = right(x);

		return const_iterator(y);
	}


	// ��������,����Υ�����������ʱ������ת����
	inline void rb_tree_rotate_left(rb_tree_node_base* x, rb_tree_node_base*& root) {
		rb_tree_node_base* y = x->right;//yΪ��ת������ӽڵ�
		x->right = y->left;
		if (y->left != 0)
			y->left->parent = x;
		y->parent = x->parent;

		//����x���ڵ��left�� right
		if (x == root)
			root = y;
		else if (x->parent->right == x)
			x->parent->right = y;
		else
			x->parent->left = y;

		y->left = x;
		x->parent = y;
	}
	// ��������,����Υ�����������ʱ������ת����
	inline void rb_tree_rotate_right(rb_tree_node_base* x, rb_tree_node_base*& root) {
		rb_tree_node_base* y = x->left;//yΪ��ת������ӽڵ�
		x->left = y->right;
		if (y->right != 0)
			y->right->parent = x;
		y->parent = x->parent;

		//����x���ڵ��left�� right
		if (x == root)
			root = y;
		else if (x->parent->right == x)
			x->parent->right = y;
		else
			x->parent->left = y;

		y->right = x;
		x->parent = y;
	}
	//����RB_TREE�� ��ת���ı���ɫ
	//https://zhuanlan.zhihu.com/p/79980618?utm_source=cn.wiz.note
	//pΪ����ڵ�ĸ��ڵ㣬u(y)Ϊuncle�ڵ㣬gΪ�游�ڵ�,xΪ����ڵ�(�ο��ʼǱ�)
	inline void rb_tree_rebalance(rb_tree_node_base* x, rb_tree_node_base*& root) {
		x->color = rb_tree_red;  //�½ڵ��Ϊ��ɫ
		//�������ڵ�ĸ��ڵ�Ҳ�Ǻ�ɫ������Ҫ��������������������ڵ�Ϊ��ɫ������������������ĸ�����
		while (x!=root && x->parent->color==rb_tree_red){   
			//��pΪg�����ӽڵ�
			if (x->parent == x->parent->parent->left) {
				rb_tree_node_base* y = x->parent->parent->right;//yΪuncle
				//*****************case3.1**********************
				//uncleΪ��ɫ
				if (y && y->color == rb_tree_red) {
					x->parent->color = rb_tree_black;
					y->color = rb_tree_black;
					x->parent->parent->color = rb_tree_red;
					x = x->parent->parent;//�����ظ����ϵ��������
				}
				else {
					//��uncle���� uncleΪ��
					if (x == x->parent->right) {
						//case:3.2.2
						x = x->parent;
						rb_tree_rotate_left(x, root);
					}
					//case3.2.1
					x->parent->color = rb_tree_black;
					x->parent->parent->color = rb_tree_red;
					rb_tree_rotate_right(x->parent->parent, root);
				}
			}
			else{
				//��pΪg�����ӽڵ�
				rb_tree_node_base* y = x->parent->parent->left;//yΪuncle
				if (y && y->color == rb_tree_red) {
					x->parent->color = rb_tree_black;
					y->color = rb_tree_black;
					x->parent->parent->color = rb_tree_red;
					x = x->parent->parent;//�����ظ����ϵ��������
				}
				else{
					//��uncle���� uncleΪ��
					if (x == x->parent->left) {
						//case:3.2.4
						x = x->parent;
						rb_tree_rotate_right(x, root);
					}
					//case3.2.3
					x->parent->color = rb_tree_black;
					x->parent->parent->color = rb_tree_red;
					rb_tree_rotate_left(x->parent->parent, root);
				}
			}
		}//�������
		root->color = rb_tree_black;
	}

	/*
	//ɾ���ڵ��ʹ rb tree ����ƽ�⣬����һΪҪɾ���Ľڵ㣬������Ϊ���ڵ㣬������Ϊ��С�ڵ㣬������Ϊ���ڵ�
	//https://blog.csdn.net/v_JULY_v/article/details/6109153
	inline rb_tree_node_base* rb_tree_rebalance_for_erase(rb_tree_node_base* z, rb_tree_node_base*& root, rb_tree_node_base*& leftmt, rb_tree_node_base*& rightmt) {
		// y �ǿ��ܵ��滻�ڵ㣬ָ������Ҫɾ���Ľڵ�
		auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
		// x �� y ��һ�����ӽڵ�� NIL �ڵ�
		auto x = y->left != nullptr ? y->left : y->right;
		// xp Ϊ x �ĸ��ڵ�
		rb_tree_node_base xp = nullptr;

		// y != z ˵�� z �������ǿ��ӽڵ㣬��ʱ y ָ�� z ������������ڵ㣬x ָ�� y �����ӽڵ㡣
		// �� y ���� z ��λ�ã��� x ���� y ��λ�ã������ y ָ�� z
		if (y != z)
		{
			z->left->parent = y;
			y->left = z->left;

			// ��� y ���� z �����ӽڵ㣬��ô z �����ӽڵ�һ��������
			if (y != z->right)
			{ // x �滻 y ��λ��
				xp = y->parent;
				if (x != nullptr)
					x->parent = y->parent;

				y->parent->left = x;
				y->right = z->right;
				z->right->parent = y;
			}
			else
			{
				xp = y;
			}

			// ���� y �� z �ĸ��ڵ� 
			if (root == z)
				root = y;
			else if (rb_tree_is_lchild(z))
				z->parent->left = y;
			else
				z->parent->right = y;
			y->parent = z->parent;
			MiniSTL::swap(y->color, z->color);
			y = z;
		}
		// y == z ˵�� z ����ֻ��һ������
		else
		{
			xp = y->parent;
			if (x)
				x->parent = y->parent;

			// ���� x �� z �ĸ��ڵ�
			if (root == z)
				root = x;
			else if (rb_tree_is_lchild(z))
				z->parent->left = x;
			else
				z->parent->right = x;

			// ��ʱ z �п���������ڵ�����ҽڵ㣬��������
			if (leftmt == z)
				leftmt = x == nullptr ? xp : rb_tree_min(x);
			if (rightmt == z)
				rightmt = x == nullptr ? xp : rb_tree_max(x);
		}

		// ��ʱ��y ָ��Ҫɾ���Ľڵ㣬x Ϊ����ڵ㣬�� x �ڵ㿪ʼ������
		// ���ɾ���Ľڵ�Ϊ��ɫ����������û�б��ƻ����������������������x Ϊ���ӽڵ�Ϊ������
		// case 1: �ֵܽڵ�Ϊ��ɫ����ڵ�Ϊ�죬�ֵܽڵ�Ϊ�ڣ��������ң�������������
		// case 2: �ֵܽڵ�Ϊ��ɫ���������ӽڵ㶼Ϊ��ɫ�� NIL�����ֵܽڵ�Ϊ�죬���ڵ��Ϊ��ǰ�ڵ㣬��������
		// case 3: �ֵܽڵ�Ϊ��ɫ�����ӽڵ�Ϊ��ɫ�� NIL�����ӽڵ�Ϊ��ɫ�� NIL��
		//         ���ֵܽڵ�Ϊ�죬�ֵܽڵ�����ӽڵ�Ϊ�ڣ����ֵܽڵ�Ϊ֧���ң���������������
		// case 4: �ֵܽڵ�Ϊ��ɫ�����ӽڵ�Ϊ��ɫ�����ֵܽڵ�Ϊ���ڵ����ɫ�����ڵ�Ϊ��ɫ���ֵܽڵ�����ӽڵ�
		//         Ϊ��ɫ���Ը��ڵ�Ϊ֧�����ң������������ʵ�����ɣ��㷨����
		if (!rb_tree_is_red(y))
		{ // x Ϊ��ɫʱ������������ֱ�ӽ� x ��Ϊ��ɫ����
			while (x != root && (x == nullptr || !rb_tree_is_red(x)))
			{
				if (x == xp->left)
				{ // ��� x Ϊ���ӽڵ�
					auto brother = xp->right;
					if (rb_tree_is_red(brother))
					{ // case 1
						rb_tree_set_black(brother);
						rb_tree_set_red(xp);
						rb_tree_rotate_left(xp, root);
						brother = xp->right;
					}
					// case 1 תΪΪ�� case 2��3��4 �е�һ��
					if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
						(brother->right == nullptr || !rb_tree_is_red(brother->right)))
					{ // case 2
						rb_tree_set_red(brother);
						x = xp;
						xp = xp->parent;
					}
					else
					{
						if (brother->right == nullptr || !rb_tree_is_red(brother->right))
						{ // case 3
							if (brother->left != nullptr)
								rb_tree_set_black(brother->left);
							rb_tree_set_red(brother);
							rb_tree_rotate_right(brother, root);
							brother = xp->right;
						}
						// תΪ case 4
						brother->color = xp->color;
						rb_tree_set_black(xp);
						if (brother->right != nullptr)
							rb_tree_set_black(brother->right);
						rb_tree_rotate_left(xp, root);
						break;
					}
				}
				else  // x Ϊ���ӽڵ㣬�Գƴ���
				{
					auto brother = xp->left;
					if (rb_tree_is_red(brother))
					{ // case 1
						rb_tree_set_black(brother);
						rb_tree_set_red(xp);
						rb_tree_rotate_right(xp, root);
						brother = xp->left;
					}
					if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
						(brother->right == nullptr || !rb_tree_is_red(brother->right)))
					{ // case 2
						rb_tree_set_red(brother);
						x = xp;
						xp = xp->parent;
					}
					else
					{
						if (brother->left == nullptr || !rb_tree_is_red(brother->left))
						{ // case 3
							if (brother->right != nullptr)
								rb_tree_set_black(brother->right);
							rb_tree_set_red(brother);
							rb_tree_rotate_left(brother, root);
							brother = xp->left;
						}
						// תΪ case 4
						brother->color = xp->color;
						rb_tree_set_black(xp);
						if (brother->left != nullptr)
							rb_tree_set_black(brother->left);
						rb_tree_rotate_right(xp, root);
						break;
					}
				}
			}
			if (x != nullptr)
				rb_tree_set_black(x);
		}
		return y;
	}
	*/


#endif // !_RB_TREE_H

