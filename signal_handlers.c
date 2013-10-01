#include "shell_header.h"

void catch_int( int sig_num ) {

	signal(SIGINT , catch_int);

}

