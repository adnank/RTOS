/*
 * wallclock.c
 *
 *  Created on: 2010-11-28
 *      Author: Joel J
 */


#include "rtx.h"

//Details:
//===================================================================
//The time initialize() is called when the RTX is started.
//Everytime the SigALRM is sent to the i_process_timer the
//time_increment() function called which updates the clock count by one
//The time_change() function passes in values to change the clock.
//ms = microseconds ss = seconds mm = minutes hh = hours
void time_initialize()
{
	ms=00;
	ss=00;
	mm=00;
	hh=00;
	printf("%d:%d:%d\n",hh,mm,ss);
	printf("Time initialised!\n");
}

void time_increment()
{
	ms++;
	if(ms%10==0){
	ss++;
		if (ss==60){
			ss=0;
			mm++;
			if(mm==60){
				mm=0;
				hh++;
				if(hh==24){
					hh=0;
				}
			}
		}
	}
	printf("%d : %d : %d\n",hh,mm,ss);
}

void time_change(hh,mm,ss)
{
	printf("Time has been changed to %d : %d : %d\n",hh,mm,ss);
}
