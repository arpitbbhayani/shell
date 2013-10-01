#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#define EXIT	0


/* Shell execute.c */
void rc_init();
int add_local_var ( char * command );
int change_arg_echo ( char ** command );
int change_arg_ipop_redir ( char ** command );

void execute ( char * shell_line , int from_rc );
void _execute(int num, char ***commands , int *child_pid , int is_bg);
int __execute( char **command , int fd_read , int fd_write , int is_bg);


/* Internal commands */
int execute_cd( char ** command );
int execute_pwd( char ** command );
int execute_export( char ** command );
int execute_alias( char ** command );
int execute_history( char ** command );
int execute_bang( char ** command );
int exec_bang( char * command );
int exec_alias( char * command );
int execute_exit();

/* Shell util */
int is_in_child( char * command );
int is_internal( char * command );
char * get_local( char * var );
char * get_env( char * var );
int change_shell_with_alias( char * command );
char * str_replace ( const char *string, const char *substr, const char *replacement );
char * str_replace_once ( const char *string, const char *substr, const char *replacement );
int getpcname( char * hostname );
void print_prompt( char * hostname );
char * trimwhitespace ( char * str );
int get_splits ( char *** commands , char * shell_line );
int readline( char *str );
void printline( char *str );
int is_not_whitespace( char * );

/* Signal handler */
void catch_int( int sig_num );

/* history */
int add_history ( char * command );
int get_command ( char * command , char * command_exex);
void print_history ();
int write_history ();
int read_history ();

/* alias */
int get_alias ( char * command , char * value);
int add_alias ( char ** command );

/* Job control */
int remove_job ( int job_num );
int add_job ( int process_id , char * job_n);
