devilo:			main.o signal_handlers.o shell_util.o execute.o internal_commands.o history.o alias.o job_control.o
			gcc main.o shell_util.o execute.o signal_handlers.o internal_commands.o history.o alias.o job_control.o -o devilo

job_control.o:		job_control.c shell_header.h
			gcc -c -Wall job_control.c

alias.o:		alias.c shell_header.h
			gcc -c -Wall alias.c

history.o:		history.c shell_header.h
			gcc -c -Wall history.c

signal_handlers.o:	signal_handlers.c shell_header.h
			gcc -c -Wall signal_handlers.c

internal_commands.o:	internal_commands.c shell_header.h
			gcc -c -Wall internal_commands.c

execute.o:		execute.c shell_header.h
			gcc -c -Wall execute.c

shell_util.o:		shell_util.c shell_header.h
			gcc -c -Wall shell_util.c

main.o:			main.c shell_header.h
			gcc -c -Wall main.c


clean:
			rm shell_util.o main.o execute.o signal_handlers.o internal_commands.o history.o alias.o job_control.o

run:			devilo
			./devilo


