#pragma once

#include <stdio.h>
#include <new>

#include "common.h"
#include "memoryBlockList.h"

namespace memoryPool{

	class CMemoryPool{

		struct stAllocator;

	public:

		CMemoryPool();

		void*	allocMem	(int memSize);
		bool	freeMem		(void* ptr);

		inline int getAllocatorNum();

		inline void setAllocatorBegin();
		inline int getAllocatorMemoryBlockNum();
		inline int getAllocatorMemoryBlockCapacity();
		inline int getAllocatorMemorySize();
		inline bool setNextAllocator();

		#if defined(MEMORY_POOL_SAFE)
			inline int getErrorCode();
		#endif

	private:

		// allocator capacity
		int _allocatorCap;
		// allocator num
		int _allocatorNum;
		// allocator arr
		stAllocator* _allocatorArr;

		// allocator iterator
		stAllocator* _allocatorIter;
		stAllocator* _allocatorEnd;
	
		#if defined(MEMORY_POOL_SAFE)
			// error code
			int _errorCode;
		#endif

		// heap handle
		HANDLE _heapHandle;

		struct stAllocator{
			int _memSize;
			CMemoryBlockList* _allocator;
		};

		stAllocator* getSuitableAllocator(int memSize);
		stAllocator* addAllocator(int memSize);
		void resizeAllocatorArr();

	};

	

	int CMemoryPool::getAllocatorNum(){
		return _allocatorNum;
	}

	void CMemoryPool::setAllocatorBegin(){
		_allocatorIter = _allocatorArr;
	}

	int CMemoryPool::getAllocatorMemoryBlockNum(){
		return _allocatorIter->_allocator->getAllocMemoryBlockNum();
	}

	int CMemoryPool::getAllocatorMemoryBlockCapacity(){
		return _allocatorIter->_allocator->getMemoryBlockCapacity();
	}

	int CMemoryPool::getAllocatorMemorySize(){
		return _allocatorIter->_memSize;
	}

	bool CMemoryPool::setNextAllocator(){
		
		_allocatorIter += 1;
		if(_allocatorIter == _allocatorEnd){
			return false;
		}

		return true;
	}

	#if defined(MEMORY_POOL_SAFE)
		int CMemoryPool::getErrorCode(){
			return _errorCode;
		}
	#endif

};