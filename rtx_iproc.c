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
	// save current_process context
	current_process->State = INTERRUPT;
	Prev_Proc = current_process;

atomic(ON);
	//select i process based on interrupt source
	switch(variablesignal){

	case (SIGALRM) : // i_process_timer;
	current_process = get_PCB(TIMER_I);
	i_process_timer();
	//i_process_timer state is set as executing
	//context_switch(current_process,);
	break;


    case (SIGUSR1) : // i_process_kb
	printf("Keyboard signal received\n");
	current_process = get_PCB(KBD_I);
	//i_process_kb state is set as executing
	context_switch(Prev_Proc, current_process);
	break;

	case (SIGUSR2) : // i_process_crt;
	current_process = get_PCB(CRT_I);
	//i_process_crt state is set as executing
	//context_switch(current_process, );
	break;

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
		time_increment();
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

//i_process_kb()
//=================================================================
//The following I-Process is responsible for putting the keystrokes
//from a buffer into a message envelope which can be passed.

void i_process_kb()
{

Envelope *input;
int j;
	while(current_process->recievelist->head!=NULL) // endless loop
	{
			input = K_recieve_message();
			for (j=0;j<input_buffer->Length;j++)
			{
				 input->Data[j] = input_buffer->buffer[j];
			}
//Store the string of characters from the kb buffer into msg_env
//Set msg_env message subject field to console_input send message envelope to invoking process
		input->DestinationID = input->SenderID;
		input->SenderID = KBD_I;
		input->Msg_Type = KB_INPUT;
		K_Enqueue_MsgEnv(input,current_process->Own);
		K_send_message(input->DestinationID, input);
	}
	input_buffer->Length = 0;
	input_buffer->Read = 0;
	context_switch(current_process,Prev_Proc);
}

//i_process_crt()
//=================================================================
//The following I-Process is responsible for outputting the current
//buffer to the screen

void i_process_crt()
{
Envelope *output;
int i;

	while(1){ // endless loop
			if (current_process->recievelist->head != NULL)
			{
				i=0;
				output=K_recieve_message();
				output_buffer->Length=4;
				for(i;i<output_buffer->Length;i++)
				{
					output_buffer->buffer[i]=output->Data[i];
					printf("%c\n",output_buffer->buffer[i]);
				}
				output_buffer->Length=i+1;
			}
			else{
				output_buffer->Length=0;
				context_switch(current_process,Prev_Proc); //NEED RELEASE PROCESSOR?????????????
			}
	}

}

