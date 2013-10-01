#include "shell_header.h"

#define SIZE 1001

char history[SIZE][512];
int first = -1;
int last = -1;

int add_history ( char * command ) {
	if( last == -1 )
		first = last = 0;

	//char * newline = strtok ( command , "\n" );

	history[last][0] = '\0';
	strcpy ( history[last] , command );

	if ( last == 0 ) {
		if ( strcmp ( history[SIZE - 1] , command ) == 0 )
			return 0;
	}
	else {
		if ( strcmp ( history[last - 1] , command ) == 0 )
			return 0;
	}

	last = (last + 1 ) % SIZE;

	if( last == first ) {
		first = (first + 1) % SIZE;
	}

	return 0;
}

int is_string_present ( char * search_in , char * search_str ) {

	int i = 0;

	while ( search_in[i] != '\0' && search_str[i] != '\0' ) {
		if ( search_in[i] != search_str[i] )
			return 0;
		i++;
	}

	if( search_in[i] == '\0' )
		return 0;

	return 1;
}

int get_command ( char * command , char * command_exec ) {

	if( first == -1 && last == -1 ) {
		return -2;
	}

	int i = 0 , k = 0;
	char temp[16];
	int n_flag = 0;
	int s_flag = 0;

	for ( i = 1 ; command[i] != '\0' ; i++ ) {
		if( (i == 1 && command[i] == '-') || (command[i] >= '0' && command[i] <= '9') ) {
			temp[k++] = command[i];
			n_flag = 1;
		}
		else {
			s_flag = 1;
			break;
		}
	}
	temp[k] = '\0';
	int index = atoi ( temp );
	

	if ( n_flag == 1 ) {

		/* There is number given in ! */
		if ( index > 0 )
			if ( (first + index - 1 ) >= last || (first + index - 1 ) < first )
				return -2;
		if ( index == 0 )
				return -2;

		if ( index < 0 ) {
			if ( (last + index ) >= last || (last + index ) < first )
				return -2;
		}
		if ( index > 0 ) {
			strcpy( command_exec , history[ (first + index - 1) % SIZE ] );
		}
		else {
			strcpy( command_exec , history[ (last + index + SIZE ) % SIZE ] );
		}

		char * str = &command[i];
		strcat ( command_exec , str );

	}
	if ( s_flag == 0 ) {

		/* There is no string given in ! along with number */

		return 0;

	}
	else {
		/* There is only string given in ! */
		char *str = &command[1];
		for ( i = ( last > 0 ) ? last - 1 : SIZE ; i != first ; i = (i==0) ? (SIZE-1) : ((i-1) % SIZE)  ) {
			if ( is_string_present( history[i] , str ) ) {
				strcat ( command_exec , history[i] );
				break;
			}
		}
		if ( first != -1 ) {
			if ( is_string_present( history[first] , str ) ) {
				strcat ( command_exec , history[first] );
			}
		}
	}
	return 0;
}

void print_history () {
	int i = 0 , index = 1;

	if ( first == -1 && last == -1 )
		return;

	for ( i = first ; i != last ; i = (i + 1) % SIZE ) {
		printf("%4d. %s\n" , index , history[i]);
		index++;
	}
}

int read_history () {

	int i = 0 ;

	char *path = (char *) calloc ( 1024 , sizeof(char) );


	char * home = get_local ( "HOME" );
	if ( home == NULL ) {
		home = get_env ( "HOME" );
	}
	strcat ( path , home );

	if ( path[strlen ( path ) - 1] != '/' ) {
		strcat ( path , "/" );
	}

	strcat ( path , ".devilo_history" );


	int fd = open ( path ,  O_RDONLY );
	if ( fd == -1 )
		return -1;
	int read_t = 0;
	char ch;
	char * buffer = (char *) malloc ( 1024 * sizeof(char) );

	while ( ( read_t = read (fd , &ch , 1) ) != 0 ) {
		if ( ch == '\n') {
			buffer[i] = '\0';
			add_history ( buffer );
			i = -1;
		}
		else {
			buffer[i] = ch;
		}
		i++;
	}

	if( i != 0 ) {
		buffer[i] = '\0';
		add_history(buffer);
	}

	free ( buffer );
	return 0;
}

int write_history () {
	
	int i = 0 , index = 1;

	char *path = (char *) malloc ( 1024 * sizeof(char) );

	if ( first == -1 && last == -1 )
		return -1;

	char * home = get_local ( "HOME" );
	if ( home == NULL ) {
		home = get_env ( "HOME" );
	}

	strcpy ( path , home );

	if ( path[strlen ( path ) - 1] != '/' ) {
		strcat ( path , "/" );
	}

	strcat ( path , ".devilo_history" );
	int fd = open ( path ,  O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR );
	for ( i = first ; i != last ; i = (i + 1) % SIZE ) {
		if( i != first )
			write(fd , "\n" , 1 );
		write(fd , history[i] , strlen( history[i] ) );
		index++;
	}
	close ( fd );

	return 0;
}
