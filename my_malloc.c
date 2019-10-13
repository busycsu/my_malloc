#include "my_malloc.h"
#include "stdlib.h"
#include "stdio.h"

struct malloc_stc{
	struct malloc_stc *next;
	struct malloc_stc *prev;
	int size;
	int unused;	
	unsigned char *buffer;
};

static struct malloc_stc *MyFreeList;
static unsigned char MyBuff[MAX_MALLOC_SIZE];

void InitMyMalloc(){
	MyFreeList = (struct malloc_stc *)MyBuff;
	MyFreeList->next = NULL;
	MyFreeList->prev = NULL;
	MyFreeList->unused = 1;
	MyFreeList->size = sizeof(MyBuff) - sizeof(struct malloc_stc);
	MyFreeList->buffer = (unsigned char *)(MyBuff + sizeof(struct malloc_stc));

}

void PrintMyMallocFreeList(){
	
	struct malloc_stc * curr = MyFreeList;
	/*printf("*****print all******\n");
	while(curr){
		printf("-----node-----\nnext: %p\nprev: %p\nbuffer: %p\nsize: %d\nunused: %d\n",
			curr->next, curr->prev, curr->buffer, curr->size, curr->unused);
		printf("-----end_node-----\n");
		curr = curr->next;
	}
	printf("*****end*****\n\n");*/
	struct malloc_stc * pre;
	 struct malloc_stc * nxt;
	 while(curr){
	 	if(curr->unused == 1){
	 		pre = curr->prev;
	 		nxt = curr->next;

			while(pre!=NULL && pre->unused!=1){
				pre = pre->prev;
			}

			while(nxt!=NULL && nxt->unused!=1){
				nxt = nxt->next;
			}

	 		printf("block: %p\n", curr);
	 		printf("\tsize: %d\n", curr->size);
	 		printf("\tnext: %p\n", nxt);
	 		printf("\tprev: %p\n", pre);
	 		printf("\tbuffer: %p\n", curr->buffer);
	 	}
	 	curr = curr->next;
	}
}

struct malloc_stc * find_block(int size){
    struct malloc_stc * b = MyFreeList;
    while(b && !( b->unused && b->size >= size + sizeof(struct malloc_stc))){
        //*last = b;
        b = b->next;
    }

	return b;
}

void split(struct malloc_stc * block, int size){
	struct malloc_stc * temp;
	temp =(struct malloc_stc *)((block->buffer) + size);
	temp -> size = (block -> size) - size - sizeof(struct malloc_stc);
	temp -> next = block -> next;
	temp -> unused = 1;
	temp -> prev = block;
	temp -> buffer = ((unsigned char *)temp) + sizeof(struct malloc_stc);
	block -> size = size;
	block -> next = temp;
}

void *MyMalloc(int size){
	if(size <= 0){
		return NULL;
	}
	int realSize = (((size-1)>>3)<<3) + 8;
	struct malloc_stc * b;
	b = find_block(realSize);
	if(b == NULL) return NULL;
	if((int)((b->size) - realSize) >= (int)(sizeof(struct malloc_stc)) ) {
		split(b, realSize);
		b->unused = 0;
	}else{
		return NULL;
	}
	
	return b ->buffer;
}

struct malloc_stc * merge(struct malloc_stc * block){
	if(block->next != NULL && block->next->unused ==1){
		block->size = (block->size) + (block->next->size) + sizeof(struct malloc_stc);
		block->next = block -> next -> next;
		if(block->next != NULL){
			block -> next -> prev = block;
		}
	}
	return block;
}


void MyFree(void *buffer){
	struct malloc_stc *block = (struct malloc_stc *)((unsigned char *)buffer - sizeof(struct malloc_stc));
	block->unused = 1;
	//struct malloc_stc *block2 = buffer - sizeof(struct malloc_stc);
	while(block -> prev != NULL && block -> prev -> unused == 1){
		block =  merge(block->prev);
	}
	while(block->next != NULL && block -> next -> unused ==1){
		merge(block);
	}
}


