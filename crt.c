#include "rtx.h"

//Details:
//========================================================
//The crt.c child process is run in order to empty the output
//buffer to crt_iproc and push it to screen.

int bufsize = BUFFER_SIZE;

void crt_die(int signal)
{
	exit(0);
}

void main (int argc, char *cbuffer[])
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
	  keyboard_die(0);
    }

    output_buffer = mmap_pointer;
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

}

