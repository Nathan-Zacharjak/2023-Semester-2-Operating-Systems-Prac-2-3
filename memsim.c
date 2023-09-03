#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// A page number of -1 designates the page as free/unused!
typedef struct {
        int pageNo;
		// Big brain: use to count the number of times this page is accessed for LRU replacement
        int modified;
} page;
enum    repl { rdm, fifo, lru, clock};
int     createMMU( int);
int     checkInMemory( int ) ;
int     allocateFrame( int ) ;
page    selectVictim( int, enum repl) ;
const   int pageoffset = 12;            /* Page size is fixed to 4 KB */
int     numFrames ;
#define TABLE_SIZE 8
page 	PageTable[TABLE_SIZE];

/* Creates the page table structure to record memory allocation */
// Return 0 = success
// Return -1 = Fail :(
int     createMMU (int frames)
{
        // to do
		numFrames = frames;

        return 0;
}

/* Checks for residency: returns frame no or -1 if not found */
// Looks in TLB if the given page is saved in it
// Returns -1 if the page isn't in there
// Returns 0 if the page is found
// Use numFrames to tell how big the TLB is, and loop through it all
int     checkInMemory( int page_number)
{
        int     result = -1;

        // to do
		for (int i = 0; i < numFrames; i++){
			if (PageTable[i].pageNo == page_number){
				return i;
			}
		}

        return result ;
}

/* allocate page to the next free frame and record where it put it */
int     allocateFrame( int page_number)
{
        // to do
		// A page number of -1 designates the page as free/unused
		for (int i = 0; i < numFrames; i++){
			if(PageTable[i].pageNo == -1){
				PageTable[i].pageNo = page_number;
				PageTable[i].modified = 0;
				return i;
			}
		}
				
        return 0;
}

// Just randomly picks a page to replace
page rdmReplace(){
	page nothingPage;
	nothingPage.modified = -1;
	nothingPage.pageNo = -1;
	return nothingPage;
}

// OPTIONAL
page fifoReplace(){
	page nothingPage;
	nothingPage.modified = -1;
	nothingPage.pageNo = -1;
	return nothingPage;
}

// Picks the page that was least recently used
page lruReplace(){
	page nothingPage;
	nothingPage.modified = -1;
	nothingPage.pageNo = -1;
	return nothingPage;
}

// Selects pages in a circular buffer-style
int	nextClockReplacement = 0;
page clockReplace(){
	page nothingPage;
	nothingPage.modified = -1;
	nothingPage.pageNo = -1;
	return nothingPage;
}

/* Selects a victim for eviction/discard according to the replacement algorithm,  returns chosen frame_no  */
page    selectVictim(int page_number, enum repl  mode )
{
        page    victim;
        // to do 
		switch (mode)
		{
		case rdm:
			victim = rdmReplace();
			break;
		
		case fifo:
			victim = fifoReplace();
			break;
		
		case lru:
			victim = lruReplace();
			break;
		
		case clock:
			victim = clockReplace();
			break;
		
		default:
			break;
		}

        return victim;
}

// void testFunc(int var){
// 	int arr[var];

// 	arr[var-1] = 384378;
// 	printf("%d\n", arr[var-1]);
// 	return;
// }

		
int main(int argc, char *argv[])
{
	// int var = 5;
	// testFunc(var);
	char	*tracename;
	int	page_number,frame_no, done ;
	int	do_line;
	int	no_events, disk_writes, disk_reads;
	int     debugmode;
 	enum	repl  replace;
	int	allocated = 0; 
    unsigned address;
    char 	rw;
	page	Pvictim;
	FILE	*trace;


        if (argc < 5) {
             printf( "Usage: ./memsim inputfile numberframes replacementmode debugmode \n");
             exit ( -1);
	}
	else {
        tracename = argv[1];	
	trace = fopen( tracename, "r");
	if (trace == NULL ) {
             printf( "Cannot open trace file %s \n", tracename);
             exit ( -1);
	}
	numFrames = atoi(argv[2]);
        if (numFrames < 1) {
            printf( "Frame number must be at least 1\n");
            exit ( -1);
        }
        if (strcmp(argv[3], "lru\0") == 0)
            replace = lru;
	    else if (strcmp(argv[3], "rand\0") == 0)
	     replace = rdm;
	          else if (strcmp(argv[3], "clock\0") == 0)
                       replace = clock;		 
	               else if (strcmp(argv[3], "fifo\0") == 0)
                             replace = fifo;		 
        else 
	  {
             printf( "Replacement algorithm must be rand/fifo/lru/clock  \n");
             exit ( -1);
	  }

        if (strcmp(argv[4], "quiet\0") == 0)
            debugmode = 0;
	else if (strcmp(argv[4], "debug\0") == 0)
            debugmode = 1;
        else 
	  {
             printf( "Replacement algorithm must be quiet/debug  \n");
             exit ( -1);
	  }
	}
	
	done = createMMU (numFrames);
	if ( done == -1 ) {
		 printf( "Cannot create MMU" ) ;
		 exit(-1);
        }
	no_events = 0 ;
	disk_writes = 0 ;
	disk_reads = 0 ;

    do_line = fscanf(trace,"%x %c",&address,&rw);
	while ( do_line == 2)
	{
		page_number =  address >> pageoffset;
		frame_no = checkInMemory( page_number) ;    /* ask for physical address */

		if ( frame_no == -1 )
		{
		  disk_reads++ ;			/* Page fault, need to load it into memory */
		  if (debugmode) 
		      printf( "Page fault %8d \n", page_number) ;
		  if (allocated < numFrames)  			/* allocate it to an empty frame */
		   {
                     frame_no = allocateFrame(page_number);
		     allocated++;
                   }
                   else{
		      Pvictim = selectVictim(page_number, replace) ;   /* returns page number of the victim  */
		      frame_no = checkInMemory( page_number) ;    /* find out the frame the new page is in */
		   if (Pvictim.modified)           /* need to know victim page and modified  */
	 	      {
                      disk_writes++;			    
                      if (debugmode) printf( "Disk write %8d \n", Pvictim.pageNo) ;
		      }
		   else
                      if (debugmode) printf( "Discard    %8d \n", Pvictim.pageNo) ;
		   }
		}
		
		if ( rw == 'R'){
		    if (debugmode) printf( "reading    %8d \n", page_number) ;
		}
		else if ( rw == 'W'){
		    // mark page in page table as written - modified  
		    if (debugmode) printf( "writting   %8d \n", page_number) ;
		}
		 else {
		      printf( "Badly formatted file. Error on line %d\n", no_events+1); 
		      exit (-1);
		}

		no_events++;
        	do_line = fscanf(trace,"%x %c",&address,&rw);
	}

	printf( "total memory frames:  %d\n", numFrames);
	printf( "events in trace:      %d\n", no_events);
	printf( "total disk reads:     %d\n", disk_reads);
	printf( "total disk writes:    %d\n", disk_writes);
	printf( "page fault rate:      %.4f\n", (float) disk_reads/no_events);


	return 0;
}
				
