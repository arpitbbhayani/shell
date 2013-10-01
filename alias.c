#include "shell_header.h"

int alias_t;
char alias[64][2][1024];

int get_alias ( char * command , char * value) {

	int i = 0;
	int found = 0;

	for( i = 0 ; i < alias_t ; i++ ) {
		if ( strcmp( command , alias[i][0] ) == 0 ) {
			strcpy( value , alias[i][1]);
			found = 1;
		}
	}
	if ( found == 0 )
		value = NULL;
	return found;
}

int add_alias ( char ** command ) {

	int i = 0 , j = 0;
	int found = 0;

	char * cmd = malloc ( 512 * sizeof(char) );
	char * key = malloc ( 512 * sizeof(char) );
	char * value = malloc( 512 * sizeof(char) );
	char * split_eq;

	strcpy ( cmd , command[1] );

	split_eq = strtok ( cmd , "=" );
	if ( split_eq != NULL ) {
		strcpy(key , split_eq);		
	}

	for( i = 0 ; i < alias_t ; i++ ) {
		if ( strcmp( key , alias[i][0] ) == 0 ) {
			found = 1;
			break;
		}
	}

	split_eq = strchr( command[1] , '=' );
	split_eq ++;

	strcat ( value , split_eq );

	for( j = 2 ; command[j] != NULL ; j++ ) {
		strcat( value , " ");
		strcat( value , command[j]);
	}

	if ( found == 0 ) {
		strcpy ( alias[alias_t][0] , key );
		strcpy ( alias[alias_t][1] , value );
		alias_t ++;
	}
	else {
		strcpy ( alias[i][0] , key );
		strcpy ( alias[i][1] , value );
	}

	free( cmd );
	free( key );
	free( value );

	return 0;
}


