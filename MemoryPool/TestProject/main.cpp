#include <stdio.h>

#include "lib/memoryPool/headers/memoryPool.h"
#pragma comment(lib, "lib/memoryPool/MemoryPool.lib")

int main(){

	memoryPool::CMemoryPool pool;

	for(;;){

		char* buf[100];
		for(int loopCnt = 0; loopCnt < 100; loopCnt++){

			buf[loopCnt] = (char*)pool.allocMem(10);
			memset(buf[loopCnt], 0xFF, 10);

		}

		for(int loopCnt = 0; loopCnt < 100; loopCnt++){
			if(pool.freeMem(buf[loopCnt]) == false){
				int errorCode = pool.getErrorCode();
				printf("errorCode: %d\n", errorCode);
			}
		}
		
		for(int loopCnt = 0; loopCnt < 100; loopCnt++){

			buf[loopCnt] = (char*)pool.allocMem(30);
			memset(buf[loopCnt], 0xFF, 30);


		}
		
		for(int loopCnt = 0; loopCnt < 100; loopCnt++){
			if(pool.freeMem(buf[loopCnt]) == false){
				int errorCode = pool.getErrorCode();
				printf("errorCode: %d\n", errorCode);
			}
		}

		system("cls");
		int allocatorNum = pool.getAllocatorNum();
		printf("Allocator Num: %d\n", allocatorNum);
		
		pool.setAllocatorBegin();
		for(int allocatorCnt = 0; allocatorCnt < allocatorNum; ++allocatorCnt){

			printf("Allocator %02d Manage Memory Size   : %d\n", allocatorCnt, pool.getAllocatorMemorySize());
			printf("Allocator %02d Memory Block Capacity: %d\n", allocatorCnt, pool.getAllocatorMemoryBlockCapacity());
			printf("\n");
			pool.setNextAllocator();

		}

		getchar();

	}

	return 0;

}