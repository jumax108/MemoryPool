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
			// ����ڰ� ��û�� ũ��
			int _requestAllocSize;
			// ������ �Ҵ�� ũ��, �˸��� ����Ʈ�� �ǵ��ư��� ����
			int _actualAllocSize;
			bool _isAlloc = true;
			// ������ �����ִ� ����Ʈ�� ���
			void* _blockListNode;
			unsigned __int64 _underflow = OVERFLOW_CHECK_VALUE;
		};

		struct stNextMemory{
			unsigned __int64 _overflow = OVERFLOW_CHECK_VALUE;
		};

	#else

		struct stPrevMemory{
			// ����ڰ� ��û�� ũ��
			int _requestAllocSize;
			// ������ �Ҵ�� ũ��, �˸��� ����Ʈ�� �ǵ��ư��� ����
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

		// �Ҵ� ũ��
		int _memSize;

		// ������ �޸� �� ��
		int _memoryBlockCapacity;

		// �Ҵ��� �޸� �� ��
		int _allocMemoryBlockNum;

		// �Ҵ� ������ �� ����Ʈ
		stBlockList* _allocAbleList;

		#if defined(USE_OWN_HEAP)
			// �� �ڵ�
			HANDLE _heapHandle;
		#endif

		#ifdef MEMORY_POOL_SAFE
			// ���� �ڵ� ��� �뵵
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