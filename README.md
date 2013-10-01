*********************************************************
	Arpit Bhayani - Devilo
*********************************************************
How to:

compile : make
execute : ./devilo OR make run

** NOTE : Before executing the shell make sure you set HOME in .devilorc file
This will be your HOME folder

exit the shell : exit or Ctrl-C

Features implemented :
- external commands with arguments
- internal commands : cd , pwd , export , exit , alias
- shell rc file with variables and commands
- echoing environment variable & shell variables
- Redirection operators > , >> , < , <<
- Shell history maintained
- Multiple pipe
- Background and foreground of jobs

Additional Feature :
- alias
- redirections within pipe
	e.g : ls > out | cat
- rc file handles command
	e.g : cat .devilo_ascii.txt in rc file executes the command `cat .devilo_ascii.txt`
- shell variables handled
	e.g : var=10

Limitation : 
- space to be given between tokens ( not in case of assignment operation )
