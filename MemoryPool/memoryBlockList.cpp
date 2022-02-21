
#include "headers/memoryBlockList.h"

namespace memoryPool{

	CMemoryBlockList::CMemoryBlockList(int memSize){
		_memSize = memSize;
		_allocAbleList = nullptr;

		#if defined(USE_OWN_HEAP)
			_heapHandle = HeapCreate(0, 0, 0);
		#endif

		_memoryBlockCapacity = 0;
		_allocMemoryBlockNum = 0;
	}

	CMemoryBlockList::~CMemoryBlockList(){

		while(_allocAbleList != nullptr){
			stBlockList* next = _allocAbleList->_next;
			#if defined(USE_OWN_HEAP)
				HeapFree(_heapHandle, 0, _allocAbleList->_block);
				HeapFree(_heapHandle, 0, _allocAbleList);
			#else
				free(_allocAbleList->_block);
				free(_allocAbleList);
			#endif
			_allocAbleList = next;
		}
	}

	void* CMemoryBlockList::allocMem(){

		if(_allocAbleList == nullptr){
			unsigned __int64 blockSize = _memSize + sizeof(stPrevMemory) + sizeof(stNextMemory);
			#if defined(USE_OWN_HEAP)
				_allocAbleList = (stBlockList*)HeapAlloc(_heapHandle, 0, sizeof(stBlockList));
				_allocAbleList->_block = HeapAlloc(_heapHandle, 0, blockSize);
				_allocAbleList->_next = nullptr;
			#else	
				_allocAbleList = (stBlockList*)malloc(sizeof(stBlockList));
				_allocAbleList->_block = malloc(blockSize);
			#endif
			_memoryBlockCapacity += 1;
		}

		stPrevMemory* prevInfo = (stPrevMemory*)_allocAbleList->_block;
		prevInfo->_blockListNode = _allocAbleList;
		prevInfo->_actualAllocSize = _memSize;
		#ifdef MEMORY_POOL_SAFE
			prevInfo->_underflow = UNDERFLOW_CHECK_VALUE;
			prevInfo->_isAlloc = true;
		#endif
		
		// set next block
		_allocAbleList = _allocAbleList->_next;

		_allocMemoryBlockNum += 1;

		// return memory block
		return (void*)prevInfo;
	}

	bool CMemoryBlockList::freeMem(void* memPtr){
	
		void* usedBlock = memPtr;
		stPrevMemory* prevMemory = (stPrevMemory*)usedBlock;

		#if defined(MEMORY_POOL_SAFE)
		{
			stNextMemory* nextMemory = (stNextMemory*)((unsigned __int64)prevMemory + prevMemory->_requestAllocSize + sizeof(stPrevMemory));

			if(prevMemory->_isAlloc == false){
				lastError = memoryPool::DuplicatedFree;
				return false;
			}
			if(prevMemory->_underflow != UNDERFLOW_CHECK_VALUE){
				lastError = memoryPool::UnderFlow;
				return false;
			}
			if(nextMemory->_overflow != OVERFLOW_CHECK_VALUE){
				lastError = memoryPool::OverFlow;
				return false;
			}

			prevMemory->_isAlloc = false;
		}
		#endif

		stBlockList* usedBlockList = (stBlockList*)prevMemory->_blockListNode;

		usedBlockList->_next = _allocAbleList;
		_allocAbleList = usedBlockList;

		_allocMemoryBlockNum -= 1;

		return true;

	}
};
