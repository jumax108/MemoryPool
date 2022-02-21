#pragma once

#include <malloc.h>
#include <Windows.h>

#include "common.h"

namespace memoryPool{

	#if defined(MEMORY_POOL_SAFE)

		#include "errorCode.h"
		constexpr unsigned __int64	UNDERFLOW_CHECK_VALUE	= 0xFFEEDDCCCCDDEEFF;
		constexpr unsigned __int64	OVERFLOW_CHECK_VALUE	= 0xFFEEDDCCCCDDEEFF;

		struct stPrevMemory{
			// 사용자가 요청한 크기
			int _requestAllocSize;
			// 실제로 할당된 크기, 알맞은 리스트로 되돌아가기 위함
			int _actualAllocSize;
			bool _isAlloc = true;
			// 본인이 속해있던 리스트의 노드
			void* _blockListNode;
			unsigned __int64 _underflow = OVERFLOW_CHECK_VALUE;
		};

		struct stNextMemory{
			unsigned __int64 _overflow = OVERFLOW_CHECK_VALUE;
		};

	#else

		struct stPrevMemory{
			// 사용자가 요청한 크기
			int _requestAllocSize;
			// 실제로 할당된 크기, 알맞은 리스트로 되돌아가기 위함
			int _actualAllocSize;
		};

		struct stNextMemory{};

	#endif



	class CMemoryBlockList {

		struct stBlockList;
		friend class CMemoryPool;

	private:

		CMemoryBlockList(int memSize);
		~CMemoryBlockList();

		void* allocMem();
		bool freeMem(void*);

		inline int getMemoryBlockCapacity();
		inline int getAllocMemoryBlockNum();

		// 할당 크기
		int _memSize;

		// 생성한 메모리 블럭 수
		int _memoryBlockCapacity;

		// 할당한 메모리 블럭 수
		int _allocMemoryBlockNum;

		// 할당 가능한 블럭 리스트
		stBlockList* _allocAbleList;

		#if defined(USE_OWN_HEAP)
			// 힙 핸들
			HANDLE _heapHandle;
		#endif

		#ifdef MEMORY_POOL_SAFE
			// 에러 코드 얻는 용도
			int lastError = 0;
			inline int getErrorCode();
		#endif

		struct stBlockList {
			void* _block = nullptr;
			stBlockList* _next = nullptr;
			stBlockList& operator=(stBlockList& blockList){
				_block = blockList._block;
				_next  = blockList._next;
			}
		};


	};
	
		
	int CMemoryBlockList::getMemoryBlockCapacity(){
		return _memoryBlockCapacity;
	}

	int CMemoryBlockList::getAllocMemoryBlockNum(){
		return _allocMemoryBlockNum;
	}

	#if defined(MEMORY_POOL_SAFE)
		int CMemoryBlockList::getErrorCode(){
			return lastError;
		}
	#endif
};