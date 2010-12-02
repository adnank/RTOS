#include "rtx.h"

void terminate()
{

	kill(KBD_I, SIGINT);

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

		kill(CRT_I, SIGINT);

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


	exit(0);
}
