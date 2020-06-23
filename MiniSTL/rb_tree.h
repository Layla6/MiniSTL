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
	const rb_tree_color_type rb_tree_red = false;  //红色为0
	const rb_tree_color_type rb_tree_black = true;  //黑色为1

	//rb_tree的节点设计

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

	//rb_tree的迭代器设计
	struct rb_tree_base_iterator
	{
		typedef rb_tree_node_base::base_ptr base_ptr;
		typedef bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t difference_type;
		
		base_ptr node;  //用来与容器之间产生一个连结关系

		void increment() {
			if (node->right != 0) {
				//case1:node右节点不为空，则找该右节点的最左节点
				node = node->right;
				while (node->left != 0)
					node = node->left;
			}
			else {
				//node右节点为空
				//case2:向上寻找子节点不为父节点的右节点
				//case3:子节点为父节点的左子节点
				//case4:为根节点（该根节点无右节点）
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
			if (node->color == rb_tree_red && node->parent->parent == node)  //当前节点为header， header的right 为 mostright
				node = node->right;
			else if(node->left!=0){          //若该节点左子节点不为空，则查找该左子节点的最右子节点
				base_ptr y = node->left;
				while (y->right != 0)
					y = y->right;
				node = y;
			}
			else {
				base_ptr y = node->parent;   //向上寻找子节点为父节点的右子节点
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
		//配置内存空间
		link_type get_node() { return rb_tree_node_allocator::allocate(); }
		void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }
		//配置内存空间后构造内容
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
			destroy(&p->value);  //析构内容（new的） left和right不是new的
			put_node(p);  //释放内存
		}

	protected:
		// 用以下三个数据表现 rb tree
		link_type    header;      // 特殊节点，与根节点互为对方的父节点
		size_type   node_count;  // 节点数
		compare key_comp;   //节点间的键值大小比较准则（函数符）

		// 以下三个函数用于取得根节点，最小节点和最大节点
		link_type& root()      const { return (link_type&)header->parent; }
		link_type& leftmost()  const { return (link_type&)header->left; }
		link_type& rightmost() const { return (link_type&)header->right; }

		//取得node的成员
		static link_type& left(link_type x) { return (link_type&)(x->left); }
		static link_type& right(link_type x) { return (link_type&)(x->right); }
		static link_type& parent(link_type x) { return (link_type&)(x->parent); }
		static reference value(link_type x) { return x->value; }
		//注意(key value)是一个节点，set容器key和value是一个元素，map则为pair.first和pair.second
		static const key_type& key(link_type x) { return keyofvalue()(value(x)); }
		static color_type& color(link_type x) { return (color_type)(x->color); }

		//取得node的成员   //
		static link_type& left(base_ptr x) { return (link_type&)(x->left); }
		static link_type& right(base_ptr x) { return (link_type&)(x->right); }
		static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
		static reference value(base_ptr x) { return ((link_type)x)->value; }
		//注意(key value)是一个节点，set容器key和value是一个元素，map则为pair.first和pair.second
		//因此 set的keyofvalue为identity 键值即实值
		//map的keyofvalue为select1st，map元素的键值即pair的第一元素
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
			root() = 0;   //先将root置空，（header->parent）
			leftmost() = header;
			rightmost() = header;
		}

	public:
		rb_tree(const compare& comp = compare()):node_count(0), key_comp(comp){init();}
		//复制构造函数
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

		//赋值操作符
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
		//不允许重复
		pair<iterator, bool> insert_unique(const value_type& x);
		//允许重复
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
		size_type erase(const key_type& x);//注意erase的key_type，insert的value_type包含（key,value）
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
	
	
	//插入新值：结点键值允许重复，返回rbtree迭代器，指向新增节点
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
	//插入新值：结点键值不允许重复，返回pair first:rbtree迭代器，指向新增节点 second: 是否插入成功
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
		// 结束while循环后，y为插入点的父节点
		iterator j = iterator(y);   // 迭代器j指向y的迭代器
		if (compb) // 表示y的键值大于插入键值，则插入于左侧
			if (j == begin())     // 如果插入点的父节点为最左节点
				return pair<iterator, bool>(rb_insert(x, y, v), true);
			else // 否则调整j
				--j;
		// y的键值小于插入键值，则插入于右侧
		if (key_comp(key(j.node), keyofvalue()(v)))
			return pair<iterator, bool>(rb_insert(x, y, v), true);
		// 新值与树中键值重复，返回插入失败
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


	//插入函数  x为插入点，y为插入节点的父节点，v为新值
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
			if (y == header) {            //如果y==header， 则y的左右子节点都是z  rightmost()=z（将右子树设为z）
				root()=z;
				rightmost()=z;
			}
			else if(y==leftmost())
				leftmost()=z;
		}
		else {
			//key(v) > key(y)  所以新增节点在y的右侧
			z = create_node(v);
			right(y) = z;
			if (y == rightmost())
				rightmost() = z;
		}
		//更新叶子节点node
		parent(z) = y;
		left(z) = 0;
		right(z) = 0;
		//调整红黑树
		rb_tree_rebalance(z, header->parent);
		++node_count;
		return iterator(z);	
	}

	//erase
	//erase节点x所有子树
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
	//注意erase的key_type，insert的value_type包含（key,value）
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
		link_type top = clone_node(x);  //将x的 value和color拷贝
		top->parent = p;   //与p建立链接

		//递归更新top的left and right
		if (x->right)
			top->right = rb_copy(right(x), top);
		p = top;       //top已经复制了x的右子树，复制给p
		x = left(x); 
		while (x!=0)
		{
			//同理处理x的左子树
			link_type y = clone_node(x);  //****复制当前左子树节点x
			p->left = y;       //此刻p的左右子树都已经更新
			y->parent = p;   
			if (x->right)
				y->right = rb_copy(right(x), y);
			p = y; //向下更新
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
			if (!key_comp(key(rt), x)) {    // key(rt)<x   再取非
				y = tr;
				rt = left(rt);
			}
			else
				rt = right(rt);
		}
		iterator j = iterator(y);
		//其父节点为header   第二个条件 k==key(j.node)
		return (j == end() || key_comp(k, key(j.node))) ? end() : j;
	}
	template<class T_key, class T_value, class keyofvalue, class compare, class Alloc>
	typename rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::const_iterator
		rb_tree<T_key, T_value, keyofvalue, compare, Alloc>::find(const key_type& x)const {
		link_type y = header;
		link_type rt = root();
		while (rt != 0) {
			if (!key_comp(key(rt), x)) {    // key(rt)<x   再取非
				y = rt;
				rt = left(rt);
			}
			else
				rt = right(rt);
		}
		const_iterator j = const_iterator(y);
		//其父节点为header 第一个条件：？  第二个条件 k==key(j.node)
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
	
	//equal_range 查找 key x 的迭代器范围
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

	//lower_bound查找第一个大于或等于num的数字，找到返回该数字的地址
	//upper_bound查找第一个大于num的数字，找到返回该数字的地址
	//if (!key_comp(key(x),k))  求出key(x)>=k
	//if (!key_comp(k,key(x)))  求出k<key(x)
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


	// 左旋函数,插入违反红黑树规则时进行旋转调整
	inline void rb_tree_rotate_left(rb_tree_node_base* x, rb_tree_node_base*& root) {
		rb_tree_node_base* y = x->right;//y为旋转点的右子节点
		x->right = y->left;
		if (y->left != 0)
			y->left->parent = x;
		y->parent = x->parent;

		//更新x父节点的left或 right
		if (x == root)
			root = y;
		else if (x->parent->right == x)
			x->parent->right = y;
		else
			x->parent->left = y;

		y->left = x;
		x->parent = y;
	}
	// 右旋函数,插入违反红黑树规则时进行旋转调整
	inline void rb_tree_rotate_right(rb_tree_node_base* x, rb_tree_node_base*& root) {
		rb_tree_node_base* y = x->left;//y为旋转点的左子节点
		x->left = y->right;
		if (y->right != 0)
			y->right->parent = x;
		y->parent = x->parent;

		//更新x父节点的left或 right
		if (x == root)
			root = y;
		else if (x->parent->right == x)
			x->parent->right = y;
		else
			x->parent->left = y;

		y->right = x;
		x->parent = y;
	}
	//调整RB_TREE， 旋转及改变颜色
	//https://zhuanlan.zhihu.com/p/79980618?utm_source=cn.wiz.note
	//p为插入节点的父节点，u(y)为uncle节点，g为祖父节点,x为插入节点(参考笔记本)
	inline void rb_tree_rebalance(rb_tree_node_base* x, rb_tree_node_base*& root) {
		x->color = rb_tree_red;  //新节点必为红色
		//如果插入节点的父节点也是红色，则需要调整红黑树，否则若父节点为黑色，则无需调整，满足四个条件
		while (x!=root && x->parent->color==rb_tree_red){   
			//若p为g的左子节点
			if (x->parent == x->parent->parent->left) {
				rb_tree_node_base* y = x->parent->parent->right;//y为uncle
				//*****************case3.1**********************
				//uncle为红色
				if (y && y->color == rb_tree_red) {
					x->parent->color = rb_tree_black;
					y->color = rb_tree_black;
					x->parent->parent->color = rb_tree_red;
					x = x->parent->parent;//迭代重复向上调整红黑树
				}
				else {
					//无uncle或者 uncle为黑
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
				//若p为g的右子节点
				rb_tree_node_base* y = x->parent->parent->left;//y为uncle
				if (y && y->color == rb_tree_red) {
					x->parent->color = rb_tree_black;
					y->color = rb_tree_black;
					x->parent->parent->color = rb_tree_red;
					x = x->parent->parent;//迭代重复向上调整红黑树
				}
				else{
					//无uncle或者 uncle为黑
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
		}//迭代完成
		root->color = rb_tree_black;
	}

	/*
	//删除节点后使 rb tree 重新平衡，参数一为要删除的节点，参数二为根节点，参数三为最小节点，参数四为最大节点
	//https://blog.csdn.net/v_JULY_v/article/details/6109153
	inline rb_tree_node_base* rb_tree_rebalance_for_erase(rb_tree_node_base* z, rb_tree_node_base*& root, rb_tree_node_base*& leftmt, rb_tree_node_base*& rightmt) {
		// y 是可能的替换节点，指向最终要删除的节点
		auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
		// x 是 y 的一个独子节点或 NIL 节点
		auto x = y->left != nullptr ? y->left : y->right;
		// xp 为 x 的父节点
		rb_tree_node_base xp = nullptr;

		// y != z 说明 z 有两个非空子节点，此时 y 指向 z 右子树的最左节点，x 指向 y 的右子节点。
		// 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后用 y 指向 z
		if (y != z)
		{
			z->left->parent = y;
			y->left = z->left;

			// 如果 y 不是 z 的右子节点，那么 z 的右子节点一定有左孩子
			if (y != z->right)
			{ // x 替换 y 的位置
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

			// 连接 y 与 z 的父节点 
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
		// y == z 说明 z 至多只有一个孩子
		else
		{
			xp = y->parent;
			if (x)
				x->parent = y->parent;

			// 连接 x 与 z 的父节点
			if (root == z)
				root = x;
			else if (rb_tree_is_lchild(z))
				z->parent->left = x;
			else
				z->parent->right = x;

			// 此时 z 有可能是最左节点或最右节点，更新数据
			if (leftmt == z)
				leftmt = x == nullptr ? xp : rb_tree_min(x);
			if (rightmt == z)
				rightmt = x == nullptr ? xp : rb_tree_max(x);
		}

		// 此时，y 指向要删除的节点，x 为替代节点，从 x 节点开始调整。
		// 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
		// case 1: 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
		// case 2: 兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，父节点成为当前节点，继续处理
		// case 3: 兄弟节点为黑色，左子节点为红色或 NIL，右子节点为黑色或 NIL，
		//         令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理
		// case 4: 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
		//         为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束
		if (!rb_tree_is_red(y))
		{ // x 为黑色时，调整，否则直接将 x 变为黑色即可
			while (x != root && (x == nullptr || !rb_tree_is_red(x)))
			{
				if (x == xp->left)
				{ // 如果 x 为左子节点
					auto brother = xp->right;
					if (rb_tree_is_red(brother))
					{ // case 1
						rb_tree_set_black(brother);
						rb_tree_set_red(xp);
						rb_tree_rotate_left(xp, root);
						brother = xp->right;
					}
					// case 1 转为为了 case 2、3、4 中的一种
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
						// 转为 case 4
						brother->color = xp->color;
						rb_tree_set_black(xp);
						if (brother->right != nullptr)
							rb_tree_set_black(brother->right);
						rb_tree_rotate_left(xp, root);
						break;
					}
				}
				else  // x 为右子节点，对称处理
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
						// 转为 case 4
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

