#include "headers/memoryPool.h"

namespace memoryPool{

	CMemoryPool::CMemoryPool(){
		_allocatorCap = 5;
		_allocatorNum = 0;

		_heapHandle = HeapCreate(0, 0, 0);

		#if defined(USE_OWN_HEAP)
			_allocatorArr = (stAllocator*)HeapAlloc(_heapHandle, 0, sizeof(stAllocator) * _allocatorCap);
		#else
			_allocatorArr = (stAllocator*)malloc(sizeof(stAllocator) * _allocatorCap);
		#endif

		_allocatorIter = _allocatorArr;
		_allocatorEnd  = _allocatorArr + _allocatorCap;

		_errorCode = 0;
	}

	void* CMemoryPool::allocMem(int memSize){
	
		// allocator ȹ��
		stAllocator* suitableAllocator = getSuitableAllocator(memSize);
		if(suitableAllocator == nullptr){
			suitableAllocator = addAllocator((int)(memSize * 1.5));
		}

		// alloc memory block
		void* allocMemory = suitableAllocator->_allocator->allocMem();

		// set memory block
		stPrevMemory* prevMemory = (stPrevMemory*)allocMemory;
		prevMemory->_requestAllocSize = memSize;
		#if defined(MEMORY_POOL_SAFE)
			stNextMemory* nextMemory = (stNextMemory*)((unsigned __int64)prevMemory + sizeof(stPrevMemory) + memSize);
			nextMemory->_overflow = OVERFLOW_CHECK_VALUE;
		#endif

		return (void*)((unsigned __int64)allocMemory + sizeof(stPrevMemory));
	}

	bool CMemoryPool::freeMem(void* ptr){

		// get memory block start point
		stPrevMemory* prevMemory = (stPrevMemory*)((unsigned __int64)ptr - sizeof(stPrevMemory));
		
		// allocator ȹ��
		stAllocator* suitableAllocator = getSuitableAllocator(prevMemory->_actualAllocSize);
		if(suitableAllocator == nullptr){
			// underflow�� ���� allocSize �ջ�
			return false;
		}
		
		// free memory block
		if(suitableAllocator->_allocator->freeMem(prevMemory) == false){
			#if defined(MEMORY_POOL_SAFE)
				_errorCode = suitableAllocator->_allocator->getErrorCode();
			#endif
			return false;
		}

		return true;
	}

	CMemoryPool::stAllocator* CMemoryPool::getSuitableAllocator(int memSize){

		if(_allocatorNum == 0){
			return nullptr;
		}

		if(_allocatorArr[_allocatorNum - 1]._memSize < memSize){
			return nullptr;
		}

		int left = 0;
		int right = _allocatorNum;
		int mid = (left + right) / 2;

		for(;;){
			
			stAllocator* midAllocator = &_allocatorArr[mid];

			if(right - left <= 1){
				
				if(midAllocator->_memSize >= memSize){
					return midAllocator;
				} else {
					return &_allocatorArr[right];
				}
			}

			if(midAllocator->_memSize == memSize){
				return midAllocator;
			}

			if(midAllocator->_memSize > memSize){
				right = mid;
			} else {
				left = mid;
			}

			mid = (left + right) / 2;


		}

		return nullptr;

	}

	CMemoryPool::stAllocator* CMemoryPool::addAllocator(int memSize){
	
		// ���� ���� Ȯ��
		if(_allocatorCap <= _allocatorNum + 1){
			resizeAllocatorArr();
		}

		// allocator ���� ��ġ ����
		int left = 0;
		int right = _allocatorNum;
		int mid = (left + right) / 2;
		stAllocator* midAllocator = nullptr;

		for(;right - left > 1;){
			
			midAllocator = &_allocatorArr[mid];

			if(midAllocator->_memSize > memSize){
				right = mid;
			} else {
				left = mid;
			}

			mid = (left + right) / 2;

		}

		int newAllocatorIdx;

		if(midAllocator == nullptr){
			newAllocatorIdx = _allocatorNum;
		} else {
			if(midAllocator->_memSize > memSize){
				newAllocatorIdx = mid;
			} else {
				newAllocatorIdx = mid + 1;
			}
		}

		// allocator �� ĭ�� �б�
		for(int allocatorIdx = _allocatorNum - 1; allocatorIdx >= newAllocatorIdx; --allocatorIdx) {
			_allocatorArr[allocatorIdx + 1] = _allocatorArr[allocatorIdx];
		}

		// ������ �ִ� allocator�� ������� ���� �ƴϱ⿡ �Ҹ� ����� �ƴ�
		// ���� ���ο� allocator�� �ٽ� ����
		_allocatorArr[newAllocatorIdx]._memSize = memSize;
		if(_allocatorArr[newAllocatorIdx]._allocator == nullptr){
			_allocatorArr[newAllocatorIdx]._allocator = new CMemoryBlockList(memSize);
		} else {
			new (_allocatorArr[newAllocatorIdx]._allocator) CMemoryBlockList(memSize);
		}

		_allocatorNum += 1;

		return &_allocatorArr[newAllocatorIdx];
	}

	void CMemoryPool::resizeAllocatorArr(){

		int nextAllocCap = _allocatorCap * 2 + 1;
	
		stAllocator* newArr;

		#if defined(USE_OWN_HEAP)
			newArr = (stAllocator*)HeapAlloc(_heapHandle, 0, sizeof(stAllocator) * _allocatorCap);
		#else
			newArr = (stAllocator*)malloc(sizeof(stAllocator) * _allocatorCap);
		#endif

		for(int arrCnt = 0; arrCnt < _allocatorNum; ++arrCnt){
			newArr[arrCnt] = _allocatorArr[arrCnt];
		}

		HeapFree(_heapHandle, 0, _allocatorArr);
		_allocatorArr = newArr;

		_allocatorCap = nextAllocCap;
		_allocatorEnd  = _allocatorArr + _allocatorCap;

	}
	
	
};