#include "shell_header.h"
#include "ctype.h"

char var_st[64][2][128];
int var_t;

char *internal_cmd[] = {
		"exit" , "alias" , "cd" , "pwd" , "export" , "history" , "fg" ,  NULL
	};

char *in_child[] = {
		"pwd" , "history" ,  NULL
	};

int is_in_child( char * command ) {

	int i = 0;

	while ( in_child[i] ) {
		if( strcmp(in_child[i++] , command) == 0 )
			return i-1;
	}
	return -1;
	
}

int is_internal( char * command ) {

	int i = 0;

	while ( internal_cmd[i] ) {
		if( strcmp(internal_cmd[i++] , command) == 0 )
			return i-1;
	}
	return -1;
	
}

char * get_local( char * var ) {
	int i = 0;

	for( i = 0 ; i < var_t ; i++ ) {
		if ( strcmp ( var_st[i][0] , var ) == 0 ) {
			return var_st[i][1];
		}
	}
	return NULL;
}

char * get_env( char * var ) {
	return getenv(var);
}

int change_shell_with_alias ( char * shell_line ) {

	char * split_pipe = NULL;
	char * value = (char * ) malloc ( 512 * sizeof(char) );
	char * shell = ( char * ) malloc ( 1024 * sizeof(char) );
	char * command = ( char * ) malloc ( 512 * sizeof(char) );

	int i = 0;

	/* For first command ... directly split by space */

	split_pipe = strtok ( shell_line , "|" );

	i = 0;
	split_pipe = trimwhitespace( split_pipe );

	while ( split_pipe[i] != '\0' && split_pipe[i] != ' ' && split_pipe[i] != '\t' ) {
	command[i] = split_pipe[i];
		i++;
	}
	command[i] = '\0';

	int found = get_alias ( command , value );
	if ( found == 1 ) {
		split_pipe = str_replace_once ( split_pipe , command , value );
	}

	//printf("split_pipe : %s\n" , split_pipe);
	strcat ( shell , split_pipe );

	split_pipe = strtok ( NULL , "|");

	while ( split_pipe != NULL ) {

		/* String to be replaced is split_pipe */
		
		i = 0;
		split_pipe = trimwhitespace( split_pipe );

		while ( split_pipe[i] != '\0' && split_pipe[i] != ' ' && split_pipe[i] != '\t' ) {
			command[i] = split_pipe[i];
			i++;
		}
		command[i] = '\0';

		int found = get_alias ( command , value );
		if ( found == 1 ) {
			split_pipe = str_replace_once ( split_pipe , command , value );
		}

		//printf("split_pipe : %s\n" , split_pipe);

		strcat ( shell , "|");
		strcat ( shell , split_pipe );

		/* String to be replaced with is */

		split_pipe = strtok ( NULL , "|");
	}

	strcpy ( shell_line , shell );

	free(shell);
	free(value);
	free(command);

	return 0;
}

char * str_replace_once ( const char *string, const char *substr, const char *replacement ) {
	char *tok = NULL;
	char *newstr = NULL;
	char *oldstr = NULL;
	char *head = NULL;

	if ( substr == NULL || replacement == NULL ) 
		return strdup (string);

	newstr = strdup (string);
	head = newstr;

	if ( (tok = strstr ( head, substr ))) {
		oldstr = newstr;
		newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
		if ( newstr == NULL ) {
			free (oldstr);
			return NULL;
		}

		memcpy ( newstr, oldstr, tok - oldstr );
		memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
		memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
		memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
		head = newstr + (tok - oldstr) + strlen( replacement );
		free (oldstr);
	}
	return newstr;
}

char * str_replace ( const char *string, const char *substr, const char *replacement ) {
	char *tok = NULL;
	char *newstr = NULL;
	char *oldstr = NULL;
	char *head = NULL;

	if ( substr == NULL || replacement == NULL ) 
		return strdup (string);

	newstr = strdup (string);
	head = newstr;

	while ( (tok = strstr ( head, substr ))) {
		oldstr = newstr;
		newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
		if ( newstr == NULL ) {
			free (oldstr);
			return NULL;
		}

		memcpy ( newstr, oldstr, tok - oldstr );
		memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
		memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
		memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
		head = newstr + (tok - oldstr) + strlen( replacement );
		free (oldstr);
	}
	return newstr;
}

int getpcname( char * hostname ) {
        return gethostname( hostname , 1024);
}

void print_prompt( char * hostname ) {

	char * cwd = (char *) calloc(2048 , sizeof(char) );
	if ( getcwd ( cwd , 2048 ) == NULL)
		perror("getcwd() : ");

	getpcname ( hostname );
	fprintf(stdout , "devilo@%s:%s$ " , hostname , cwd);

	free(cwd);
}

char * trimwhitespace ( char * str ) {
	char *end;

	while(isspace(*str)) str++;
	if(*str == 0)
		return str;

	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;
	*(end+1) = 0;

	return str;
}

int is_not_whitespace ( char * str ) {
	int i = 0;
	while ( str[i] != '\0' ) {
		if ( str[i] == ' ' || str[i] == '\n' || str[i] == '\t' ) {}
		else
			return 1;
	}
	return 0;
}

int get_splits ( char *** commands , char * shell_line ) {

	char * split = NULL;
	char * split_space = NULL;
	int i = 0 , k =  0 , l = 0;

	char ** comm = (char ** ) malloc ( 32 * sizeof(char *) );

	split = strtok( shell_line , "|" );

	while ( split != NULL ) {
		comm[i] = split;
		i++;
		split = strtok ( NULL , "|");
	}
	for( k = 0 ; k < i ; k++ ) {
		split_space = strtok(comm[k] , " ");
		l = 0;
		while( split_space != NULL ) {
			commands[k][l++] = split_space;
			split_space = strtok( NULL , " ");
		}
		commands[k][l] = NULL;
	}
	free( comm );
	return i;
}

int readline( char *str ) {

	/*int rd = 0 , flag_anych = 0;
		if ( ( rd = read( fileno(stdin) , str , 1024 ) ) < 0 ) {
			printf("Error!!");
		}

		if ( str[0] == 0 )
			return -1;

		return strlen ( str );		
	*/	
	char ch;
	int i = 0;
	while ( ( ch = getchar() ) != '\n' ) {
		*str++ = ch;
		i++;
	}
	*str = '\0';
	return i;	
}

void printline( char *str ) {
	printf("%s" , str);
}

