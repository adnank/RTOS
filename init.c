#include "rtx.h"

//creates and initialises free envelope queue
void init_free_env_Q()
{
	//msg_env_Q *Free_Env_Queue;   declared in header file
	Free_Env_Queue = (msg_env_Q*)malloc(sizeof(msg_env_Q));
	if(Free_Env_Queue == NULL)
	{
		printf("Error allocating memory for free_env_Q\n");
		free(Free_Env_Queue);
	}
	Free_Env_Queue->free_msg_counter = 0;
	Free_Env_Queue->head = NULL;
	Free_Env_Queue->tail = NULL;
	printf("Free Envelope Queue created!\n");
}

//creates and initialises timeout queue
void init_timeout_Q()
{
	//msg_env_Q *timeout_Q;   declared in header file
	timeout_Q = (msg_env_Q*)malloc(sizeof(msg_env_Q));
	if(timeout_Q == NULL)
	{
		printf("Error allocating memory for timeout_Q\n");
		free(timeout_Q);
	}
	timeout_Q->free_msg_counter = 0;
	timeout_Q->head = NULL;
	timeout_Q->tail = NULL;
	printf("Timeout Queue created!\n");
}

//following functions allocates memory for message envelopes and initialises them
void init_env()
{
	Envelope *env_ptr;
	init_free_env_Q();		//creates and initializes free envelope queue
	int i;
	for(i = 0; i < NUM_OF_ENVELOPES; i++)
	{
		env_ptr = (Envelope *) malloc(sizeof(Envelope));
		if(env_ptr == NULL)
		{
			printf("error allocating memory for envelope!\n");
			goto cleanup;
		}
		int j;
		for (j=0;j<MSG_DATA;j++)
			env_ptr->Data[j] = NULL;				//need to check
		env_ptr->DestinationID = 0;
		env_ptr->Msg_Type = INITIAL;
		env_ptr->Next = NULL;
		env_ptr->Previous = NULL;
		env_ptr->SenderID = 0;
		env_ptr->clockticks = 0;
		env_ptr->kernelpt_next = NULL;
		env_ptr->kernelpt_previous = NULL;

		K_Enqueue_MsgEnv (env_ptr, Free_Env_Queue);
	}
	printf("All Envelopes created and enqueued onto the free envelope queue\n");

cleanup:
		free(env_ptr);
}

void init_rpq()
{
	//QueuePCB *ReadyQueue [NUM_OF_PRIORITY]; declared in header file

	int i,j;
	for(i = 0; i < NUM_OF_PRIORITY; i++)
	{
		ReadyQueue[i] = (QueuePCB*) malloc(sizeof(QueuePCB));
		if(ReadyQueue[i] == NULL)
		{
			printf("error allocating memory for ready Queue!\n");
			for(j = i-1; j >= 0; j--)
				free(ReadyQueue[j]);
			break;
		}
		ReadyQueue[i]->Head = NULL;
		ReadyQueue[i]->Tail = NULL;
	}
	printf("Ready Process Queue Created and Initialized!\n");
}

//creates and initialises Blocked_On_Resource queue
void init_blkOnRsc_Q()
{
	//QueuePCB *Blocked_On_Resources [NUM_OF_PRIORITY]; declared in header file

	int i,j;
	for(i = 0; i < NUM_OF_PRIORITY; i++)
	{
		Blocked_On_Resources[i] = (QueuePCB*) malloc(sizeof(QueuePCB));
		if(Blocked_On_Resources[i] == NULL)
		{
			printf("error allocating memory for Blocked_On_Resources Queue!\n");
			for(j = i-1; j >= 0; j--)
				free(Blocked_On_Resources[j]);
			break;
		}
		Blocked_On_Resources[i]->Head = NULL;
		Blocked_On_Resources[i]->Tail = NULL;
	}
	printf("Blocked_On_Resources Queue Created and Initialized!\n");
}

//creates and initialises Blocked_On_Envelope queue
void init_blkOnEnv_Q()
{
	//QueuePCB *Blocked_On_Envelope [NUM_OF_PRIORITY]; declared in header file

	int i,j;
	for(i = 0; i < NUM_OF_PRIORITY; i++)
	{
		Blocked_On_Envelope[i] = (QueuePCB*) malloc(sizeof(QueuePCB));
		if(Blocked_On_Envelope[i] == NULL)
		{
			printf("error allocating memory for Blocked_On_Resources Queue!\n");
			for(j = i-1; j >= 0; j--)
				free(Blocked_On_Envelope[j]);
			break;
		}
		Blocked_On_Envelope[i]->Head = NULL;
		Blocked_On_Envelope[i]->Tail = NULL;
	}
	printf("Blocked_On_Envelope Queue Created and Initialized!\n");
}

//following function allocates memory for the pcb
void init_processes()
{
	int i, size;
	//QueuePCB *PCBList;  declared in header file
	jmp_buf temp;			//used for setjmp and lonjmp

	PCBList = (QueuePCB*) malloc(sizeof(QueuePCB));

	init_rpq();
	NewPCB *pcb_ptr;
	for(i=0; i < NUM_OF_PROC; i++)
	{
		pcb_ptr = (NewPCB*)malloc(sizeof(NewPCB));
		if(pcb_ptr == NULL)
		{
			printf("Error allocating memory for PCB\n");
			free(pcb_ptr);
		}

		pcb_ptr->Kernelpt_Next = NULL;
		pcb_ptr->Kernelpt_Previous = NULL;
		pcb_ptr->Next = NULL;
		pcb_ptr->Previous = NULL;
		pcb_ptr->Own = (msg_env_Q*)malloc(sizeof(msg_env_Q));
		pcb_ptr->recievelist = (msg_env_Q*)malloc(sizeof(msg_env_Q));
		pcb_ptr->Priority = init_table[i].priority;
		pcb_ptr->ProcID = init_table[i].process_id;
		pcb_ptr->State = init_table[i].proc_status;
		pcb_ptr->address = init_table[i].proc_address;
		size = init_table[i].stack_size;
		pcb_ptr->Stack = ((char *) malloc(size))+ size - 8; 	//stacks grow down

		//head and tail of 'Own' which is a msg_env_Q struct
		pcb_ptr->Own->head = NULL;
		pcb_ptr->Own->tail = NULL;
		//head and tail of 'recievelist' which is a msg_env_Q struct
		pcb_ptr->recievelist->head = NULL;
		pcb_ptr->recievelist->tail = NULL;

		K_Enqueue_PCBLIST(pcb_ptr, PCBList);

		if(pcb_ptr->State == READY)
		{
			K_Enqueue_PCB(pcb_ptr, ReadyQueue[pcb_ptr->Priority]);
		}
		else if(pcb_ptr->State == BLK_ON_ENV)
		{
			K_Enqueue_PCB(pcb_ptr, Blocked_On_Envelope[pcb_ptr->Priority]);
		}
		else if(pcb_ptr->State == BLK_ON_RSC)
		{
			K_Enqueue_PCB(pcb_ptr, Blocked_On_Resources[pcb_ptr->Priority]);
		}
	}

	NewPCB *temp_proc = PCBList->Head;
	do{
		if(setjmp(temp) == 0)
		{

		    init_stack = temp_proc->Stack;
		 			__asm__ ("movl %0,%%esp":"=m" (init_stack));

			if(setjmp(temp_proc->jbContext) == 0)
			{
				longjmp(temp, 1);
			}
			else
			{
				start_address start = current_process->address;
				start();
			}
		}
		temp_proc = temp_proc->Kernelpt_Next;
	}while(temp_proc != NULL);
	current_process = get_PCB(init_table[3].process_id);
	if(current_process == NULL)
        printf("current_process is NULL!\n");
    longjmp(current_process->jbContext, 1);
}


//creates the initialization table
void initialize_table()
{

	init_table[0].process_id = PROC_A;
	init_table[0].priority = P_P1;
	init_table[0].stack_size = MAX_STACKSIZE;
	init_table[0].proc_status = BLK_ON_ENV;
	init_table[0].proc_address= add_procA;		//(void*)(&add_procA);

	init_table[1].process_id = PROC_B;
	init_table[1].priority = P_P1;
	init_table[1].stack_size = MAX_STACKSIZE;
	init_table[1].proc_status = READY;
	init_table[1].proc_address= add_procB;		//(void*)(&add_procB);

	init_table[2].process_id = PROC_C;
	init_table[2].priority = P_P1;
	init_table[2].stack_size = MAX_STACKSIZE;
	init_table[2].proc_status = READY;
	init_table[2].proc_address= add_procC;		//(void*)(&add_procC);

	init_table[3].process_id = NULL_PROC;
	init_table[3].priority = P_P3;
	init_table[3].stack_size = MAX_STACKSIZE;
	init_table[3].proc_status = READY;
	init_table[3].proc_address= add_pNull;		//(void*)(&add_pNull);

	init_table[4].process_id = KBD_I;
	init_table[4].priority = P_IPROCESS;
	init_table[4].stack_size = MAX_STACKSIZE;
	init_table[4].proc_status = IPROCESS;
//	init_table[4].proc_address= add_KBD;		//(void*)(&add_KBD);

	init_table[5].process_id = CRT_I;
	init_table[5].priority = P_IPROCESS;
	init_table[5].stack_size = MAX_STACKSIZE;
	init_table[5].proc_status = IPROCESS;
//	init_table[5].proc_address= add_CRT;		//(void*)(&add_CRT);

	init_table[6].process_id = TIMER_I;
	init_table[6].priority = P_IPROCESS;
	init_table[6].stack_size = MAX_STACKSIZE;
	init_table[6].proc_status = IPROCESS;
//	init_table[6].proc_address= add_TIMER;		//(void*)(&add_TIMER);

	init_table[7].process_id = PROC_CCI;
	init_table[7].priority = P_P0;
	init_table[7].stack_size = MAX_STACKSIZE;
	init_table[7].proc_status = READY;
//	init_table[7].proc_address= add_CCI;		//(void*)(&add_CCI);

	init_table[8].process_id = PROC_WALL_CLK;
	init_table[8].priority = P_P0;
	init_table[8].stack_size = MAX_STACKSIZE;
	init_table[8].proc_status = READY;
//	init_table[8].proc_address= add_CLK;		//(void*)(&add_CLK);

	printf("initialization table created!\n");
}


//creating and initialising Trace Arrays
void init_TraceArrays()
{
	int i,j;
	for(i = 0; i < TRACE_ARRAY; i++)
	{
		Recieve_Trace_Array[i] = (TraceArray*) malloc(sizeof(TraceArray));
		Send_Trace_Array[i] = (TraceArray*) malloc(sizeof(TraceArray));
		if(Recieve_Trace_Array[i] == NULL)
		{
			printf("error allocating memory for Receive Trace array!\n");
			for(j = i-1; j >= 0; j--)
				free(Recieve_Trace_Array[j]);
            break;
		}
		Recieve_Trace_Array[i]->DestintionID = 0;
		Recieve_Trace_Array[i]->Msg_Type = 0;
		Recieve_Trace_Array[i]->SenderID = 0;
		Recieve_Trace_Array[i]->timestamp = 0;

		if(Send_Trace_Array[i] == NULL)
		{
			printf("error allocating memory for Send Trace array!\n");
			for(j = i-1; j >= 0; j--)
				free(Recieve_Trace_Array[j]);
            break;
		}
		Send_Trace_Array[i]->DestintionID = 0;
		Send_Trace_Array[i]->Msg_Type = 0;
		Send_Trace_Array[i]->SenderID = 0;
		Send_Trace_Array[i]->timestamp = 0;
	}
	printf("Trace Arrays Created and Initialized!\n");
}

//creating and initialising I/O Buffers
void init_ioBuffers()
{
	//io_buffer *output_buffer;		//declared in header file
	//io_buffer *input_buffer;		//declared in header file
	output_buffer = (io_buffer*) malloc(sizeof(io_buffer));
	input_buffer = (io_buffer*) malloc(sizeof(io_buffer));

	if(output_buffer == NULL)
	{
		printf("Error allocating memory for output buffer\n");
		free(output_buffer);
	}

	output_buffer->Length = 0;
    printf("Output Buffer Created and Initialized!\n");

	if(input_buffer == NULL)
	{
		printf("Error allocating memory for input buffer\n");
		free(input_buffer);
	}

	input_buffer->Length = 0;
	printf("Input Buffer Created and Initialized!\n");
}



