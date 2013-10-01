#include "shell_header.h"

#define ALIAS_T	   128

char var_env_st[64][1024];
int var_env_t;

int exec_alias( char * shell_line ) {

	change_shell_with_alias ( shell_line );

	return 0;
}

int exec_bang( char * shell_line ) {

	int i = 0;
	char bang_var[64];
	char * bang_val = (char * )malloc(128 * sizeof(char) );
	char * shell = (char * ) malloc ( 1024 * sizeof(char) );

	strcpy( shell , shell_line );

	char * bang_str = strchr ( shell , '!' );

	if( bang_str != NULL && strlen(bang_str) == 1) 
		return -2;

	while ( bang_str != NULL ) {

		for( i = 0 ; bang_str[i] != ' ' && bang_str[i] != '\0' && bang_str[i] != '\t' ; i++ ) {
			bang_var[i] = bang_str[i];
		}
		bang_var[i] = '\0';
		bang_val[0] = '\0';
		int r = get_command( bang_var , bang_val );
		if ( r == -2 ) {
			return r;
		}
		shell = str_replace_once(shell , bang_var , bang_val );
		bang_str = strchr( shell , '!');
	}

	strcpy( shell_line , shell );

	free(shell);
	
	return 0;
}

int execute_bang( char ** command ) {
	char * command_exec = (char *) malloc ( 1024 * sizeof(char) );
	int ret_val = get_command( command[0] , command_exec );
	if ( ret_val == -1 || ret_val == -2 ) {
		printf("bash: %s: event not found" , command[0]);
	}
	printf("%s\n" , command_exec);
	
	execute( command_exec , 0);

	return 0;
}

int execute_history( char ** command ) {
	print_history();
	return 0;
}

int execute_cd( char ** command ) {
	int i = 0;
	for( i = 0 ; command[i] != NULL ; i++ ) {}

	if( i == 1 ) {
		char * str = get_local ( "HOME" );
		if( str == NULL )
			str = get_env ( "HOME" );

		if ( chdir ( str ) < 0 ) {
			fprintf( stderr , "devilo: cd: ERROR\n");
		}
	}
	else {
		if ( chdir ( command[1] ) < 0 ) {
			fprintf( stdout , "devilo: cd: %s: No such file or directory\n" , command[1] );
		}
	}
	
	return 0;
}

int execute_pwd( char ** command ) {

	char * cwd = (char *) calloc(2048 , sizeof(char) );
	if ( getcwd ( cwd , 2048 ) == NULL)
		perror("getcwd() : ");

	printf("%s\n" , cwd);
	free(cwd);
	return 0;
}

int execute_export( char ** command ) {

	int i = 0;
	if( command[1] == NULL ) {
		for ( i = 0 ; i < var_env_t ; i++ ) {
			printf("export %s=%s\n" , var_env_st[i] , get_env(var_env_st[i]));
		}
		return 0;
	}

	char key[1024] , value[1024];
	int error = 0;

	char * str = strtok( command[1] , "=" );
	if ( str != NULL ) {
		strcpy ( key , str );
		str = strtok ( NULL , "=" );
		if( str == NULL ) {
			error = 1;
		}
		else {
			strcpy( value , str );
		}
	}
	if( error == 0 ) {
		/* assert export <key>=<value> */
		strcpy ( var_env_st[ var_env_t ++ ] , key );
		setenv( key , value, 1);
		
	}

	
	return 0;
}


int execute_alias( char ** command ) {

	add_alias ( command );

	return 0;
}

int execute_exit( ) {
	write_history();
	printf("Exiting devilo ... have a nice day >>\n");
	return -1;
}
