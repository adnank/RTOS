#include "rtx.h"


//following is the main function for the RTX Project

int main()
{
	int i;
	printf("Starting.....!\n");

	printf("Initializing Signals!\n");
	init_signals();

	time_initialize();

	initialize_table();
//	printf("Initialization Table created and initialised!\n");

	init_env();
	init_timeout_Q();
	init_blkOnRsc_Q();
	init_blkOnEnv_Q();
//	printf("All Queues and Envelope Queues Initialized!\n");
    init_TraceArrays();
	init_ioBuffers();

	init_processes();
//	printf("Initialising Processes!\n");


//	init_helperprocess();
//	printf("KBD and CRT Processes initialized and forked!\n");

	return 0;
}



