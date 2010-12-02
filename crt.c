#include "rtx.h"

//Details:
//========================================================
//The crt.c child process is run in order to empty the output
//buffer to crt_iproc and push it to screen.

int bufsize = BUFFER_SIZE;

void out_die(int signal)
{
	exit(0);
}

/*
int main (int argc, char *cbuffer[])
{

	int parent_pid, fid;
	char c;
	sigset(SIGINT,crt_die);
	caddr_t mmap_pointer;

	scanf(cbuffer[1], "%d", &parent_pid);
	scanf(cbuffer[2], "%d", &fid);

	mmap_pointer = mmap((caddr_t) 0, bufsize, PROT_READ | PROT_WRITE, MAP_SHARED, fid, (off_t) 0);

    if (mmap_pointer == MAP_FAILED)
    {
	  crt_die(0);
    }

    output_buffer = (io_buffer*) mmap_pointer;
    output_buffer->Length = 0;
    output_buffer->Read = 0;

    while(1)
    {

    	kill(parent_pid, SIGUSR2);
    	usleep(100000);

    	if (output_buffer->Read == 0)
    	{

    		printf("%s", output_buffer->buffer);
    		output_buffer->Length =0;
    		output_buffer->Read = 0;
    	}
    }
    return 0;
}
*/

int main (int argc, char * argv[])
{
	int parent_pid, fid;

	caddr_t mmap_ptr;
	io_buffer* in_mem_p;
	char c;


	// if parent tells us to terminate, then clean up first
	sigset(SIGINT,out_die);

	// get id of process to signal when we have input
	// and the file id of the memory mapped file
	// i.e. process input arguments 
	sscanf(argv[1], "%d", &parent_pid );
	sscanf(argv[2], "%d", &fid );  // get the file id

	// attach to shared memory so we can pass input to 
	// keyboard interrupt handler
	
	mmap_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 lets O/S choose */
		    bufsize,/* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fid,           /* which file is associated with mmap */
		    (off_t) 0);    /* Offset in page frame */
    if (mmap_ptr == MAP_FAILED){
      printf("Child memory map has failed, KB is aborting!\n");
	  out_die(0);
    }
	
	in_mem_p = (io_buffer *) mmap_ptr; // now we have a shared memory pointer
//	input_buffer= (io_buffer*) in_mem_p;
	// read keyboard
//	buf_index = 0;
	//in_mem_p->Read = 0; 
	do
	{
		kill(parent_pid, SIGUSR2);
    		usleep(100000);		

		 if (in_mem_p->Read == 0)
    		{
		int i;		
		for(i=0;i<in_mem_p->Length; i++){
    		printf("%s", in_mem_p->buffer[i]);
		}
    		in_mem_p->Length =0;
    		in_mem_p->Read = 1;//DOUBLE CHECK THIS IS 1 or 0
    		}
	
	}
		
	while(1);  //an infinite loop - exit when parent signals us

} // keyboard

