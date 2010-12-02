
#include "rtx.h"

void die()
{
	cleanup();
	printf ("Signal Prompts Immediate Exit... Sorry\n");
	exit(0);
}

//CLEANUP FUNCTION I DON't KNOW WHERE THIS GOES
void cleanup()
{
	kill(inputpid, SIGINT);

			if (munmap(mmap_pointer, BUFFER_SIZE) == -1)
			{
				printf("Bad Cleanup - Munmap\n");
			}

			if(close(fid)== -1)
			{
				printf("Bad Cleanup - Munmap Close\n");
			}

			if(unlink(fname) == -1)
			{
				printf("Bad Cleanup - Unlink\n");
			}

}
//PUT THIS IN INITIALIZATION
void init_helperprocess()
{
	fname1 = (char *) malloc(sizeof(char)*20);
	fname2 = (char *) malloc(sizeof(char)*20);
    caddr_t mmap_ptr;
    io_buffer * input_buffer;

	//Initialize Keyboard Helper
	fname1 = "KeyboardBuffer";
	fid1 = open(fname1, O_RDWR | O_CREAT, (mode_t) 0755);

  if (fid1 < 0){
    printf("Bad Open of mmap file <%s>\n", fname1);
	exit(0);
    };

	int status = ftruncate(fid1, BUFFER_SIZE);

if (status){
      printf("Failed to ftruncate the file <%s>, status = %d\n", fname1, status );
      exit(0);
  }



	//Excel Function
	    char kbarg1[15];
	    char kbarg2[15];
	    int parent_pid;
	    parent_pid = getpid();

	    sprintf(kbarg1, "%d", parent_pid);
	    sprintf(kbarg2, "%d", fid1);

//	mmap_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 lets O/S choose */
//		    BUFFER_SIZE,/* How many bytes to mmap */
//		    PROT_READ | PROT_WRITE, /* Read and write permissions */
//		    MAP_SHARED,    /* Accessible by another process */
//		    fid1,           /* which file is associated with mmap */
//		    (off_t) 0);    /* Offset in page frame */
//	    if (mmap_ptr == MAP_FAILED)
//	    {
//	      printf("Memory Map Initialization Failed!!!!!!!!!!!!!!!... Sorry!\n");
//		  die();
//	    }

 //       input_buffer = (io_buffer *) mmap_ptr;

   //     printf("THIS IS IOBUFFER LENGTH%d\n",input_buffer->Length);

	    pid = fork();
	   if (pid == 0)
	   {
	        int a  = execl("./Keyboard", "Keyboard", kbarg1, kbarg2, (char *)0);
	        printf("%d\n", a);
	            fprintf(stderr,"FAILED ON KEYBOARD EXECUTING, errno %d\n",errno);
	            inputpid = getpid();
	            cleanup();
	            exit(1);
	   }

    sleep(1);
printf("THIS IS IOBUFFER LENGTH%d\n",input_buffer->Length);

	//Initialize CRT Helper
	fname2 = "CRTBuffer";
	fid2 = open(fname2, O_RDWR | O_CREAT, (mode_t) 0755);
	ftruncate(fid2, BUFFER_SIZE);

//		mmap_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 lets O/S choose */
//		    BUFFER_SIZE,/* How many bytes to mmap */
//		    PROT_READ | PROT_WRITE, /* Read and write permissions */
//		    MAP_SHARED,    /* Accessible by another process */
//		    fid2,           /* which file is associated with mmap */
//		    (off_t) 0);    /* Offset in page frame */
//	    if (mmap_ptr == MAP_FAILED)
//	    {
//	      printf("Memory Map Initialization Failed!!!!!!!!!!!!!!!... Sorry!\n");
//		  die();
//	    }

//	    output_buffer = (io_buffer *) mmap_ptr;


	//Excel Function
	char crtarg1[15];
	char crtarg2[15];

	parent_pid = getpid();
	sprintf(crtarg1, "%d", parent_pid);
	sprintf(crtarg2, "%d", fid2);

	pid = fork();
	if (pid == 0)
	{
		execl("./CRT", "CRT", crtarg1, crtarg2, (char *)0);
		fprintf(stderr,"FAILED AT CRT EXECUTING, errno %d\n",errno);
		inputpid = getpid();
		cleanup();
		exit(1);
	}


}


void init_signals()
{
//catch illegal signals and terminate program or
//catch sigalrm, sigusr1, sigusr2 and invoke interrupt handler

	sigset(SIGINT,die); //kill signals
	sigset(SIGBUS,die); //bus errors
	sigset(SIGHUP,die);
	sigset(SIGILL,die); //illegal instruction
	sigset(SIGQUIT,die);
	sigset(SIGABRT,die);
	sigset(SIGTERM,die);
	//sigset(SIGSEGV,die); //seg faults
	sigset(SIGALRM, interrupt_handler); //alarm
	sigset(SIGUSR1, interrupt_handler); //keyboard
	sigset(SIGUSR2, interrupt_handler); //screen

//ualarm(100000,100000); //initialize alarm to signal every 100ms
		printf("Signals have been initialized!\n");
}
