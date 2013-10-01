#include "shell_header.h"

int job_st[1024];
char job_name[1024][1024];
int job_t = -1;

int add_job ( int process_id , char * job_n ) {

	job_t++;
	job_st [job_t] = process_id;
	strcpy ( job_name[job_t] , job_n );
	return job_t + 1;
}

int remove_job ( int job_num ) {

	if ( job_t == -1 ) {
		printf("devilo: fg: current: no such job\n");
		return 0;
	}

	if ( (job_num - 1) > job_t ) {
		printf("devilo: fg: %d: no such job\n" , job_num );
		return 0;
	}

	if ( job_num == 0 ) {
		int status = 0;
		waitpid ( job_st[job_t] , &status , 0 );

		printf("devilo: fg: job has terminated\n[%d]+\tDone\t%s\n" , job_t + 1 , job_name[job_t] );

		job_t --;
	}
	else {
		int i = 0 , status = 0;

		waitpid ( job_st[job_num - 1] , &status , 0 );

		printf("devilo: fg: job has terminated\n[%d]+\tDone\t%s\n" , job_num , job_name[job_num - 1]);

		for ( i = job_num ; i <= job_t ; i++ ) {

			job_st[ i - 1 ] = job_st[i];
			strcpy ( job_name[i-1] , job_name[i] );
		}
		job_t --;
	}
	return 0;
}
