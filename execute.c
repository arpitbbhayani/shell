#include "shell_header.h"

#define RC_COMMAND_S	1024
#define PIPE_READ	0
#define PIPE_WRITE	1

#define MAX_PIPES	32
#define MAX_ARG		32
#define MAX_SIZE	512

#define EXIT		0
#define ALIAS		1
#define CD		2
#define PWD		3
#define EXPORT		4
#define HISTORY		5
#define FG		6

void execute ( char * shell_line , int from_rc ) {

	int total_commands = 0;
	char *** commands;

	int * child_pid;
	int is_bg = 0;
	int i , j;

	if ( shell_line == NULL )
		return;

	commands = (char ***) calloc ( MAX_PIPES , sizeof( char** ) );

	int ret_val = exec_bang ( shell_line );
	if ( ret_val == -2 || ret_val == -1 ) {
		printf("devilo: %s: event not found\n" , shell_line);
		return ;
	}
	if ( ! from_rc && ret_val == 0 )
		add_history( shell_line );

	exec_alias ( shell_line );

	for( i = 0  ; i < MAX_PIPES ; i++ ) {
		commands[i] = (char ** ) calloc ( MAX_ARG , sizeof( char * ) );
		for( j = 0 ; j < MAX_ARG ; j++ ) {
			commands[i][j] = (char *) calloc ( MAX_SIZE , sizeof(char) );
		}
	}

	int index = strlen(shell_line) - 1;
	if ( shell_line[index] == '&' ) {
		is_bg = 1;
		shell_line[index] = '\0';
	}
	
	total_commands = get_splits ( commands , shell_line );
	child_pid = (int *) calloc ( total_commands , sizeof(int) );

	_execute( total_commands , commands , child_pid , is_bg );

}

void _execute(int num, char ***commands , int *child_pid , int is_bg ) {

	int fd_read , fd_write;
	int fd_pipe[2];
	int i;

	fd_read = INT_MIN;

	for ( i = 0 ; i < num ; i++ ) {
		if ( (i + 1) < num ) {
			/* Creating a PIPE and setting fd_write to the fd_pipe[1] i.e. the WRITE_END */
			pipe( fd_pipe );
			fd_write = fd_pipe[PIPE_WRITE];
		}
		else {
			/* If the command to be executed is not last one the setting the fd_write to STDOUT */
			fd_write = INT_MIN;
		}

		if( commands == NULL ) {
			printf("NULL\n");
		}


		child_pid[i] = __execute( commands[i] , fd_read , fd_write , is_bg);

		if ( is_bg && i == (num - 1) ) {
			/* Adding job to job_st */

			int job_n = add_job ( child_pid[i] , *commands[i] );
			printf("[%d] %d\n" , job_n , child_pid[i]);

		}


		close(fd_read);
		close(fd_write);

		fd_read = fd_pipe[PIPE_READ];

	}

}

int __execute( char **command , int fd_read , int fd_write , int is_bg ) {

	int cmd = 0;


	/* if *command is a variable assignment */

	char * split_spcl_char = strchr ( command[0] , '=' );
	if( split_spcl_char != NULL ) {
		/* case where it is an assignment */
		add_local_var ( command[0] );
		return 0;
	}

	if ( strcmp( *command , "fg" ) == 0 ) {
		int job_num = 0;
		if ( command[1] != NULL ) {
			job_num = atoi ( command[1] );
		}
		remove_job ( job_num );
		return 0;
	}

	pid_t new_process = fork();

	if ( command[0][0] == '!' ) {
		execute_bang( command );
		return 0;
	}

	if ( new_process == 0 ) {


		/* Child process will execute the execvp command */

		/* If the value of fd_read is INT_MIN then I/P : STDIN
		   If the value of fd_write is INT_MIN the O/P : STDOUT */

		/* If read is not STDIN then dup2 it to STDIN */
		if (fd_read != INT_MIN && fd_read != STDIN_FILENO ) {
			dup2( fd_read, STDIN_FILENO );
			close(fd_read);
		}

		/* If write is not STDOUT then dup2 it to STDOUT */
		if (fd_write != INT_MIN && fd_write != STDOUT_FILENO ) {
			dup2(fd_write, 1);
			close(fd_write);
		}

		/* Execute the command */
		/* This command will be executed in child so it will not interfere with parent */

		change_arg_ipop_redir ( command );
		change_arg_echo ( command );

		if ( (cmd = is_internal ( *command )) != -1  && (is_in_child ( *command )) != -1 ) {
			/* All commands that need to be executed in CHILD */
			switch ( cmd ) {
				case PWD:
					execute_pwd(command);
					break;
				case HISTORY:
					execute_history(command);
					break;
			}	
		}
		else if ( (cmd = is_internal ( *command )) == -1 ) {

			/*if ( strcmp ( *command , "echo" ) == 0 ) {
				change_arg_echo ( command );
			}*/
			execvp( *command , command );
			perror( *command );
		}

		/* Program control will reach here only when the FILE *command is not found i.e when invalid command is encountered  */

		exit(-1);

	}
	else if ( new_process > 0 ) {

		/* Parent process will return the PID of its child */
		/* Parent process will execute all the internal commands that dows not generate any output */

		/*
			Since processes are fork()ed in loop.
			So to make parent wait for all the child processes ... putting this infinite loop here.
			The loop will break when wait() will throw and error i.e. -1 when it has no more process to wait.
		*/

		/*while( is_bg == 0 ) {
			int status;
			int pid = wait(&status);
			//printf("Parent process waiting for %d\n" , pid);
			if(pid < 0)
				break;
		}*/
		int status;
		if ( is_bg == 0 ) {
			waitpid( new_process , &status , 0);
		}
		
		if ( (cmd = is_internal ( *command )) != -1 && (is_in_child ( *command ) == -1 )) {
			switch ( cmd ) {
				case ALIAS:
					execute_alias(command);
					break;
				case CD:
					execute_cd(command);
					break;
				case EXPORT:
					execute_export(command);
					break;
			}	
		}

		return new_process;
	}
	else {
		/* In case of fork error ... throwing error */
		perror("fork");
		return -2;
	}

}

void rc_init( ) {
	
	int fd_rc = 0;
	int readcount = 0 , i = 0;
	char ch;

	char * rc_command = (char *) calloc ( RC_COMMAND_S , sizeof( char ) );


	fd_rc = open( ".devilorc" , O_RDONLY );

	if( fd_rc < 0 ) {
	}
	else {
		readcount = read( fd_rc , &ch , 1);
		i = 0;
		while ( readcount > 0 ) {

			if( ch == '\n' || ch == ';' ) {
				rc_command[i++] = '\0';
				execute(rc_command , 1);
				i = 0;
			}
			else {
				rc_command[i++] = ch;
			}

			readcount = read( fd_rc , &ch , 1 );
		}
		rc_command[i++] = '\0';

		if( i != 1 ) {
			execute(rc_command , 1);
		}

	}

	char * str = get_local ( "HOME" );
        if( str == NULL )
        	str = get_env ( "HOME" );

        if ( chdir ( str ) < 0 ) {
        	fprintf( stderr , "devilo: cd: Invalid Directory %s\n" , str);
        }


	close( fd_rc );

	return;

}

int add_local_var ( char * command ) {

	char key[128] , value[128];
	int error = 0;

	char * split_eq = strtok ( command , "=" );
	strcpy( key , split_eq );
	split_eq = strtok ( NULL , "=" );
	if ( split_eq == NULL )
		error = 1;
	else
		strcpy( value , split_eq );

	if ( error == 0 ) {
		extern int var_t;
		extern char var_st[64][2][128];
		int i = 0;
		for( i = 0 ; i < var_t ; i++ ) {
			if( strcmp ( var_st[i][0] , key ) == 0 ) {
				strcpy ( var_st[i][1] , value );
				break;
			}
		}
		if ( i == var_t ) {
			strcpy( var_st[var_t][0] , key );
			strcpy( var_st[var_t][1] , value );
			var_t++;
		}
	}

	return 0;

}

int change_arg_ipop_redir ( char ** command ) {

	int i= 0;
	char * split_ip_redir = NULL;

	for( i = 0 ; command[i] != NULL && command[i+1] != NULL ; i++ ) {

		split_ip_redir = strstr ( command[i] , "<<" );

		if ( split_ip_redir != NULL ) {
			command [i] = NULL;
			int buf_t = 0;
			char *ip_str = (char *) calloc ( 512 , sizeof(char) );
			int fd = open ( ".temp" , O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR);

			while ( 1 ) {
				printf(">");
				buf_t = readline ( ip_str );

				if ( strcmp ( ip_str , command[i+1] ) == 0 )
					break;

				write ( fd , ip_str , buf_t );
				write ( fd , "\n" , 1 );

			}

			close ( fd );
			fd = open ( ".temp" , O_RDONLY );
			dup2 ( fd , STDIN_FILENO );
			free( ip_str );

			continue;
		}

		split_ip_redir = strchr ( command[i] , '<' );

		if ( split_ip_redir != NULL ) {
			command[i] = NULL;

			/* change the stdin to command[i+1] file */

			if( command[i+1] == NULL ) {
				fprintf( stderr , "bash: syntax error near unexpected token `newline'" );
			}
			else {
				int fd = open ( command[i+1] , O_RDONLY );
				dup2 ( fd , fileno(stdin) );
			}
			continue;			
		}

		
		split_ip_redir = strstr ( command[i] , ">>" );

		if ( split_ip_redir != NULL ) {
			command[i] = NULL;

			/* change the stdout to command[i+1] file */

			if( command[i+1] == NULL ) {
				fprintf( stderr , "bash: syntax error near unexpected token `newline'" );
			}
			else {
				int fd = open ( command[i+1] ,  O_WRONLY | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR );
				dup2 ( fd , fileno(stdout) );
			}
			
			continue;			
		}



		split_ip_redir = strchr ( command[i] , '>' );
		if ( split_ip_redir != NULL ) {
			command[i] = NULL;

			/* change the stdout to command[i+1] file */

			if( command[i+1] == NULL ) {
				fprintf( stderr , "bash: syntax error near unexpected token `newline'" );
			}
			else {
				int fd = open ( command[i+1] ,  O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR );
				dup2 ( fd , fileno(stdout) );
			}
			
			continue;			
		}

	}
	return 0;
}

int change_arg_echo ( char ** command ) {

	char var[1024];
	char temp[1024];
	int i = 0;
	int l = 0;
	int k = 0;
	for ( k = 0 ; command[k] != NULL ; k++ ) {

		char * str_dollar = strchr( command[k] , '$' );
		while ( str_dollar != NULL ) {
			i = 1;
			l = 0;
			while ( str_dollar[i] != '\0' && (( str_dollar[i] >= 'a' && str_dollar[i] <= 'z') \
	                                              ||  ( str_dollar[i] >= 'A' && str_dollar[i] <= 'Z')\
	                                              ||  ( str_dollar[i] >= '0' && str_dollar[i] <= '9')\
	                                              ||  ( str_dollar[i] == '_' ))) {
	
        	                                var[l++] = str_dollar[i++];

			}
			
			var[l] = '\0';
			char * value = get_local ( var );
			strcpy(temp , "$");
			if ( value != NULL ) {
				strcat( temp , var );
				command[k] = str_replace ( command[k] , temp , value );
			}
			else {
				value = get_env ( var );
				if( value != NULL ) {
					strcat( temp , var );
					command[k] = str_replace ( command[k] , temp , value );
				}
				else {
					strcat( temp , var );
					command[k] = str_replace ( command[k] , temp , "" );
				}
			}
		
			str_dollar = strchr ( str_dollar+1 , '$' );
		}

	}
	return 0;
}
