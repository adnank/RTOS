
#include "rtx.h"




//Details:
//=========================================================
//The keyboard.c file is the keyboard helper child process which
//is constantly checking for input signals. If the input signal
//is a carriage return on exceeds the limit the kb_iprocess is
//called which empties the kb_buffer which was sent.

int bufsize = BUFFER_SIZE;

void keyboard_die(int signal){
	exit(0);
}

void main (int argc, char *kbuffer[]){

	int parent_pid, fid;
	char c;
	sigset(SIGINT,keyboard_die);
	caddr_t mmap_pointer;

	scanf(kbuffer[1], "%d", &parent_pid);
	scanf(kbuffer[2], "%d", &fid);

	mmap_pointer = mmap((caddr_t) 0, bufsize, PROT_READ | PROT_WRITE, MAP_SHARED, fid, (off_t) 0);

    if (mmap_pointer == MAP_FAILED){
	  keyboard_die(0);
    }

    input_buffer = mmap_pointer;
    input_buffer->Length = 0;
    input_buffer->Read = 0;

    while(1){
    	c = getchar();
    	input_buffer->Length = input_buffer->Length+1;

    	if (c == '\n' || input_buffer->Length == bufsize){
    		input_buffer->Read = 1;
    		kill(parent_pid, SIGUSR1);
    		while (input_buffer->Read == 1){
    			usleep(100000);
    		}
    	}
    	else{
    		input_buffer->buffer[input_buffer->Length] = c;
    	}
    }

}

