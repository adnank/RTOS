/*
 * rtx_iproc.c
 *
 *  Created on: 2010-11-18
 *      Author: Joel J
 */

#include "rtx.h"


int variablesignal;

//Details:
//==========================================================
//The iprocesses are called depending on sigalrm, sigusr1,
//sigusr2, which are sent because of ualarm or the keyboard or crt helpers
//The interrupt handler decides which i processes is called
//based on the signal function


void interrupt_handler(variablesignal){
	//save current_process context
//	current_process->State = INTERRUPT;
	Prev_Proc = current_process;
//	printf("previous process had PID %d\n",Prev_Proc->ProcID);
//	printf("the previous process has state %d\n",Prev_Proc->State);

atomic(ON);
	//select i process based on interrupt source
	switch(variablesignal){

/*	case (SIGALRM) : // i_process_timer;
	current_process = get_PCB(TIMER_I);
	i_process_timer();
	//i_process_timer state is set as executing
	//context_switch(current_process,);
	break;
*/

    case (SIGUSR1) : // i_process_kb
	printf("Keyboard signal received and converting to envelope...\n");
	current_process = get_PCB(KBD_I);
	//i_process_kb state is set as executing
	context_switch(Prev_Proc, current_process);

//	i_process_kb();
	break;

/*	case (SIGUSR2) : // i_process_crt;
	current_process = get_PCB(CRT_I);
	//i_process_crt state is set as executing
	context_switch(Prev_Proc, current_process);
	break;
*/

//	case D: // i_process_terminate
	}

atomic(OFF);
}
 //i_process_timer()
//=================================================================
//The following I-Process is responsible for putting the keystrokes
//from a buffer into a message envelope which can be passed.

void i_process_timer()
{
    while(1){
		time_increment();
		printf("I PROCESS TIMER CALLED\n");
		//Envelope *TimeOUTPUT=K_request_msg_env;
		//TimeOUTPUT->Data[0]=hh;
		//TimeOUTPUT->Data[1]=':';
		//TimeOUTPUT->Data[2]=mm;
		//TimeOUTPUT->Data[3]=':';
		//TimeOUTPUT->Data[4]=ss;
		//send_console_chars(TimeOUTPUT);
		Envelope *timeout_request;
		timeout_request = K_recieve_message();
		while (timeout_request != NULL)
		{
			K_Sort_Envelope_Enqueue(timeout_request);
			timeout_request = K_recieve_message();
		}
		if (timeout_Q->free_msg_counter > 0)
		{
			Envelope* Temp=timeout_Q->head;
			while (Temp->Next!=NULL)
			{
				Temp->clockticks--;
				Temp=Temp->Next;
			}
			Temp=timeout_Q->head;
			while (Temp->clockticks==0)
			{
				Envelope* Send;
				Send=K_Dequeue_MsgEnv(timeout_Q);
				timeout_Q->free_msg_counter--;
				Send->Msg_Type=2;
				K_send_message (Send->SenderID,Send);
				Temp=Temp->Next;
			}
		}
		context_switch(current_process, Prev_Proc);
    }
}

//i_process_kb()
//================================================================
//The following I-Process is responsible for putting the keystrokes
//from a buffer into a message envelope which can be passed.

void i_process_kb()
{
Envelope *input;
//Envelope *input1;
int j;
caddr_t mmap_ptr;
mmap_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 lets O/S choose */
        BUFFER_SIZE,/* How many bytes to mmap */
        PROT_READ | PROT_WRITE, /* Read and write permissions */
        MAP_SHARED,    /* Accessible by another process */
        fid1,           /* which file is associated with mmap */
        (off_t) 0);    /* Offset in page frame */
    if (mmap_ptr == MAP_FAILED)
    {
      printf("Memory Map Initialization Failed!!!!!!!!!!!!!!!... Sorry!\n");
      die();
    }

    input_buffer = (io_buffer *) mmap_ptr;
//input1 = K_recieve_message();
	//while(current_process->recievelist->head!=NULL) // endless loop
	while(1)
	{
			input = K_recieve_message();
			if (input!=NULL)
			{
			    int i;
			    char firstletter;
			    firstletter = 's';
			for (j=0;j<input_buffer->Length;j++)
			{
                input->Data[j] = input_buffer->buffer[j];
			}
			input_buffer->Read = 0;
			input_buffer->Length = 0;

//Store the string of characters from the kb buffer into msg_env
//Set msg_env message subject field to console_input send message envelope to invoking process

		input->DestinationID = input->SenderID;
		printf("the dest pid is %d\n",input->SenderID);
		input->SenderID = KBD_I;
		input->Msg_Type = KB_INPUT;
		printf("<%c>\n\n",input->Data[0]);
		K_Enqueue_MsgEnv(input,current_process->Own);
		K_send_message(input->DestinationID, input);


			}
        context_switch(current_process,Prev_Proc);

	}
	}

//i_process_crt()
//=================================================================
//The following I-Process is responsible for outputting the current
//buffer to the screen

void i_process_crt()
{
Envelope *output;
int i;
caddr_t mmap_ptr;

		mmap_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 lets O/S choose */
		    BUFFER_SIZE,/* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fid2,           /* which file is associated with mmap */
		    (off_t) 0);    /* Offset in page frame */
	    if (mmap_ptr == MAP_FAILED)
	    {
	      printf("Memory Map Initialization Failed!!!!!!!!!!!!!!!... Sorry!\n");
		  die();
	    }

	    output_buffer = (io_buffer *) mmap_ptr;

	while(1){ // endless loop
			if (current_process->recievelist->head != NULL)
			{

				output=K_recieve_message();
				i = 0;
				while(output->Data[i] != '\O')
				{
				    output_buffer->buffer[i]=output->Data[i];
					printf("%c\n",output_buffer->buffer[i]);
                    i++;
				}

				output_buffer->Length = i;
				output_buffer->Read = 0;
			}
			else{
				output_buffer->Length=0;
				context_switch(current_process,Prev_Proc); //NEED RELEASE PROCESSOR?????????????
			}
	}

}

