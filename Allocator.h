#pragma once

#include<stdarg.h>

//#define __USE_MALLOC

//////////////////////////////////////////////////////////////////
// Trace

// 配置文件
#define __DEBUG__

static string GetFileName(const string& path)
{
	char ch = '/';

#ifdef _WIN32
	ch = '\\' ;
#endif

	size_t pos = path. rfind(ch );
	if (pos == string:: npos)
		return path ;
	else
		return path .substr( pos + 1);
}
// 用于调试追溯的trace log
inline static void __trace_debug (const char* function,
								  const char * filename, int line , char* format , ...)
{
	// 读取配置文件

#ifdef __DEBUG__
	// 输出调用函数的信息
	fprintf(stdout , "【 %s:%d】%s" , GetFileName (filename). c_str(), line , function);

	// 输出用户打的trace信息
	va_list args ;
	va_start (args , format);
	vfprintf (stdout , format, args);
	va_end (args );
#endif
}

#define __TRACE_DEBUG(...)  \
	__trace_debug(__FUNCTION__ , __FILE__, __LINE__, __VA_ARGS__);


/////////////////////////////////////////////////////////////////
// 一级空间配置器
//

typedef void(*HANDER_FUNC)();

template <int inst>
class __MallocAllocTemplate
{

private:
	static void *OomMalloc(size_t size)
	{
		while (1)
		{
			if (__mallocAllocOomHandler)
			{
				__mallocAllocOomHandler();
				void* ret = malloc(size);
				if (ret)
					return ret;
			}
			else
			{
				throw bad_alloc();
			}
		}
	}

	//static void (* __mallocAllocOomHandler)();
	static HANDER_FUNC __mallocAllocOomHandler;

public:
	static void * Allocate(size_t n)
	{
		void *result = malloc(n);
		if (0 == result)
			result = OomMalloc(n);

		__TRACE_DEBUG("一级空间配置分配空间:%u，0x%p\n", n, result);

		return result;
	}

	static void Deallocate(void *p, size_t /* n */)
	{
		__TRACE_DEBUG("一级空间配置释放空间:0x%p\n", p);
		free(p);
	}

	static HANDER_FUNC SetMallocHandler(HANDER_FUNC f)
	{
		HANDER_FUNC old = __mallocAllocOomHandler;
		__mallocAllocOomHandler = f;
		return (old);
	}
};

template<int inst>
HANDER_FUNC __MallocAllocTemplate<inst>::__mallocAllocOomHandler = 0;

/////////////////////////////////////////////////////////////
// 二级空间配置器
template <bool threads, int inst>
class __DefaultAllocTemplate 
{
protected:
	enum {__ALIGN = 8};							// 基准值对齐
	enum {__MAX_BYTES = 128};					// 最大字节
	enum {__NFREELISTS = __MAX_BYTES/__ALIGN};	// 自由链表的大小

	union Obj
	{
		union Obj * _freeListLink;
		char client_data[1];    /* The client sees this.        */
	};

	// 自由链表
	static Obj* _freeList[__NFREELISTS];

	// 内存池
	static char* _startFree;			// 水位线
	static char* _endFree;			// 池底
	static size_t _heapSize;		// 总共分配多少内存

	// 7 8 9
	static size_t FREELIST_INDEX(size_t bytes)
	{
		return (((bytes) + __ALIGN-1)/__ALIGN - 1);
	}

	static size_t ROUND_UP(size_t bytes)
	{
		return (((bytes) + __ALIGN-1) & ~(__ALIGN - 1));
	}

public:
	static char* ChunkAlloc(size_t size, size_t& nobjs)
	{
		char * result;
		size_t totalBytes = size * nobjs;
		size_t bytesLeft = _endFree - _startFree;

		if (bytesLeft >= totalBytes)
		{
			__TRACE_DEBUG("内存池有足够%d个对象大小的空间\n", size, nobjs);

			result = _startFree;
			_startFree += totalBytes;
			return result;
		}
		else if (bytesLeft >= size)
		{
			nobjs = bytesLeft/size;
			totalBytes = nobjs*size;

			__TRACE_DEBUG("内存池%d个对象大小的空间\n", size, nobjs);

			result = _startFree;
			_startFree += totalBytes;
			return result;
		}
		else
		{
			__TRACE_DEBUG("内存池空间不足\n", size, nobjs);

			 size_t bytesToGet = 2 * totalBytes
				 + ROUND_UP(_heapSize >> 4);

			 // 处理内存池剩余内存
			 if (bytesLeft > 0)
			 {
				 size_t index = FREELIST_INDEX(bytesLeft);
				 ((Obj*)_startFree)->_freeListLink = _freeList[index];
				 _freeList[index] = (Obj*)_startFree;
			 }

			 _startFree = (char*)malloc(bytesToGet);
			 __TRACE_DEBUG("到系统申请空间给内存池:0x%p,size:%u\n", _startFree, bytesToGet);
			 if (_startFree == 0)
			 {
				 // 到更大的自由链表中去取
				 for(size_t i = size; i <= __MAX_BYTES;
					 i += __ALIGN)
				 {
					 size_t index = FREELIST_INDEX(i);
					 if (_freeList[index])
					 {
						 _startFree = (char*)_freeList[index];
						 _endFree = _startFree+size;
						 _freeList[index] = _freeList[index]->_freeListLink;
						 return ChunkAlloc(size, nobjs);
					 }
				 }

				 _endFree = 0;

				 // 山穷水尽
				 _startFree = (char*)__MallocAllocTemplate<0>::Allocate(bytesToGet);
			}

			_heapSize += bytesToGet;
			_endFree = _startFree+bytesToGet;
			return ChunkAlloc(size, nobjs);
		}
	}

	static void* Refill(size_t size)
	{
		size_t nobjs = 20;
		// 找内存池分配内存
		char* chunk = ChunkAlloc(size, nobjs);

		__TRACE_DEBUG("size:%u,nobjs:%d\n", size, nobjs);

		if (nobjs == 1)
			return chunk;

		size_t index = FREELIST_INDEX(size);
		Obj* cur = (Obj*)(chunk+size);
		Obj* next = NULL;

		// 将剩余内存块挂到自由链表下面
		_freeList[index] = cur;
		for(int i = 1; i < nobjs-1; ++i)
		{
			next = (Obj*)((char*)cur+size);
			cur->_freeListLink = next;
			cur = next;
		}
		cur->_freeListLink = NULL;

		return chunk;
	}

	static void* Allocate(size_t n)
	{
		__TRACE_DEBUG("二级空间配置分配空间:%u\n", n);

		if (n > __MAX_BYTES)
		{
			return __MallocAllocTemplate<0>::Allocate(n);
		}

		size_t index = FREELIST_INDEX(n);
		if (_freeList[index] == NULL)
		{
			return Refill(ROUND_UP(n));
		}
	
		Obj* ret = _freeList[index];
		_freeList[index] = ret->_freeListLink;

		return ret;
	}

	static void Deallocate(void *p, size_t n)
	{
		__TRACE_DEBUG("二级空间配置器释放空间:0x%p\n", p);

		if (n > __MAX_BYTES)
		{
			__MallocAllocTemplate<0>::Deallocate(p, n);
			return;
		}

		Obj* obj = (Obj*)p;
		size_t index = FREELIST_INDEX(n);
		obj->_freeListLink = _freeList[index];
		_freeList[index] = obj;
	}
};

template <bool threads, int inst>
char* __DefaultAllocTemplate<threads, inst>::_startFree = NULL;

template <bool threads, int inst>
char* __DefaultAllocTemplate<threads, inst>::_endFree = NULL;

template <bool threads, int inst>
size_t __DefaultAllocTemplate<threads, inst>::_heapSize = NULL;

template <bool threads, int inst>
typename __DefaultAllocTemplate<threads, inst>::Obj*
__DefaultAllocTemplate<threads, inst>::_freeList[__NFREELISTS] = {0};

#ifdef __USE_MALLOC
typedef __MallocAllocTemplate<0> Alloc;
#else
typedef __DefaultAllocTemplate<0,0> Alloc;
#endif // _DEBUG


///////////////////////////////////////////////
// 封装空间配置器

template<class T, class Alloc>
class SimpleAlloc
{
public:
	static T* Allocate(size_t n)
	{ 
		return 0 == n? 0 : (T*) Alloc::Allocate(n * sizeof (T));
	}

	static T* Allocate(void)
	{ 
		return (T*) Alloc::Allocate(sizeof (T));
	}

	static void Deallocate(T *p, size_t n)
	{ 
		if (0 != n)
			Alloc::Deallocate(p, n * sizeof (T));
	}

	static void Deallocate(T *p)
	{ 
		Alloc::Deallocate(p, sizeof (T));
	}
};

// 黑盒测试、白盒测试
// 测试内存池的一级、二级配置器功能
void TestAllocate ()
{
	// 测试调用一级配置器分配内存
	cout<<" 测试调用一级配置器分配内存 "<<endl ;
	char*p1 = SimpleAlloc< char, Alloc>::Allocate (129);
	SimpleAlloc<char , Alloc>:: Deallocate(p1 , 129);

	// 测试调用二级配置器分配内存
	cout<<" 测试调用二级配置器分配内存 "<<endl;
	char*p2 = SimpleAlloc< char, Alloc>::Allocate (128);
	char*p3 = SimpleAlloc< char, Alloc>::Allocate (128);
	char*p4 = SimpleAlloc< char, Alloc>::Allocate (128);
	char*p5 = SimpleAlloc< char, Alloc>::Allocate (128);
	SimpleAlloc<char , Alloc>:: Deallocate(p2 , 128);
	SimpleAlloc<char , Alloc>:: Deallocate(p3 , 128);
	SimpleAlloc<char , Alloc>:: Deallocate(p4 , 128);
	SimpleAlloc<char , Alloc>:: Deallocate(p5 , 128);

	for (int i = 0; i < 21; ++i)
	{
		printf(" 测试第%d次分配 \n", i +1);
		char*p = SimpleAlloc< char, Alloc>::Allocate (128);
	}
}

// 测试系统堆内存耗尽的场景
void Test3 ()
{
	cout<<" 测试系统堆内存耗尽 "<<endl ;

	SimpleAlloc<char , Alloc>::Allocate (1024*1024*1024);
	//SimpleAlloc<char, Alloc>::Allocate(1024*1024*1024);
	SimpleAlloc<char , Alloc>::Allocate (1024*1024);

	// 不好测试，说明系统管理小块内存的能力还是很强的。
	for (int i = 0; i < 100000; ++i )
	{
		char*p1 = SimpleAlloc< char, Alloc>::Allocate (128);
	}
}
