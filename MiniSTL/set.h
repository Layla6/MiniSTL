#ifndef _SET_H
#define _SET_H

#include "rb_tree.h"
#include "algorithm.h"
#include "type_traits.h"
#include "alloc.h"
#include "iterator.h"
#include "uninitialized_imp.h"
#include "simple_alloc.h"
#include "construct.h"
#include "funtion.h"  // less<key>

namespace MiniSTL {

	template<class T_key,class Compare=less<T_key>,class Alloc=alloc>
	class set {
	public:
		typedef T_key key_type;
		typedef T_key value_type;
		//key value使用同一个compare
		typedef Compare key_compare;
		typedef Compare value_compare;

	private:
		typedef rb_tree< key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;
		rep_type t; //红黑树实现set

	public:
		typedef typename rep_type::const_pointer          pointer;
		typedef typename rep_type::const_pointer          const_pointer;
		typedef typename rep_type::const_reference        reference;
		typedef typename rep_type::const_reference        const_reference;
		typedef typename rep_type::const_iterator         iterator;
		typedef typename rep_type::const_iterator         const_iterator;
		typedef typename rep_type::size_type              size_type;
		typedef typename rep_type::difference_type        difference_type;

		// 构造函数
		set() :t(Compare()) {};
		template <class InputIterator>
		set(InputIterator first, InputIterator last):t(Compare()){
			t.insert_unique(first, last);
		}
		template <class InputIterator>
		set(InputIterator first, InputIterator last,const Compare& comp) :t(Comp) {
			t.insert_unique(first, last);
		}
		set(const set & rhs):t(rhs.t){}
		//set(const set<T_key, Compare , Alloc>& x) : t(x.t) {}

		set<T_key, Compare, Alloc>& operator=(const set<T_key, Compare, Alloc>& x){
			t = x.t;
			return *this;
		}


	public:
		key_compare key_comp()const { return t.key_compare(); }
		value_compare value_comp()const { return t.key_compare(); }
		iterator begin() { return t.begin(); }
		iterator end() { return t.end(); }
		bool empty()const { return t.empty(); }
		size_type size()const { return t.size(); }
		size_type max_size()const { return t.max_size(); }
		void swap(set<T_key, Compare, Alloc>& x) {t.swap(x.t);}

		// insert/erase
		pair<iterator, bool> insert(const value_type& x) {
			pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
			return pair<iterator, bool>(p.first, p.second);
		}
		iterator insert(iterator position, const value_type& x) {
			typedef typename rep_type::iterator rep_iterator;
			return t.insert_unique((rep_iterator&)position, x);  // transfer to const
		}
		template <class InputIterator>
		void insert(InputIterator first, InputIterator last) {
			t.insert_unique(first, last);
		}
		void insert(const_iterator first, const_iterator last) {
			t.insert_unique(first, last);
		}
		void insert(const value_type* first, const value_type* last) {
			t.insert_unique(first, last);
		}
		void erase(iterator position) {
			typedef typename rep_type::iterator rep_iterator;
			t.erase((rep_iterator&)position);
		}
		size_type erase(const key_type& x) {
			return t.erase(x);
		}
		void erase(iterator first, iterator last) {
			typedef typename rep_type::iterator rep_iterator;
			t.erase((rep_iterator&)first, (rep_iterator&)last);
		}
		void clear() { t.clear(); }


		//set operation
		iterator find(const key_type& x) const { return t.find(x); }
		size_type count(const key_type& x) const { return t.count(x); }
		iterator lower_bound(const key_type& x) { return t.lower_bound(x); }
		iterator upper_bound(const key_type& x) { return t.upper_bound(x); }
		pair<iterator, iterator> equal_range(const key_type& x) const { return t.equal_range(x); }


		template <class K1, class C1, class A1>
		friend bool operator== (const set<K1, C1, A1>&, const set<K1, C1, A1>&);

	};


	template <class Key, class Compare, class Alloc>
	inline bool operator==(const set<Key, Compare, Alloc>& x,const set<Key, Compare, Alloc>& y) {
		return x.t == y.t;
	}



}

#endif // !_SET_H

