#pragma once

#include<stdarg.h>

//#define __USE_MALLOC

//////////////////////////////////////////////////////////////////
// Trace

// �����ļ�
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
// ���ڵ���׷�ݵ�trace log
inline static void __trace_debug (const char* function,
								  const char * filename, int line , char* format , ...)
{
	// ��ȡ�����ļ�

#ifdef __DEBUG__
	// ������ú�������Ϣ
	fprintf(stdout , "�� %s:%d��%s" , GetFileName (filename). c_str(), line , function);

	// ����û����trace��Ϣ
	va_list args ;
	va_start (args , format);
	vfprintf (stdout , format, args);
	va_end (args );
#endif
}

#define __TRACE_DEBUG(...)  \
	__trace_debug(__FUNCTION__ , __FILE__, __LINE__, __VA_ARGS__);


/////////////////////////////////////////////////////////////////
// һ���ռ�������
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

		__TRACE_DEBUG("һ���ռ����÷���ռ�:%u��0x%p\n", n, result);

		return result;
	}

	static void Deallocate(void *p, size_t /* n */)
	{
		__TRACE_DEBUG("һ���ռ������ͷſռ�:0x%p\n", p);
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
// �����ռ�������
template <bool threads, int inst>
class __DefaultAllocTemplate 
{
protected:
	enum {__ALIGN = 8};							// ��׼ֵ����
	enum {__MAX_BYTES = 128};					// ����ֽ�
	enum {__NFREELISTS = __MAX_BYTES/__ALIGN};	// ��������Ĵ�С

	union Obj
	{
		union Obj * _freeListLink;
		char client_data[1];    /* The client sees this.        */
	};

	// ��������
	static Obj* _freeList[__NFREELISTS];

	// �ڴ��
	static char* _startFree;			// ˮλ��
	static char* _endFree;			// �ص�
	static size_t _heapSize;		// �ܹ���������ڴ�

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
			__TRACE_DEBUG("�ڴ�����㹻%d�������С�Ŀռ�\n", size, nobjs);

			result = _startFree;
			_startFree += totalBytes;
			return result;
		}
		else if (bytesLeft >= size)
		{
			nobjs = bytesLeft/size;
			totalBytes = nobjs*size;

			__TRACE_DEBUG("�ڴ��%d�������С�Ŀռ�\n", size, nobjs);

			result = _startFree;
			_startFree += totalBytes;
			return result;
		}
		else
		{
			__TRACE_DEBUG("�ڴ�ؿռ䲻��\n", size, nobjs);

			 size_t bytesToGet = 2 * totalBytes
				 + ROUND_UP(_heapSize >> 4);

			 // �����ڴ��ʣ���ڴ�
			 if (bytesLeft > 0)
			 {
				 size_t index = FREELIST_INDEX(bytesLeft);
				 ((Obj*)_startFree)->_freeListLink = _freeList[index];
				 _freeList[index] = (Obj*)_startFree;
			 }

			 _startFree = (char*)malloc(bytesToGet);
			 __TRACE_DEBUG("��ϵͳ����ռ���ڴ��:0x%p,size:%u\n", _startFree, bytesToGet);
			 if (_startFree == 0)
			 {
				 // �����������������ȥȡ
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

				 // ɽ��ˮ��
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
		// ���ڴ�ط����ڴ�
		char* chunk = ChunkAlloc(size, nobjs);

		__TRACE_DEBUG("size:%u,nobjs:%d\n", size, nobjs);

		if (nobjs == 1)
			return chunk;

		size_t index = FREELIST_INDEX(size);
		Obj* cur = (Obj*)(chunk+size);
		Obj* next = NULL;

		// ��ʣ���ڴ��ҵ�������������
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
		__TRACE_DEBUG("�����ռ����÷���ռ�:%u\n", n);

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
		__TRACE_DEBUG("�����ռ��������ͷſռ�:0x%p\n", p);

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
// ��װ�ռ�������

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

// �ںв��ԡ��׺в���
// �����ڴ�ص�һ������������������
void TestAllocate ()
{
	// ���Ե���һ�������������ڴ�
	cout<<" ���Ե���һ�������������ڴ� "<<endl ;
	char*p1 = SimpleAlloc< char, Alloc>::Allocate (129);
	SimpleAlloc<char , Alloc>:: Deallocate(p1 , 129);

	// ���Ե��ö��������������ڴ�
	cout<<" ���Ե��ö��������������ڴ� "<<endl;
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
		printf(" ���Ե�%d�η��� \n", i +1);
		char*p = SimpleAlloc< char, Alloc>::Allocate (128);
	}
}

// ����ϵͳ���ڴ�ľ��ĳ���
void Test3 ()
{
	cout<<" ����ϵͳ���ڴ�ľ� "<<endl ;

	SimpleAlloc<char , Alloc>::Allocate (1024*1024*1024);
	//SimpleAlloc<char, Alloc>::Allocate(1024*1024*1024);
	SimpleAlloc<char , Alloc>::Allocate (1024*1024);

	// ���ò��ԣ�˵��ϵͳ����С���ڴ���������Ǻ�ǿ�ġ�
	for (int i = 0; i < 100000; ++i )
	{
		char*p1 = SimpleAlloc< char, Alloc>::Allocate (128);
	}
}
