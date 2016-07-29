//#pragma once
//
////
//// ���������ͱ�
////
//struct InputIteratorTag {};
//struct OutputIteratorTag {};
//struct ForwardIteratorTag : public InputIteratorTag {};
//struct BidirectionalIteratorTag : public ForwardIteratorTag {};
//struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};
//
////
//// Traits ����һ̨��������ȡ������եȡ���������������ԣ���Ӧ���ͱ�
////
//template <class Iterator>
//struct IteratorTraits
//{
//	typedef typename Iterator:: IteratorCategory IteratorCategory ;
//	typedef typename Iterator:: ValueType        ValueType;
//	typedef typename Iterator:: DifferenceType   DifferenceType ;
//	typedef typename Iterator:: Pointer           Pointer;
//	typedef typename Iterator:: Reference         Reference;
//};
//
////
//// ƫ�ػ�ԭ��ָ������
////
//template <class T>
//struct IteratorTraits< T*>
//{
//	typedef RandomAccessIteratorTag IteratorCategory ;
//	typedef T                           ValueType;
//	typedef ptrdiff_t                   DifferenceType ;
//	typedef T *                         Pointer;
//	typedef T &                         Reference;
//};
//
////
//// ƫ�ػ�constԭ��ָ������
////
//template <class T>
//struct IteratorTraits< const T *>
//{
//	typedef RandomAccessIteratorTag IteratorCategory ;
//	typedef T                           ValueType;
//	typedef ptrdiff_t                   DifferenceType ;
//	typedef const T*                   Pointer;
//	typedef const T&                   Reference;
//};
//
//
//
//// O(1)
//template <class InputIterator>
//inline size_t _Distance (InputIterator first, InputIterator last, RandomAccessIteratorTag)
//{
//	return last-first;
//}
//
//// O(N)
//template <class InputIterator>
//inline size_t _Distance (InputIterator first, InputIterator last, InputIteratorTag)
//{
//	size_t n = 0;
//	while (first != last)
//	{
//		n++;
//		first++;
//	}
//
//	return n;
//}
//
//
//template <class InputIterator>
//inline size_t Distance (InputIterator first, InputIterator last)
//{
//	return _Distance(first, last, IteratorTraits<InputIterator>::IteratorCategory());
//}
//
//template <class InputIterator, class Distance>
//inline void __Advance(InputIterator & i, Distance n, ForwardIteratorTag)
//{
//	while (n--)
//	{
//		++i;
//	}
//}
//
//template <class InputIterator, class Distance>
//inline void  __Advance(InputIterator & i, Distance n, BidirectionalIteratorTag)
//{
//	if (n > 0)
//	{
//		while (n--)
//			++i;
//	}
//	else
//	{
//		while (n++)
//			--i;
//	}
//}
//
//template <class InputIterator, class Distance>
//inline void __Advance(InputIterator & i, Distance n, RandomAccessIteratorTag)
//{
//	i += n;
//}
//
//template <class InputIterator, class Distance>
//inline void Advance(InputIterator & i, Distance n)
//{
//	__Advance(i, n, IteratorTraits <InputIterator>:: IteratorCategory());
//}
//
//
/////////////////////////////////////////////////////////////////
//// ���������
//
//template <class Iterator>
//class ReverseIterator 
//{
//public:
//	// ͨ����������ȡ������ȡ������������ж���Ļ�������    
//	typedef typename IteratorTraits<Iterator>::IteratorCategory
//		IteratorCategory;
//	typedef typename IteratorTraits<Iterator>::ValueType
//		ValueType;
//	typedef typename IteratorTraits<Iterator>::DifferenceType
//		DifferenceType;
//	typedef typename IteratorTraits<Iterator>::Pointer
//		Pointer;
//	typedef typename IteratorTraits<Iterator>::Reference
//		Reference;
//
//	Iterator _current;
//
//	typedef ReverseIterator<Iterator> Self;
//
//	ReverseIterator(){};
//
//	// ?
//	explicit ReverseIterator(const Iterator& it)
//		:_current(it)
//	{}
//
//	Reference operator*() const
//	{
//		Iterator tmp = _current;
//		return *--tmp;
//	}
//
//	Pointer operator->()const
//	{ 
//		return &(operator*());
//	}
//
//	Self& operator++() 
//	{
//		--_current;
//		return *this;
//	}
//
//	Self operator++(int)
//	{
//		Self tmp = *this;
//		--_current;
//		return tmp;
//	}
//
//	Self& operator--()
//	{
//		++_current;
//		return *this;
//	}
//
//	Self operator--(int)
//	{
//		Self tmp = *this;
//		++_current;
//		return tmp;
//	}
//
//	bool operator==(const Self& s) const
//	{
//		return _current == s._current;
//	}
//
//	bool operator!=(const Self& s) const
//	{
//		return _current != s._current;
//	}
//};
//
////ReverseIterator rIt = ReverseIterator(it);
//
//


#pragma once

//
// ���������ͱ�
//
struct InputIteratorTag {};
struct OutputIteratorTag {};
struct ForwardIteratorTag : public InputIteratorTag {};
struct BidirectionalIteratorTag : public ForwardIteratorTag {};
struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};


template <class T, class Distance>
struct InputIterator {
	typedef InputIteratorTag IteratorCategory;
	typedef T                  ValueType;
	typedef Distance           DifferenceType;
	typedef T*                 Pointer;
	typedef T&                 Reference;
};

struct OutputIterator {
	typedef OutputIteratorTag IteratorCategory;
	typedef void                ValueType;
	typedef void                DifferenceType;
	typedef void                Pointer;
	typedef void                Reference;
};

template <class T, class Distance> 
struct ForwardIterator {
	typedef ForwardIteratorTag IteratorCategory;
	typedef T                    ValueType;
	typedef Distance             DifferenceType;
	typedef T*                   Pointer;
	typedef T&                   Reference;
};


template <class T, class Distance>
struct BidirectionalIterator {
	typedef BidirectionalIteratorTag IteratorCategory;
	typedef T                          ValueType;
	typedef Distance                   DifferenceType;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

template <class T, class Distance>
struct RandomAccessIterator {
	typedef RandomAccessIteratorTag IteratorCategory;
	typedef T                          ValueType;
	typedef Distance                   DifferenceType;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

//
// ��������Ƕ������5����Ӧ���ͱ�
// Iterator Category��Value Type��Difference Type��Pointer��Reference
// ��5����Ƕ���ͱ��壬ȷ�����ܹ�������ĸ�STL�ںϡ�
// �ҷ���Iterator Traits��������ȡ
//
template <class Category, class T, class Distance = ptrdiff_t,
class Pointer = T*, class Reference = T&>
struct Iterator
{
	typedef Category  IteratorCategory;		// ����������
	typedef T         ValueType;			// ��������ָ��������
	typedef Distance  DifferenceType;		// ����������֮��ľ���
	typedef Pointer   Pointer;				// ��������ָ�������͵�ָ��
	typedef Reference Reference;			// ��������ָ�������͵�����
};

//
// Traits����һ̨��������ȡ������եȡ���������������ԣ���Ӧ���ͱ�
//
template <class Iterator>
struct IteratorTraits
{
	typedef typename Iterator::IteratorCategory IteratorCategory;
	typedef typename Iterator::ValueType        ValueType;
	typedef typename Iterator::DifferenceType   DifferenceType;
	typedef typename Iterator::Pointer           Pointer;
	typedef typename Iterator::Reference         Reference;
};

//
// ƫ�ػ�ԭ��ָ������
//
template <class T>
struct IteratorTraits<T*>
{
	typedef RandomAccessIteratorTag IteratorCategory;
	typedef T                          ValueType;
	typedef ptrdiff_t                  DifferenceType;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

//
// ƫ�ػ�constԭ��ָ������
//
template <class T>
struct IteratorTraits<const T*>
{
	typedef RandomAccessIteratorTag IteratorCategory;
	typedef T                          ValueType;
	typedef ptrdiff_t                  DifferenceType;
	typedef const T*                   Pointer;
	typedef const T&                   Reference;
};

//////////////////////////////////////////////////////////////
// Distance��ʵ��

template <class InputIterator>
inline typename IteratorTraits<InputIterator>::DifferenceType
__Distance(InputIterator first, InputIterator last, InputIteratorTag)
{
	IteratorTraits<InputIterator>::DifferenceType n = 0;
	while (first != last) {
		++first; ++n;
	}
	return n;
}

template <class RandomAccessIterator>
inline typename IteratorTraits<RandomAccessIterator>::DifferenceType
__Distance(RandomAccessIterator first, RandomAccessIterator last,
		   RandomAccessIteratorTag)
{
	return last - first;
}

template <class InputIterator>
inline typename IteratorTraits<InputIterator>::DifferenceType
Distance(InputIterator first, InputIterator last)
{
	return __Distance(first, last, IteratorTraits<InputIterator>::IteratorCategory());
}

///////////////////////////////////////////////////////////
// Advance��ʵ��

template <class InputIterator, class Distance>
inline void __Advance(InputIterator& i, Distance n, InputIteratorTag)
{
	while (n--) ++i;
}

template <class BidirectionalIterator, class Distance>
inline void __Advance(BidirectionalIterator& i, Distance n, 
					  BidirectionalIteratorTag)
{
	if (n >= 0)
		while (n--) ++i;
	else
		while (n++) --i;
}

template <class RandomAccessIterator, class Distance>
inline void __Advance(RandomAccessIterator& i, Distance n, 
					  RandomAccessIteratorTag)
{
	i += n;
}

template <class InputIterator, class Distance>
inline void Advance(InputIterator& i, Distance n) 
{
	__Advance(i, n, IteratorTraits<InputIterator>::IteratorCategory());
}

//////////////////////////////////////////////////////////////////////
//
template <class T, class Distance> 
inline T* ValueType(const InputIterator<T, Distance>&) {
	return (T*)(0); 
}

template <class T, class Distance> 
inline T* ValueType(const ForwardIterator<T, Distance>&) {
	return (T*)(0);
}

template <class T, class Distance> 
inline T* ValueType(const BidirectionalIterator<T, Distance>&) {
	return (T*)(0);
}

template <class T, class Distance> 
inline T* ValueType(const RandomAccessIterator<T, Distance>&) {
	return (T*)(0);
}

template <class T>
inline T* ValueType(const T*) { return (T*)(0); }

//////////////////////////////////////////////////////////////////////
// ���õ������Ķ��壬������������������һ���װ

template <class Iterator>
class ReverseIterator 
{
public:
	// ͨ����������ȡ������ȡ������������ж���Ļ�������	
	typedef typename IteratorTraits<Iterator>::IteratorCategory
		IteratorCategory;
	typedef typename IteratorTraits<Iterator>::ValueType
		ValueType;
	typedef typename IteratorTraits<Iterator>::DifferenceType
		DifferenceType;
	typedef typename IteratorTraits<Iterator>::Pointer
		Pointer;
	typedef typename IteratorTraits<Iterator>::Reference
		Reference;

	typedef Iterator IteratorType;
	typedef ReverseIterator<Iterator> Self;

public:
	ReverseIterator() {}
	explicit ReverseIterator(IteratorType x)
		: _current(x) {}

	Reference operator*() const
	{
		// ע�����������ʱȡ���ǵ�ǰλ�õ�ǰһ�����ݡ�
		// ��ΪRBegin()==End() REnd()==Begin() 
		Iterator tmp = _current;
		return *--tmp;
	}

	Pointer operator->() const
	{ return &(operator*()); }

	Self& operator++() {
		--_current;
		return *this;
	}
	Self operator++(int) {
		Self tmp = *this;
		--_current;
		return tmp;
	}
	Self& operator--() {
		++_current;
		return *this;
	}
	Self operator--(int) {
		Self tmp = *this;
		++_current;
		return tmp;
	}

	bool operator != (const Self& x) {
		return _current != x._current;
	}

protected:
	Iterator _current;
};