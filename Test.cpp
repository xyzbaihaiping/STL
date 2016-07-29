//#include <iostream>
//using namespace std;
//
//#include <vector>
//#include <list>
//#include <algorithm>
//#include <functional>
//
//void PrintVector(const vector<int>& v)
//{
//	for (size_t i = 0; i < v.size(); ++i)
//	{
//		cout<<v[i]<<" ";
//	}
//
//	cout<<endl;
//}
////
////	vector<int>::const_iterator it = v.begin();
////	while (it != v.end())
////	{
////		//(*it)++;
////		cout<<*it<<" ";
////		++it;
////	}
////	cout<<endl;
////
////	/*vector<int>::const_reverse_iterator rit = v.rbegin();
////	while(rit != v.rend())
////	{
////		cout<<*rit<<" ";
////		++rit;
////	}*/
////
////	//cout<<endl;
////}
////
////// int* p *p
////// const int* p *p = 
////
////void TestVector()
////{
////	vector<int> v;
////	v.push_back(12222);
////
////	v.push_back(22);
////	v.push_back(322);
////	v.push_back(41);
////	v.push_back(42);
////	v.push_back(43);
////
////	v.push_back(45);
////
////	PrintVector(v);
////
////	vector<int>::iterator ret = find(v.begin(), v.end(), 3);
////	if (ret != v.end())
////	{
////		cout<<*ret<<endl;
////		*ret = 10;
////	}
////	else
////	{
////		cout<<"没有找到"<<endl;
////	}
////
////	PrintVector(v);
////
//////	sort(v.begin(), v.end());
//////	PrintVector(v);
////	
////	//[first，last)
////	vector<int>::iterator first = find(v.begin(),v.end(), 22);
////	vector<int>::iterator last = find(v.begin(),v.end(), 43);
////	sort(first, last, greater<int>());
////	PrintVector(v);
////}
//
//void TestVector()
//{
//	vector<int> v;
//	v.push_back(1);
//	v.push_back(2);
//	v.push_back(3);
//	v.push_back(4);
//
//	// 迭代器失效
//	vector<int>::iterator it = v.begin();
//	while (it != v.end())
//	{
//		if (*it % 2 == 0)
//		{
//			it = v.erase(it);
//		}
//		else
//		{
//			++it;
//		}
//	}
//
//	PrintVector(v);
//}
//
//void TestList()
//{
//	list<int> l;
//	l.push_back(1);
//	l.push_back(2);
//	l.push_back(3);
//	l.push_back(4);
//	l.push_back(2);
//	l.push_back(2);
//	l.push_back(4);
//
//
//	/*list<int>::iterator it = l.begin();
//	while (it != l.end())
//	{
//		list<int>::iterator cur = it;
//		++it;
//		if (*cur % 2 == 0)
//		{
//			l.erase(cur);
//		}
//	}*/
//
//	//l.remove(2);
//	//l.remove(2);
//	l.sort();
//	l.unique();
//
//	list<int>::iterator it = l.begin();
//	while(it != l.end())
//	{
//		cout<<*it<<" ";
//		++it;
//	}
//	cout<<endl;
//
//	//sort(l.begin(), l.end());
//
//	//it = find(l.begin(), l. end(), 2);
//	//if (it != l.end())
//	//{
//	//	l.erase(it);
//	//}
//}
//
//int main()
//{
//	//TestVector();
//	TestList();
//
//	return 0;
//}

#include <iostream>
using namespace std;

//#include "List.h"
//#include "Vector.h"
//#include "Iterator.h"
//
//void TestIterator()
//{
//	List<int> l;
//	l.PushBack(1);
//	l.PushBack(2);
//	l.PushBack(3);
//	l.PushBack(4);
//	//PrintList(l);
//
//	cout<<Distance(l.Begin(), l.End())<<endl;
//
//	Vector<int> v;
//	v.PushBack(1);
//	v.PushBack(2);
//	v.PushBack(3);
//	v.PushBack(4);
//
//	cout<<Distance(v.Begin(), v.End())<<endl;
//
//	List<int>::Iterator lIt = l.Begin();
//	Advance(lIt, -2);
//
//	Vector<int>::Iterator vIt = v.Begin();
//	Advance(vIt, 2);
//}
//

#include "Allocator.h"
#include "List.h"
#include "Vector.h"

int main()
{
	//TestList();
	TestVector();
	//TestIterator();
	//TestAllocate();
	//Test3 ();

	return 0;
}