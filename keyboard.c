
#include "rtx.h"




//Details:
//=========================================================
//The keyboard.c file is the keyboard helper child process which
//is constantly checking for input signals. If the input signal
//is a carriage return on exceeds the limit the kb_iprocess is
//called which empties the kb_buffer which was sent.

int bufsize = BUFFER_SIZE;
int buf_index;

void in_die(int signal){
	exit(0);
}

/*int main (int argc, char *kbuffer[]){

	int parent_pid;
	int fid;
	char c;
//	io_buffer * input_buffer;
	sigset(SIGINT,keyboard_die);
	caddr_t mmap_pointer;

	scanf(kbuffer[1], "%d", &parent_pid);
	scanf(kbuffer[2], "%d", &fid);

printf("%d\n", fid);
printf("%d\n", parent_pid);

mmap_pointer = mmap((caddr_t) 0,   // Memory Location, 0 lets O/S choose 
		    bufsize,// How many bytes to mmap 
		    PROT_READ | PROT_WRITE, // Read and write permissions 
		    MAP_SHARED,    // Accessible by another process 
		    fid,           // which file is associated with mmap 
		    (off_t) 0);    // Offset in page frame 

    if (mmap_pointer == MAP_FAILED){
printf("Child memory map has failed, KB is aborting!\n");
	  keyboard_die(0);
    };

    input_buffer = (io_buffer*) mmap_pointer;
    input_buffer->Length = 0;
    input_buffer->Read = 0;
printf("keyboard works\n");

    while(1){
    	c = getchar();
    	input_buffer->Length = input_buffer->Length+1;

    	if (c == '\n' || input_buffer->Length == bufsize){
    		input_buffer->Read = 1;
		printf("signal is being sent\n");
    		kill(parent_pid, SIGUSR1);
    		while (input_buffer->Read == 1){
    			usleep(100000);
    		}
    	}
    	else{
    		input_buffer->buffer[input_buffer->Length] = c;
    	}
    }
    return 0;

}*/

int main (int argc, char * argv[])
{
	int parent_pid, fid;

	caddr_t mmap_ptr;
	io_buffer* in_mem_p;
	char c;


	// if parent tells us to terminate, then clean up first
	sigset(SIGINT,in_die);

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
	  in_die(0);
    }
	
	in_mem_p = (io_buffer *) mmap_ptr; // now we have a shared memory pointer
//	input_buffer= (io_buffer*) in_mem_p;
	// read keyboard
	buf_index = 0;
	in_mem_p->Read = 0; 
	do
	{
		c = getchar();
		if ( c != '\n' ) {
					if( buf_index < bufsize-1 ) {
						in_mem_p->buffer[buf_index++] = c;
					} 
				} else {
					in_mem_p->buffer[buf_index] = '\0';
					in_mem_p->Read = 1;  //set ready status bit
					in_mem_p->Length=buf_index;
					printf("Keyboard Helper detects length of %d\n\n",in_mem_p->Length);
					//strcpy(input_buffer->buffer,in_mem_p->buffer);
					kill(parent_pid,SIGUSR1); //send a signal to parent	
					buf_index = 0;  // for now, just restart
					while( in_mem_p->Read == 1)
						usleep(100000);
									
				}
	
	}
		
	while(1);  //an infinite loop - exit when parent signals us

} // keyboard

