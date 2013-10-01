#include "shell_header.h"

#define COMMANDS	internal_cmd

#define SHELL_LINE_S	1024
#define MAX_ALIAS	32

int main( int argc , char * argv[] ) { 

	char * shell_line;
	char * hostname;
	extern char *internal_cmd[];

	shell_line = (char *)  calloc ( SHELL_LINE_S , sizeof(char) );
	hostname = (char *)  calloc ( SHELL_LINE_S , sizeof(char) );

	signal(SIGINT , catch_int);

	rc_init();
	read_history();

	do {	
		print_prompt(hostname);

		int len = readline( shell_line );

		if ( len == 0 )
			continue;

		if ( strcmp ( COMMANDS[ EXIT ] , shell_line ) == 0 ) {
			execute_exit();
			break;
		}

		execute( shell_line , 0 );

	} while( 1 );

	free( shell_line );

	return 0;

}
