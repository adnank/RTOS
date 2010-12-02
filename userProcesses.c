#include "rtx.h"

void add_procA()
{
	Envelope* A = recieve_message();
	printf("Process A has been releasef from blocked on envelope queue/n");
	int B = release_msg_env(A);
	static int num = 0;
	do
	{
		A = request_msg_Env();
		A->Msg_Type = COUNTER_REPORT;
		A->Data[0] = num;
		send_message(PROC_B,A);
		num++;
		release_processor();
	}
	while (1);
}


void add_procB()
{
	Envelope* message;
	do
	{
		message = recieve_message();
		send_message(PROC_C, message);
	}
	while(current_process->recievelist->head != NULL );
	release_processor();
}

void add_procC()
{

	Envelope* msgenv;
	int num1 = msgenv->Data[0];
	msg_env_Q* local_queue;
	local_queue->head = NULL;
	local_queue->tail= NULL;
	int S=0;
	do
	{
	// check for message on local queue first
		msgenv = K_Dequeue_MsgEnv(local_queue);
		if (msgenv == NULL)
			msgenv = recieve_message();
		if(msgenv->Msg_Type == COUNTER_REPORT)
		{
		if ((num1) % 20 == 0)
			{
				strcpy(msgenv->Data,"Process C\n");
				msgenv->Msg_Type = CONSOLE_OUT;
				send_console_chars(msgenv);
				//
				while( S == 0 )
				{
					msgenv = recieve_message();
					//if(msgenv == NULL)

					if(msgenv->Msg_Type ==  DISPLAY_ACK)
					{
						request_delay(1000, WAKEUP_CODE, msgenv);//1000 = 10 secs
					}
					if(msgenv->Msg_Type ==  WAKEUP_CODE)
					{
						S++;
					}
					if((msgenv->Msg_Type ==  !WAKEUP_CODE) || (msgenv->Msg_Type ==  !DISPLAY_ACK))
					{
					K_Enqueue_MsgEnv(msgenv, local_queue );
					}

				}
			}

	}
		release_msg_env(msgenv);
		release_processor();
	}
	while(1);
}











