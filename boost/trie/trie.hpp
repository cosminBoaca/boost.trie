#ifndef BOOST_TRIE_HPP
#define BOOST_TRIE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif


#include <map>
#include <iterator>
#include <utility>
#include <cstdio>
#include <memory>
#include <iostream>
#include <stack>
#include <vector>
#include <list>
#include <boost/utility.hpp>


namespace boost { namespace tries {

namespace detail {

template <typename Key, typename Value, class Compare>
struct trie_node;

struct list_node_base : protected boost::noncopyable {
	typedef list_node_base *base_ptr;
	base_ptr pred;
	base_ptr next;

	list_node_base() : pred(0), next(0)
	{}
};

template <typename Key, typename Value, class Compare>
struct value_list_node : public list_node_base {
	typedef Key key_type;
	typedef Value value_type;
	typedef trie_node<key_type, value_type, Compare> trie_node_type;
	typedef trie_node_type * trie_node_ptr;
	typedef value_list_node<key_type, value_type, Compare> node_type;
	typedef node_type * node_ptr;
	typedef list_node_base * base_ptr;
	value_type value;
	trie_node_ptr node_in_trie;
	explicit value_list_node() : value(), node_in_trie(0)
	{	
	}

	explicit value_list_node(const value_type& x) : value(x), node_in_trie(0)
	{
	}

	/*
private:
	explicit value_list_node(const node_type&)
	{

	}

	node_type& operator=(const node_type&)
	{
	}
	*/
};



template <typename Key, typename Value, class Compare>
struct trie_node : private boost::noncopyable {
//protected:
	typedef Key key_type;
	typedef key_type* key_ptr;
	typedef Value value_type;
	typedef value_type * value_ptr;
	typedef size_t size_type;
	typedef trie_node<key_type, value_type, Compare> node_type;
	typedef node_type* node_ptr;
	typedef value_list_node<key_type, value_type, Compare> value_list_type;
	typedef value_list_type * value_list_ptr;
	// maybe the pointer container of children could be defined by user?!
	typedef std::map<key_type, node_ptr, Compare> children_type;

	typedef typename children_type::iterator child_iter;

	children_type child;

//public:
	node_ptr parent;
	// store the iterator to optimize operator++ and operator--
	// utilize that the iterator in map does not change after insertion
	child_iter child_iter_of_parent;

	// it is used for something like count_prefix 
	//size_type node_count;
	size_type value_count;
	size_type self_value_count;
	//value_ptr value;
	value_list_ptr value_list_header;
	value_list_ptr value_list_tail;

	//value_list_ptr leftmost_value_node;
	//value_list_ptr rightmost_value_node;

	explicit trie_node() : parent(0), value_count(0), self_value_count(0), 
	value_list_header(0), value_list_tail(0)//, leftmost_value_node(0), rightmost_value_node(0)
	{
	}

	const key_type& key_elem() const
	{
		return child_iter_of_parent->first;
	}

	size_type count() const
	{
		return self_value_count;
	}

	bool no_value() const
	{
		return self_value_count == 0;
	}

	/*
private:
	explicit trie_node(const node_type&)
	{
	}

	node_type& operator=(const node_type&)
	{
	}
	*/

};

/*
template <typename Key, class Compare>
struct trie_node<Key, void, Compare> {
	typedef Key key_type;
	typedef key_type* key_ptr;
	typedef void value_type;
	typedef value_type * value_ptr;
	typedef size_t size_type;
	typedef trie_node<key_type, value_type, Compare> node_type;
	typedef node_type* node_ptr;
	// maybe the pointer container of children could be defined by user?!
	typedef std::map<key_type, node_ptr, Compare> children_type;

	typedef typename children_type::iterator child_iter;

	children_type child;

//public:
	node_ptr parent;
	child_iter child_iter_of_parent;

	size_type value_count;
	size_type self_value_count;

	explicit trie_node() : parent(0), value_count(0), self_value_count(0)
	{
	}

	const key_type& key_elem() const
	{
		return child_iter_of_parent->first;
	}

	size_type count() const
	{
		return self_value_count;
	}

	bool no_value() const
	{
		return self_value_count == 0;
	}

private:
	explicit trie_node(const node_type&)
	{
	}

	node_type& operator=(const node_type&)
	{
	}
};
*/



template <typename Key, typename Value, typename Reference, typename Pointer, class Compare>
struct trie_iterator
{
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef Key key_type;
	typedef Value value_type;
	typedef Reference ref;
	typedef Pointer ptr;
	typedef trie_iterator<Key, Value, Value&, Value*, Compare> iterator;
	typedef trie_iterator<Key, Value, Reference, Pointer, Compare> iter_type;
	typedef iter_type self;
	typedef trie_iterator<Key, Value, const Value&, const Value*, Compare> const_iterator;
	typedef trie_node<Key, Value, Compare> trie_node_type;
	typedef trie_node_type* trie_node_ptr;
	typedef value_list_node<Key, Value, Compare> value_node_type;
	typedef value_node_type* value_node_ptr;

	trie_node_ptr tnode;
	value_node_ptr vnode;

public:
	explicit trie_iterator() : tnode(0), vnode(0)
	{
	}

	trie_iterator(trie_node_ptr x) : tnode(x), vnode(x->value_list_header)
	{
	}

	trie_iterator(value_node_ptr x) : tnode(x->node_in_trie), vnode(x)
	{
	}

	explicit trie_iterator(trie_node_ptr t, value_node_ptr v) : tnode(t), vnode(v)
	{
	}

	trie_iterator(const iterator &it) : tnode(it.tnode), vnode(it.vnode)
	{
	}

	/*
	 *
	 * a function returns the key on the path should be invented
	 *
	 */
	std::vector<key_type> get_key()
	{
		std::vector<key_type> key_path;
		trie_node_ptr cur = tnode;
		while (cur->parent != NULL)
		{
			key_path.push_back(cur->key_elem());
			cur = cur->parent;
		}
		return std::vector<key_type>(key_path.rbegin(), key_path.rend());
	}

	/*
	 * should have a version that copy the path to a parameter
	std::list<key_type> get_key()
	{
		std::list<key_type> key_path;
		trie_node_ptr cur = tnode;
		while (cur->parent != NULL)
		{
			key_path.push_front(cur->key_elem());
			cur = cur->parent;
		}
		return key_path;
	}
	*/

	/*
	template<typename Container>
	Container<key_type> get_key(Container<key_type>& container)
	{
		
	}
	*/

	ref operator*() const 
	{
		return vnode->value;
	}

	ptr operator->() const
	{
		return &(operator*()); 
	}

	bool operator==(const trie_iterator& other) const
	{
		return tnode == other.tnode && vnode == other.vnode;
	}

	bool operator!=(const trie_iterator& other) const
	{
		return tnode != tnode || vnode != other.vnode;
	}

	void trie_node_increment()
	{
		// at iterator end
		if (tnode->parent == NULL)
			return;
		trie_node_ptr cur = tnode;
		if (!cur->child.empty())
		{ // go down to the first node with a value in it, and there always be at least one
			do {
				cur = cur->child.begin()->second;
			} while (cur->no_value());
			tnode = cur;
		} else {
			// go up till there is a sibling next to cur
			// the algorithm here is not so efficient
			while (cur->parent != NULL)
			{
				trie_node_ptr p = cur->parent;
				typename trie_node_type::child_iter ci = cur->child_iter_of_parent;
				++ci;
				if (ci != p->child.end())
				{
					cur = ci->second;
					//"change value to self_value_count
					while (cur->no_value()) {
						cur = cur->child.begin()->second;
					}
					break;
				}
				cur = p;
			}
			tnode = cur;
		}
	}

	void trie_node_decrement()
	{
		trie_node_ptr cur = tnode;
		// handle the decrement of end()
		if (cur->parent == NULL)
		{
			while (!cur->child.empty())
			{
				cur = cur->child.rbegin()->second;
			}
			tnode = cur;
			return;
		}
		trie_node_ptr p = cur->parent;
		typename trie_node_type::child_iter ci = cur->child_iter_of_parent;
		while (p != NULL && ci == p->child.begin() && p->no_value())
		{
			cur = p;
			p = cur->parent;
			ci = cur->child_iter_of_parent;
		}
		// root
		if (p == NULL)
		{
			tnode = cur;
			return;
		}
		// go down the trie
		if (ci != p->child.begin())
		{
			--ci;
			cur = ci->second;
			while (!cur->child.empty())
			{
				cur = cur->child.rbegin()->second;
			}
			tnode = cur;
			return;
		}
		// to parent which p->no_value == true
		tnode = p;
	}

	void increment()
	{
		if (vnode != NULL && vnode->next != NULL)
		{
			vnode = static_cast<value_node_ptr>(vnode->next);
			return;
		}
		trie_node_increment();
		vnode = tnode->value_list_header;
	}

	void decrement()
	{
		if (vnode != NULL && vnode->pred != NULL)
		{
			vnode = static_cast<value_node_ptr>(vnode->pred);
			return;
		}
		trie_node_decrement();
		vnode = tnode->value_list_tail;
	}

	self& operator++() 
	{
		increment();
		// increment
		return *this;
	}
	self operator++(int)
	{
		self tmp = *this;
		increment();
		// increment
		return tmp;
	}

	self& operator--()
	{
		// decrement
		decrement();
		return *this;
	}
	self operator--(int)
	{
		self tmp = *this;
		decrement();
		return tmp;
	}
}; 

template <typename Key, typename Value, typename Reference, typename Pointer, class Compare>
struct trie_reverse_iterator : public trie_iterator<Key, Value, Reference, Pointer, Compare> {
	typedef trie_iterator<Key, Value, Reference, Pointer, Compare> iterator;
	typedef typename iterator::const_iterator const_iterator;
	typedef trie_reverse_iterator<Key, Value, Value&, Value*, Compare> reverse_iterator;
	typedef trie_reverse_iterator<Key, Value, Reference, Pointer, Compare> iter_type;
	typedef iter_type self;
	typedef trie_reverse_iterator<Key, Value, const Value&, const Value*, Compare> const_reverse_iterator;
	typedef trie_node<Key, Value, Compare> trie_node_type;
	typedef trie_node_type* trie_node_ptr;
	typedef value_list_node<Key, Value, Compare> value_node_type;
	typedef value_node_type* value_node_ptr;

public:
	explicit trie_reverse_iterator() : iterator()
	{
	}

	trie_reverse_iterator(trie_node_ptr x) : iterator(x)
	{
	}

	trie_reverse_iterator(value_node_ptr x) : iterator(x)
	{
	}

	explicit trie_reverse_iterator(trie_node_ptr t, value_node_ptr v) : iterator(t, v)
	{
	}

	trie_reverse_iterator(iterator it) : iterator(it)
	{

	}
	trie_reverse_iterator(const reverse_iterator &it) : iterator(it)
	{
	}

	self& operator++() 
	{
		iterator::decrement();
		// increment
		return *this;
	}
	self operator++(int)
	{
		self tmp = *this;
		iterator::decrement();
		// increment
		return tmp;
	}

	self& operator--()
	{
		// decrement
		iterator::increment();
		return *this;
	}
	self operator--(int)
	{
		self tmp = *this;
		iterator::increment();
		return tmp;
	}
};



/*
template <typename Key, typename Reference, typename Pointer, class Compare>
struct trie_iterator<Key, void, Reference, Pointer, Compare> {
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef Key key_type;
	typedef void value_type;
	typedef Reference ref;
	typedef Pointer ptr;
	//void does not allow const and reference
	typedef trie_iterator<Key, value_type, value_type, value_type, Compare> iterator;
	typedef trie_iterator<Key, value_type, value_type, value_type, Compare> iter_type;
	typedef iterator self;
	//void does not allow const and reference
	typedef trie_iterator<Key, value_type, value_type, value_type, Compare> const_iterator;
	typedef trie_node<Key, value_type, Compare> node_type;
	typedef node_type* node_ptr;

	node_ptr node;

public:
	explicit trie_iterator() : node(0)
	{
	}

	trie_iterator(node_ptr x) : node(x) 
	{
	}

	trie_iterator(const iterator &it) : node(it.node)
	{
	}

	std::vector<key_type> get_key()
	{
		std::vector<key_type> key_path;
		node_ptr cur = node;
		while (cur->parent != NULL)
		{
			key_path.push_back(cur->key_elem());
			cur = cur->parent;
		}
		return std::vector<key_type>(key_path.rbegin(), key_path.rend());
	}


private:
	ref operator*() const 
	{
		return node->value;
	}

	ptr operator->() const
	{
		return &(operator*()); 
	}

public:
	bool operator==(const trie_iterator& other) const
	{
		return node == other.node;
	}

	bool operator!=(const trie_iterator& other) const
	{
		return node != other.node;
	}

	void increment()
	{
		// at iterator end
		if (node->parent == NULL)
			return;
		node_ptr cur = node;
		if (!cur->child.empty())
		{ // go down to the first node with a value in it, and there always be at least one
			do {
				cur = cur->child.begin()->second;
			} while (cur->no_value());
			node = cur;
		} else {
			// go up till there is a sibling next to cur
			// the algorithm here is not so efficient
			while (cur->parent != NULL)
			{
				node_ptr p = cur->parent;
				typename node_type::child_iter ci = cur->child_iter_of_parent;
				++ci;
				if (ci != p->child.end())
				{
					cur = ci->second;
					"change value to self_value_count
					while (cur->no_value()) {
						cur = cur->child.begin()->second;
					}
					break;
				}
				cur = p;
			}
			node = cur;
		}
	}

	void decrement()
	{
		node_ptr cur = node;
		// handle the decrement of end()
		if (cur->parent == NULL)
		{
			while (!cur->child.empty())
			{
				cur = cur->child.rbegin()->second;
			}
			node = cur;
			return;
		}
		node_ptr p = cur->parent;
		typename node_type::child_iter ci = cur->child_iter_of_parent;
		while (p != NULL && ci == p->child.begin() && p->no_value())
		{
			cur = p;
			p = cur->parent;
			ci = cur->child_iter_of_parent;
		}
		// root
		if (p == NULL)
		{
			node = cur;
			return;
		}
		// go down the trie
		if (ci != p->child.begin())
		{
			--ci;
			cur = ci->second;
			while (!cur->child.empty())
			{
				cur = cur->child.rbegin()->second;
			}
			node = cur;
			return;
		}
		// to parent which p->no_value == true
		node = p;
	}

	self& operator++() 
	{
		increment();
		// increment
		return *this;
	}
	self operator++(int)
	{
		self tmp = *this;
		increment();
		// increment
		return tmp;
	}

	self& operator--()
	{
		// decrement
		decrement();
		return *this;
	}
	self operator--(int)
	{
		self tmp = *this;
		decrement();
		return tmp;
	}
};
*/

} // namespace detail



template <typename Key, typename Value,
		 class Compare>
class trie {
public:
	typedef Key key_type;
	typedef key_type * key_ptr;
	typedef Value value_type;
	typedef value_type* value_ptr;
	typedef trie<key_type, value_type, Compare> trie_type;
	typedef typename detail::trie_node<key_type, value_type, Compare> node_type;
	typedef node_type * node_ptr;
	typedef typename detail::value_list_node<key_type, value_type, Compare> value_node_type;
	typedef value_node_type * value_node_ptr;

	typedef std::allocator< node_type > trie_node_allocator;
	typedef std::allocator< value_node_type > value_allocator;
	typedef size_t size_type;

private:
	trie_node_allocator trie_node_alloc;
	value_allocator value_alloc;

	node_ptr root;
	size_type node_count; // node_count is difficult and useless to maintain on each node, so, put it on the tree

	value_node_ptr new_value_node(const value_type& x)
	{
		value_node_ptr v = value_alloc.allocate(1);
		if (v == NULL)
			return v;
		new(v) value_node_type(x);
		return v;
	}

	bool delete_value_node(value_node_ptr p)
	{
		if (!p)
			return false;
		--p->node_in_trie->self_value_count;
		value_alloc.destroy(p);
		value_alloc.deallocate(p, 1);
		return true;
	}

	void erase_value_list(node_ptr cur)
	{
		value_node_ptr vp = cur->value_list_header;
		if (!cur->no_value())
		{
			while (vp != NULL)
			{
				value_node_ptr tmp = static_cast<value_node_ptr>(vp->next);
				delete_value_node(vp);
				vp = tmp;
			}
		}
		cur->self_value_count = 0;
		cur->value_list_header = cur->value_list_tail = NULL;
	}

	node_ptr get_trie_node() 
	{
		node_ptr new_node = trie_node_alloc.allocate(1);
		if (new_node != NULL)
			++node_count; 
		return new_node;
	}

	// that function seems bad, it is only for initializing root
	node_ptr create_root() 
	{
		node_ptr tmp = get_trie_node();
		if (tmp != NULL)
		{
			new(tmp) node_type();
		}
		return tmp;
	}

	node_ptr create_trie_node() 
	{
		node_ptr tmp = get_trie_node();
		if (tmp != NULL)
		{
			new(tmp) node_type();
		}
		return tmp;
	}

	node_ptr create_trie_node(const value_type& value) 
	{
		node_ptr tmp = get_trie_node();
		if (tmp != NULL)
		{
			new(tmp) node_type();
			value_node_ptr vn = new_value_node(value); 
			value_list_push(tmp, vn);
		}
		return tmp;
	}

	void value_list_push(node_ptr tmp, value_node_ptr vn)
	{
		vn->node_in_trie = tmp;
		vn->next = tmp->value_list_header;
		if (tmp->value_list_header != NULL)
		{
			tmp->value_list_header->pred = vn;
		}
		else {
			// empty list
			tmp->value_list_tail = vn;
		}
		tmp->value_list_header = vn;
		++tmp->self_value_count;
	}

	node_ptr create_trie_node(value_node_ptr vl_header)
	{
		node_ptr tmp = get_trie_node();
		if (tmp != NULL)
		{
			new(tmp) node_type();
			while (vl_header != NULL)
			{
				value_node_ptr vn = new_value_node(vl_header->value); 
				value_list_push(tmp, vn);
				vl_header = static_cast<value_node_ptr>(vl_header->next);
			}
		}
		return tmp;
	}

	bool delete_trie_node(node_ptr p)
	{
		if (p == NULL)
			return false;
		erase_value_list(p);
		trie_node_alloc.destroy(p);
		trie_node_alloc.deallocate(p, 1);
		node_count--;
		return true;
	}

	// need constant time to get leftmost
	node_ptr leftmost_node(node_ptr node) const
	{
		node_ptr cur = node;
		while (!cur->child.empty() && cur->no_value())
		{
			cur = cur->child.begin()->second;
		}
		return cur;
	}

	value_node_ptr leftmost_value(node_ptr node) const
	{
		node = leftmost_node(node);
		return static_cast<value_node_ptr>(node->value_list_header);
	}

	// need constant time to get rightmost
	node_ptr rightmost_node(node_ptr node) const
	{
		node_ptr cur = node;
		while (!cur->child.empty())
		{
			cur = cur->child.rbegin()->second;
		}
		return cur;
	}

	value_node_ptr rightmost_value(node_ptr node) const
	{
		node = rightmost_node(node);
		return static_cast<value_node_ptr>(node->value_list_tail);
	}

	// copy the whole trie tree
	void copy_tree(node_ptr other_root)
	{
		if (other_root == root)
			return;

		clear();

		std::stack<node_ptr> other_node_stk, self_node_stk;
		std::stack<typename node_type::child_iter> ci_stk;
		other_node_stk.push(other_root);
		self_node_stk.push(root);
		ci_stk.push(other_root->child.begin());
		for (; !other_node_stk.empty(); )
		{
			node_ptr other_cur = other_node_stk.top();
			node_ptr self_cur = self_node_stk.top();
			if (ci_stk.top() == other_cur->child.end())
			{
				other_node_stk.pop();
				ci_stk.pop();
				self_node_stk.pop();
			} else {
				node_ptr c = ci_stk.top()->second;
				// create new node
				node_ptr new_node;
				if (!c->no_value())
				{
					new_node = create_trie_node(c->value_list_header);
				}
				else {
					new_node = create_trie_node();
				}
				new_node->self_value_count = c->self_value_count;
				new_node->value_count = c->value_count;
				new_node->parent = self_cur;
				new_node->child_iter_of_parent = self_cur->child.insert(std::make_pair(ci_stk.top()->first, new_node)).first;
				// to next node
				++ci_stk.top();
				other_node_stk.push(c);
				ci_stk.push(c->child.begin());
				self_node_stk.push(new_node);
			}
		}
		root->value_count = other_root->value_count;
		root->self_value_count = other_root->self_value_count;
	}

public:
	// iterators still unavailable here

	explicit trie() : trie_node_alloc(), value_alloc(), root(create_root()), node_count(0)/*, value_count(0) */
	{
	}

	explicit trie(const trie_type& t) : trie_node_alloc(), value_alloc(), root(create_root()), node_count(0)/* , value_count(0) */
	{
		copy_tree(t.root);
	}

	trie_type& operator=(const trie_type& t)
	{
		copy_tree(t.root);
		return *this;
	}


	typedef detail::trie_iterator<Key, Value, Value&, Value*, Compare> iterator;
	typedef typename iterator::const_iterator const_iterator;
	typedef detail::trie_reverse_iterator<Key, Value, Value&, Value*, Compare> reverse_iterator;
	typedef typename reverse_iterator::const_reverse_iterator const_reverse_iterator;
	typedef std::pair<iterator, bool> pair_iterator_bool;
	typedef std::pair<iterator, iterator> iterator_range;

	iterator begin() 
	{
		value_node_ptr vp = leftmost_value(root);
		if (vp == NULL)
			return root;
		else return vp;
	}

	const_iterator begin() const
	{
		value_node_ptr vp = leftmost_value(root);
		if (vp == NULL)
			return root;
		else return vp;
	}

	const_iterator cbegin() const
	{
		value_node_ptr vp = leftmost_value(root);
		if (vp == NULL)
			return root;
		else return vp;
	}

	iterator end() 
	{
		return root;
	}

	const_iterator end() const
	{
		return root;
	}

	const_iterator cend() const
	{
		return root;
	}

	reverse_iterator rbegin() 
	{
		value_node_ptr vp = rightmost_value(root);
		if (vp == NULL)
			return root;
		else return vp;
	}

	const_reverse_iterator rbegin() const
	{
		value_node_ptr vp = rightmost_value(root);
		if (vp == NULL)
			return root;
		else return vp;
	}

	const_reverse_iterator crbegin() const
	{
		return rbegin();
	}

	reverse_iterator rend() 
	{
		return root;
	}

	const_reverse_iterator rend() const
	{
		return root;
	}

	const_reverse_iterator crend() const
	{
		return rend();
	}

	template<typename Iter>
		iterator __insert(node_ptr cur, Iter first, Iter last,
				const value_type& value)
		{
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				node_ptr new_node = create_trie_node();
				new_node->parent = cur;
				typename node_type::child_iter ci = cur->child.insert(std::make_pair(cur_key, new_node)).first;
				new_node->child_iter_of_parent = ci;
				cur = ci->second;
			}
			// insert the new value node into value_list
			value_node_ptr vn = new_value_node(value);
			value_list_push(cur, vn);

			// update value_count on the path
			node_ptr tmp = cur;
			while (tmp != NULL) // until root
			{
				++tmp->value_count;
				tmp = tmp->parent;
			}

			return cur->value_list_header;
		}

	template<typename Iter>
		pair_iterator_bool insert_unique(Iter first, Iter last,
				const value_type& value)
		{
			node_ptr cur = root;
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::child_iter ci = cur->child.find(cur_key);
				if (ci == cur->child.end())
				{
					return std::make_pair(__insert(cur, first, last, value), true);
				}
				cur = ci->second;
			}
			if (cur->no_value())
			{
				return std::make_pair(__insert(cur, first, last, value), true);
			}

			return std::make_pair((iterator)cur, false);
		}

	template<typename Container>
		pair_iterator_bool insert_unique(const Container &container, const value_type& value)
		{
			return insert_unique(container.begin(), container.end(), value);
		}

	template<typename Iter>
		iterator insert_equal(Iter first, Iter last,
				const value_type& value)
		{
			node_ptr cur = root;
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::child_iter ci = cur->child.find(cur_key);
				if (ci == cur->child.end())
				{
					return __insert(cur, first, last, value);
				}
				cur = ci->second;
			}
			return __insert(cur, first, last, value);
		}

	template<typename Container>
		iterator insert_equal(const Container &container, const value_type& value)
		{
			return insert_equal(container.begin(), container.end(), value);
		}

	template<typename Iter>
		node_ptr find_node(Iter first, Iter last)
		{
			node_ptr cur = root;
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::child_iter ci = cur->child.find(cur_key);
				if (ci == cur->child.end())
				{
					return NULL;
				}
				cur = ci->second;
			}
			return cur;
		}

	template<typename Container>
		node_ptr find_node(const Container &container)
		{
			return find_node(container.begin(), container.end());
		}

	template<typename Iter>
		iterator find(Iter first, Iter last)
		{
			node_ptr node = find_node(first, last);
			if (node == NULL || node->no_value())
				return end();
			return node;
		}

	template<typename Container>
		iterator find(const Container &container)
		{
			return find(container.begin(), container.end());
		}

	// count
	template<typename Iter>
		size_type count(Iter first, Iter last)
		{
			node_ptr node = find_node(first, last);
			if (node == NULL || node->no_value())
				return 0;
			return node->count();
		}

	template<typename Container>
		size_type count(const Container &container)
		{
			return count(container.begin(), container.end());
		}

	// find by prefix, return a pair of iterator(begin, end)
	template<typename Iter>
		iterator_range find_prefix(Iter first, Iter last)
		{
			node_ptr node = find_node(first, last);
			if (node == NULL)
			{
				return make_pair(end(), end());
			}
			iterator begin = leftmost_value(node);
			// optimization is needed here
			iterator end = rightmost_value(node);
			++end;
			return make_pair(begin, end);
		}

	template<typename Container>
		iterator_range find_prefix(const Container &container)
		{
			return find_prefix(container.begin(), container.end());
		}

	// count_prefix() to count values with the same prefix
	template<typename Iter>
		size_type count_prefix(Iter first, Iter last)
		{
			node_ptr node = find_node(first, last);
			if (node == NULL)
			{
				return 0;
			}
			return node->value_count;
		}

	template<typename Container>
		size_type count_prefix(const Container &container)
		{
			return count_prefix(container.begin(), container.end());
		}

	// upper_bound() to find the first node that greater than the key
	template<typename Iter>
		node_ptr upper_bound(Iter first, Iter last)
		{
			node_ptr cur = root;
			// use a stack to store iterator in order to avoid the iterator cannot go backward
			std::stack< Iter > si;
			for (; first != last; ++first)
			{
				si.push(first);
				const key_type& cur_key = *first;
				typename node_type::child_iter ci = cur->child.find(cur_key);
				// using upper_bound needs comparison in every step, so using find until ci == NULL
				if (ci == cur->child.end())
				{
					// find a node that
					ci = cur->child.upper_bound(cur_key);
					si.pop();
					while (ci == cur->child.end())
					{
						if (cur->parent == NULL)
							return root;
						cur = cur->parent;
						ci = cur->child.upper_bound(*si.top());
					}
					cur = ci->second;
					while (cur->no_value())
					{
						cur = cur->child.begin()->second;
					}
					return cur;
				}
				cur = ci->second;
			}
			// if find a full match, then increment it
			iterator tmp(cur);
			//++tmp;
			tmp.trie_node_increment();
			cur = tmp.tnode;

			return cur;
		}

	template<typename Container>
		node_ptr upper_bound(const Container &container)
		{
			return upper_bound(container.begin(), container.end());
		}

	// lower_bound()
	template<typename Iter>
		node_ptr lower_bound(Iter first, Iter last)
		{
			node_ptr cur = root;
			// use a stack to store iterator in order to avoid the iterator cannot go backward
			std::stack< Iter > si;
			for (; first != last; ++first)
			{
				si.push(first);
				const key_type& cur_key = *first;
				typename node_type::child_iter ci = cur->child.find(cur_key);
				// using upper_bound needs comparison in every step, so using find until ci == NULL
				if (ci == cur->child.end())
				{
					// find a node that
					ci = cur->child.upper_bound(cur_key);
					si.pop();
					while (ci == cur->child.end())
					{
						if (cur->parent == NULL)
							return root;
						cur = cur->parent;
						ci = cur->child.upper_bound(*si.top());
					}
					cur = ci->second;
					while (cur->no_value())
					{
						cur = cur->child.begin()->second;
					}
					return cur;
				}
				cur = ci->second;
			}
			// lower_bound() needn't increment here!!!
			return cur;
		}

	template<typename Container>
		node_ptr lower_bound(const Container &container)
		{
			return lower_bound(container.begin(), container.end());
		}

	// equal_range() is the same as find_prefix? the meaning is different
	template<typename Iter>
		iterator_range equal_range(Iter first, Iter last)
		{
			return make_pair(lower_bound(first, last), upper_bound(first, last));
		}

	template<typename Container>
		iterator_range equal_range(const Container &container)
		{
			return equal_range(container.begin(), container.end());
		}

	void erase_check_ancestor(node_ptr cur, size_type delta) // delete empty ancestors and update value_count
	{
		while (cur != root && cur->child.empty() && cur->no_value())
		{
			node_ptr parent = cur->parent;
			parent->child.erase(cur->child_iter_of_parent);
			delete_trie_node(cur);
			cur = parent;
		}

		// update value_count on each ancestral node
		while (cur != NULL)
		{
			cur->value_count -= delta;
			cur = cur->parent;
		}
	}

public:
	//erase one node with value, and erase empty ancestors
	size_type erase_node(node_ptr node)
	{
		size_type ret = 0;
		if (node == NULL)
			return ret;
		ret = node->self_value_count;
		node_ptr cur = node;
		erase_value_list(cur);

		erase_check_ancestor(cur, ret);

		return ret;
	}

	// erase one value, after erasing value, check if it is necessary to erase node
	iterator erase(iterator it)
	{
		if (it == end())
			return it;
		iterator ret = it;
		++ret;
		value_node_ptr vp = it.vnode;
		node_ptr cur = it.tnode;
		if (vp->next == NULL && vp->pred == NULL)
		{
			erase_node(cur);
		} else {
			if (vp->pred)
			{
				vp->pred->next = vp->next;
			}
			else { // is value_list_header
				cur->value_list_header = static_cast<value_node_ptr>(vp->next);
			}
			if (vp->next)
			{
				vp->next->pred = vp->pred;
			}
			else { // is value_list_tail
				cur->value_list_tail = static_cast<value_node_ptr>(vp->pred);
			}
			// some value should be changed here
			delete_value_node(vp);

			// update value_count on each ancestral node
			node_ptr tmp = cur;
			while (tmp != NULL)
			{
				--tmp->value_count;
				tmp = tmp->parent;
			}
		}
		return ret;
	}

	iterator erase(const_iterator it)
	{
		return erase(iterator(it.tnode, it.vnode));
	}

	template<typename Iter>
		iterator erase(Iter first, Iter last)
		{
			iterator it = find(first, last);
			return erase(it);

		}

	template<typename Container>
		iterator erase(const Container &container)
		{
			return erase(container.begin(), container.end());
		}

	template<typename Iter>
		size_type erase_node(Iter first, Iter last)
		{
			return erase_node(find_node(first, last));
		}

	template<typename Container>
		size_type erase_node(const Container &container)
		{
			return erase_node(container.begin(), container.end());
		}

	// erase a range of iterators
	void erase(iterator first, iterator last)
	{
		for (; first != last; ++first)
			erase(first);
	}


	// erase all subsequences with prefix
	template<typename Iter>
		size_type erase_prefix(Iter first, Iter last)
		{
			node_ptr cur = find_node(first, last);
			size_type ret = cur->value_count;
			clear(cur);
			return ret;
		}

	template<typename Container>
		size_type erase_prefix(const Container &container)
		{
			return erase_prefix(container.begin(), container.end());
		}

	//
	// there could be two functions of erase_prefix(), one deletes nodes including the node itself, the other deletes only its children
	//




	size_type clear(node_ptr node)
	{
		// use vector to simulate stack, avoiding the possibility of stack overflow
		// it is said that the using local type in template will be allowed in C++0x
		/*
		   class stack_element {
		   node_ptr node;
		   typename node_type::child_iter ci;
		   typename node_type::child_iter ci_end;
		   };
		   */
		node_ptr cur = node;
		size_type ret = cur->value_count;
		ret -= cur->self_value_count; // do not count values on the node

		std::stack<node_ptr> node_stk;
		std::stack<typename node_type::child_iter> ci_stk;
		node_stk.push(cur);
		ci_stk.push(cur->child.begin());
		for (; !node_stk.empty(); )
		{
			cur = node_stk.top();
			if (ci_stk.top() == cur->child.end())
			{
				if (cur != root)
				{
					delete_trie_node(cur);
				} else {
					cur->child.clear();
				}
				node_stk.pop();
				ci_stk.pop();
			} else {
				node_ptr c = ci_stk.top()->second;
				++ci_stk.top();
				node_stk.push(c);
				ci_stk.push(c->child.begin());
			}
		}

		erase_check_ancestor(node, ret);

		return ret;
	}

	void swap(const trie_type& t)
	{
		// is it OK?
		std::swap(root, t.root);
		std::swap(t.node_count, node_count);
		std::swap(t.value_alloc, value_alloc);
		std::swap(t.trie_node_alloc, trie_node_alloc);
	}

	void clear()
	{
		clear(root);
	}

	size_type count_node() const
	{
		return node_count;
	}

	size_type size() const
	{
		return root->value_count;
	}

	bool empty() const {
		return root->value_count == 0;
	}

	void destroy()
	{
		clear(root);
		delete_trie_node(root);
	}

	~trie()
	{
		destroy();
	}

};


} // tries
} // boost
#endif // BOOST_TRIE_HPP
