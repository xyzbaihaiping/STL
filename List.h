#pragma once

#include "Iterator.h"
#include "Allocator.h"
#include "Construct.h"

template <class T>
struct __ListNode
{
	__ListNode<T>* _next;
	__ListNode<T>* _prev;
	T _data;

	__ListNode(const T& x = T())
		:_next(NULL)
		,_prev(NULL)
		,_data(x)
	{}
};

// T T& T*
// T const T& const T*
template<class T, class Ref, class Ptr>
struct __ListIterator 
{
	typedef __ListIterator<T, Ref, Ptr> Self;

	typedef T ValueType;
	typedef Ptr Pointer;
	typedef Ref Reference;
	typedef ptrdiff_t DifferenceType;
	typedef BidirectionalIteratorTag IteratorCategory;

	__ListNode<T>* _node;

	__ListIterator(__ListNode<T>* x)
		:_node(x)
	{}

	__ListIterator()
	{}

	bool operator==(const Self& s) const
	{
		return _node == s._node;
	}

	bool operator != (const Self& s) const
	{
		return _node != s._node;
	}
	
	Reference operator*()
	{
		return _node->_data;
	}

	//pointer operator->() const { return &(operator*()); }

	Pointer operator->()
	{
		return &(_node->_data);
	}

	Self& operator++() 
	{
		_node = _node->_next;
		return *this;
	}

	Self operator++(int)
	{
		Self tmp(*this);
		_node = _node->_next;
		return tmp;
	}

	Self operator--()
	{
		_node = _node->_prev;
		return *this;
	}

	Self operator--(int) 
	{
		Self tmp(*this);
		_node = _node->_prev;
		return tmp;
	}
};

template<class T>
class List
{
public:
	typedef __ListNode<T> ListNode;
	typedef __ListIterator<T, T&, T*> Iterator;
	typedef __ListIterator<T, const T&, const T*> ConstIterator;

	typedef ReverseIterator<Iterator> ReverseIterator;

	// 定义空间配置器
	typedef SimpleAlloc<__ListNode<T>, Alloc> ListNodeAllocator;

	__ListNode<T>* BuyNode(const T& x = T())
	{
		// 调用空间配置器开辟空间
		// 调用统一的算法初始化内存节点对象
		__ListNode<T>* node = ListNodeAllocator::Allocate();
		Construct(node, x);

		return node;
	}

	void DestoryNode(__ListNode<T>* node)
	{
		Destroy(node);
		ListNodeAllocator::Deallocate(node);
	}

	List()
	{
		//_head = new ListNode;
		_head = BuyNode();

		_head->_next = _head;
		_head->_prev = _head;
	}

	~List()
	{
		Clear();
		DestoryNode(_head);
		_head = NULL;
	}

	void Clear()
	{
		Iterator cur = Begin();
		while (cur != End())
		{
			__ListNode<T>* del = cur._node;
			++cur;
			DestoryNode(del);
		}
	}

	//void PushBack(const T& x)
	//{
	//	ListNode* tail = _head->_prev;

	//	ListNode* tmp = new ListNode(x);
	//	tail->_next = tmp;
	//	tmp->_prev = tail;

	//	_head->_prev = tmp;
	//	tmp->_next = _head;
	//}

	void PushBack(const T& x)
	{
		Insert(End(), x);
	}

	void PushFront(const T& x)
	{
		Insert(Begin(), x);
	}

	// pos前一个位置插入一个节点
	void Insert(Iterator pos, const T& x)
	{
		ListNode* cur = pos._node;
		ListNode* prev = cur->_prev;

		//ListNode* tmp = new ListNode(x);
		ListNode* tmp = BuyNode(x);

		prev->_next = tmp;
		tmp->_next = cur;

		cur->_prev = tmp;
		tmp->_prev = prev;
	}

	void PopBack()
	{
		Erase(--End());
	}

	void PopFront()
	{
		Erase(Begin());
	}

	Iterator Erase(Iterator pos)
	{
		assert(pos != End());

		ListNode* next = pos._node->_next;
		ListNode* prev = pos._node->_prev;

		prev->_next = next;
		next->_prev = prev;

		//delete pos._node;
		DestoryNode(pos._node);

		return Iterator(next);
	}

	Iterator Begin()
	{
		return Iterator(_head->_next);
	}

	Iterator End()
	{
		return Iterator(_head);
	}

	ConstIterator Begin() const
	{
		return ConstIterator(_head->_next);//Node*
	}

	ConstIterator End() const
	{
		return _head;
	}

	ReverseIterator RBegin()
	{
		return ReverseIterator(End());
	}

	ReverseIterator REnd()
	{
		return ReverseIterator(Begin());
	}

protected:
	ListNode* _head;
};

void PrintList(const List<int>& l)
{
	List<int>::ConstIterator it = l.Begin();
	while (it != l.End())
	{
		//*it = 10;
		cout<<*it<<" ";
		++it;
	}
	cout<<endl;
}

//struct A
//{
//	int _a1;
//
//	A(int a1)
//		:_a1(a1)
//	{}
//};
//
//A f()
//{
//	return 1;
//}
//
//A a1(1);
//A a2 = 2; //a2(2)
//
//A ret = f();

void TestList()
{
	List<int> l;
	l.PushBack(1);
	l.PushBack(2);
	l.PushBack(3);
	l.PushBack(4);

	PrintList(l);

	List<int>::ReverseIterator rIt = l.RBegin();
	while (rIt != l.REnd())
	{
		cout<<*rIt<<" ";
		++rIt;
	}
	cout<<endl;
}


//// operator*
//// operator->
//struct AA
//{
//	int a1;
//	int a2;
//};
//
//void Test()
//{
//	AA* p1 = new AA;
//	//*p1
//	//p1->a1
//}


//void TestList()
//{
//	List<int> l;
//	l.PushBack(1);
//	l.PushBack(2);
//	l.PushBack(3);
//	l.PushBack(4);
//
//	List<int>::Iterator it = l.Begin();
//	while (it != l.End())
//	{
//		*it = 10;
//		cout<<*it<<" ";
//		++it;
//	}
//	cout<<endl;
//
//	List<AA> l1;
//	l1.PushBack(AA());
//	List<AA>::Iterator it1 =l1.Begin();
//	//*it1 -->AA
//	//cout<<*it1<<endl;
//	it1->a1 = 10;
//	it1->a2 = 10;
//}