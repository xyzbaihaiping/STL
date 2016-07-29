#pragma once

#include "Allocator.h"
#include "Construct.h"
#include "Uninitialized.h"

//template<class T, class Ref, class Ptr>
//struct __VectorIterator
//{
//	T* _ptr;
//};

template<class T, class Alloc = Alloc>
class Vector
{
public:
	typedef T ValueType;
	typedef ValueType* Iterator;
	typedef const ValueType* ConstIterator;

	typedef ReverseIterator<Iterator> ReverseIterator;

	typedef SimpleAlloc<ValueType, Alloc> DataAllocator;

	Vector()
		:_start(0)
		,_finish(0)
		,_endOfStorage(0)
	{}

	~Vector()
	{
		Destroy(Begin(), End());
		DataAllocator::Deallocate(_start);
	}

	void PushBack(const ValueType& x)
	{
		_CheckStorage();

		*_finish = x;
		++_finish;
	}

	void Insert(Iterator pos, const T& x);
	Iterator Erase(Iterator pos);
	T& operator[](size_t index);// v[0] = 10;

	size_t Size()
	{
		return _finish - _start;
	}

	Iterator Begin()
	{
		return _start;
	}

	Iterator End()
	{
		return _finish;
	}

	ConstIterator Begin() const 
	{
		return _start;
	}

	ConstIterator End() const 
	{
		return _finish;
	}

protected:
	void _CheckStorage()
	{
		if (_finish == _endOfStorage)
		{
			size_t size = Size();
			size_t newStorage = size*2+3;
			/*T* tmp = new T[newStorage];
			if (_start)
			{
				for (size_t i = 0; i < size; ++i)
				{
					tmp[i] = _start[i];
				}

				delete[] _start;
			}*/

			T* tmp = DataAllocator::Allocate(newStorage);
			if (_start)
			{
				UninitializedCopy(_start, _finish, tmp);
				//delete[] _start;
				Destroy(_start, _finish);
			}

			_start = tmp;
			_finish = _start + size;
			_endOfStorage = _start + newStorage;
		}
	}

protected:
	//T* _a;
	//size_t _size;
	//size_t _capacity;

	Iterator _start;
	Iterator _finish;
	Iterator _endOfStorage;
};

void PrintVector1(Vector<int>& v)
{
	Vector<int>::Iterator it = v.Begin();
	while (it != v.End())
	{
		*it = 10;
		cout<<*it<<" ";
		++it;
	}
	cout<<endl;
}

void PrintVector2(const Vector<int>& v)
{
	Vector<int>::ConstIterator it = v.Begin();
	while (it != v.End())
	{
		//*it = 10;
		cout<<*it<<" ";
		++it;
	}
	cout<<endl;
}

void TestVector()
{
	Vector<int> v;
	v.PushBack(1);
	v.PushBack(2);
	v.PushBack(3);
	v.PushBack(4);

	PrintVector1(v);
	PrintVector2(v);
}