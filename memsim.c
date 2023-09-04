#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// A page number of -1 designates the page as free/unused!
typedef struct {
        int pageNo;
		// Used for telling if a page frame has ever been used for writing data to,
		// 0 designates that frame has never been written to
		// 1 designates it has at least once while in the PageTable
        int modified;
		int clockBit;
} page;
enum    repl { rdm, fifo, lru, clockEnum};
int     createMMU( int);
int     checkInMemory( int ) ;
int     allocateFrame( int ) ;
page 	rdmReplace();
page 	fifoReplace();
void 	lruBubbleSortPageTable(int usedFrameNumber);
page 	lruReplace();
page 	clockReplace();
page    selectVictim( int, enum repl) ;
const   int pageoffset = 12;            /* Page size is fixed to 4 KB */
int     numFrames ;
// The "Physical" page table size (we will hopefully only ever request up to 100 page table slots...)
#define MAX_PAGE_TABLE_SIZE 100
page 	PageTable[MAX_PAGE_TABLE_SIZE];
// page* 	PageTable;
int 	CLOCK_POINTER = 0;

/* Creates the page table structure to record memory allocation */
// Return 0 = success
// Return -1 = Fail :(
int	createMMU (int frames){
	// PageTable = malloc(sizeof(page) * frames);

	if (PageTable == NULL){
		// Requested amount of frames is too big, or memory otherwise failed to be allocated
		return -1;
	} else {
		// Otherwise, set the number of usable frames in the PageTable to numFrames
		// And initialise the PageTable with empty pages
		numFrames = frames;
		for (int i = 0; i < numFrames; i++){
			page emptyPage;
			emptyPage.pageNo = -1;
			emptyPage.modified = -1;
			emptyPage.clockBit = -1;
			PageTable[i] = emptyPage;
		}
		
    	return 0;
	}
}

/* Checks for residency: returns frame no or -1 if not found */
// Looks in PageTable if the given page is saved in it
// Returns -1 if the page isn't in there
// Returns 0 if the page is found
// Use numFrames to tell how big the PageTable is, and loop through it all
int     checkInMemory( int page_number)
{
        int     result = -1;

        // to do
		for (int i = 0; i < numFrames; i++){
			if (PageTable[i].pageNo == page_number){
				return i;
			}
		}
		// printf("Looking for %d, found %d\n", page_number, result);
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
				PageTable[i].clockBit = 1;
				return i;
			}
		}
				
        return 0;
}

// Just randomly picks a page to replace
page rdmReplace(){
	// Gets a random integer from 0 to numFrames
	int rdmNum = rand() % numFrames;
	page victim = PageTable[rdmNum];
	return victim;
}

// OPTIONAL
// Selects pages in a circular buffer-style
int	nextFifoReplacement = 0;
page fifoReplace(){
	page victim = PageTable[nextFifoReplacement];

	nextFifoReplacement++;
	if (nextFifoReplacement > numFrames){
		nextFifoReplacement = 0;
	}
	
	return victim;
}

// Picks the page that was least recently used and bubbles it up to the top of the PageTable (last index)
void lruBubbleSortPageTable(int usedFrameNumber){
	// If the number of frames is 4, the highest index in PageTable is 3, etc.
	while (usedFrameNumber < numFrames - 1){
		// printf("From frame %d to frame %d is: %d to %d\n", usedFrameNumber, usedFrameNumber + 1, PageTable[usedFrameNumber].pageNo, PageTable[usedFrameNumber + 1].pageNo);
		page temp = PageTable[usedFrameNumber + 1];
		PageTable[usedFrameNumber + 1] = PageTable[usedFrameNumber];
		PageTable[usedFrameNumber] = temp;
		usedFrameNumber++;
	}

	return;
}

page lruReplace(){
	// Always return the first page in the PageTable, as the PageTable is being sorted
	// by the above function in the main function, whenever a page is being "used" (read or written to)
	return PageTable[0];
}

// Selects first page that has a clock bit = 0
// If not 0, set the bit to 0
// If at end of page table, loop back to start
page clockReplace(){
	page victim;
	int pageFound = 0;

	while(pageFound == 0){
		victim = PageTable[CLOCK_POINTER];
		if (victim.clockBit != 0){
			victim.clockBit = 0;
		} else {
			pageFound = 1;
		}

		CLOCK_POINTER++;
		// If the number of frames is 4, the highest index in PageTable is 3, etc.
		if (CLOCK_POINTER > numFrames - 1){
			CLOCK_POINTER = 0;
		}
		
	}

	return victim;
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
		
		case clockEnum:
			victim = clockReplace();
			break;
		
		default:
			break;
		}

        return victim;
}

int main(int argc, char *argv[]){
	// Initialising random number generation
	srand(time(NULL));
	
	// Originally given code
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
                       replace = clockEnum;		 
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
	
	// printf("Initialised MMU:\n");
	// for (int i = 0; i < numFrames; i++){
	// 	printf("Page no:%d Modified:%d ClockBit:%d\n", PageTable[i].pageNo, PageTable[i].modified, PageTable[i].clockBit);
	// }

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
			// Modified for the "least recently used" algorithm, to keep track of page frames being "used"
			lruBubbleSortPageTable(frame_no);
			PageTable[frame_no].clockBit = 1;
						
		}
		else if ( rw == 'W'){
		    if (debugmode) printf( "writting   %8d \n", page_number) ;

			// printf("Before bubble sort:\n");
			// for (int i = 0; i < numFrames; i++){
			// 	printf("Page no:%d Modified:%d ClockBit:%d\n", PageTable[i].pageNo, PageTable[i].modified, PageTable[i].clockBit);
			// }
			// Modified for the "least recently used" algorithm, to keep track of page frames being "used"
			lruBubbleSortPageTable(frame_no);
			// printf("After bubble sort:\n");
			// for (int i = 0; i < numFrames; i++){
			// 	printf("Page no:%d Modified:%d ClockBit:%d\n", PageTable[i].pageNo, PageTable[i].modified, PageTable[i].clockBit);
			// }

			PageTable[frame_no].clockBit = 1;

		    // "mark page in page table as written - modified"
			// Need to do this as a bit of missing core functionality of the MMU, wasn't given to us
			// since it needs the PageTable array we had to create

			// Basically, if a frame in an actual page table is never written to, the memory address can just
			// be "discarded" since the actual memory address in the main memory never has to be modified.
			// Buuuuut.... If the memory address in the page table has had it's value modified, and it's
			// getting discarded by a replacement algorithm, its value now needs to be saved to main memory
			// beforehand.
			// So this incurs an extra "Write operation", which is recorded above by the "disk_writes++;" line
			// and is triggered by having the "modified" value in a page being non-zero
			// (We'll just set it to 1)
			PageTable[frame_no].modified = 1;
		}
		 else {
		      printf( "Badly formatted file. Error on line %d\n", no_events+1); 
		      exit (-1);
		}

		// printf("Page table after event %d:\n", no_events);
		// for (int i = 0; i < numFrames; i++){
		// 	printf("Page no:%d Modified:%d ClockBit:%d\n", PageTable[i].pageNo, PageTable[i].modified, PageTable[i].clockBit);
		// }
		

		no_events++;
        	do_line = fscanf(trace,"%x %c",&address,&rw);
	}

	printf( "total memory frames:  %d\n", numFrames);
	printf( "events in trace:      %d\n", no_events);
	printf( "total disk reads:     %d\n", disk_reads);
	printf( "total disk writes:    %d\n", disk_writes);
	printf( "page fault rate:      %.4f\n", (float) disk_reads/no_events);

	// // We're finally done with the PageTable so free its memory
	// free(PageTable);

	return 0;
}
				
