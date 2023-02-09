# Shell_Implementation
Design rationale
C program to build basic built-ins. 

Implementation logic 
For displaying the shell till the user instructs to exit, we will create a while(1) loop, i.e. infinite loop and we will get the current working directory using getcwd. To print only the top hierarchy of the file, we will use basename in C. If a NULL input is received, we will break the loop. We also handle the case to determine if stdin file descriptor refers to a terminal or file/pipe using isatty. The input is then compared with exit, echo, cd and pwd using string functions and necessary action is taken in each case. If the command is echo we simply write exit(0) to exit out of the shell. If the command is echo, we print from the 5th character of the input string as the first 4 characters are echo and the text starting from 5th character will be printed. In case of cd, we first check if there are too many arguments by checking if there are more than one space in the string. If more than one space, we declare it as too many arguments. Otherwise, if no arguments are mentioned we will redirect to HOME. We get HOME using getenv in C. Else, we change the directory specified in the command using chdir in C. In case of pwd, we get the current working directory using getcwd and print it.



Part 2
---

Design rationale 
C program to built shell functionality (wshell)

Implementation logic 

'current_working_directory' is an array of char to store the current working directory. input is an array of char to store the input command. command_arguments is an array of pointers to char to store the arguments of the input command. status is an integer variable used to store the status of the executed command. 

The code enters an infinite loop that continues until the "exit" command is entered. In each iteration of the loop, the current working directory is obtained using getcwd and printed on the console. The command is then read from the user using fgets. If the input is NULL, the loop is broken. The newline character at the end of the input string is replaced with a null terminator. The isatty function is used to check if the input is coming from a terminal or not. If not, the input is printed. 

fork() creates a child process using the fork system call. The pid_t data type is used to store the process ID of the child process.

Then we check if the process ID of the child process is equal to zero. This is a convention used to determine whether the code is being executed in the child process or the parent process.

We use the execvp system call to execute the command specified in the command_arguments array. If the call returns -1, it means an error occurred and a message is printed with perror.

the exit(0) line terminates the child process with a status of 0, indicating a successful exit.

(pid < 0) else if statement checks if the process ID of the child process is less than zero. If this is the case, it means that the fork system call failed, and a message is printed with perror.

waitpid(pid, &status, 0) line is executed in the parent process and waits for the child process to finish executing. The pid parameter is the process ID of the child process, the status parameter is a pointer to a int that will store the exit status of the child process, and the 0 parameter specifies the options for the waitpid call.

'return 0' line returns 0 from the main function, indicating a successful program execution.



Part 3
---
Design rationale 
C program to implement shell operators
Implementation logic 
In this program, commands like "history" and those containing && and || are implemented.
For implementing "history" command, we created an array named hist in which we store the last 10 commands that has been executed. We use hashing in this case, since we only need to store last 10 commands so we store all the commands into hist[cmd%10] and increment cmd in each case.

For implementing && and || we first check whether the input contains && and || using strstr function. If && is present, we split the input string to 2 parts using strtok and we use fork() to create a child process. The first child process executes first part of input and it waits for process to finish. Once it is finished it will exit with status 0 and only then second child process is created. The parent will wait for second child process to finish. 

If we encounter ||, the first child process executes first part of input string and waits for the process to finish. If the exit is not 0, second child is created to execute second command. The parent waits for second child process to finish and it then continues. We have used execlp to execute the commands.

Part 4
---
Design rationale 
C program to implement Redirection operators > and >> and piping (|)
Implementation logic 
First we check, whether the input string contains >, >> or | operator using strstr function. When > is encountered, the specified file is opened using open() in write-only mode and it truncates its content. The standard output is redirected to the file using dup2(). If we encounter >>, then the program opens the file in append mode and the standard output is redirected to the file. When we encounter "|", it creates a pipe and forks a child process using fork(). The child process then redirects the standard output to the write end of pipe, and parent process redirects standard input to read end of pipe. We then execute both the commands.



Part 5
---
Design rationale 
C program to implement background jobs (&), jobs and kill command
Implementation logic 
We first check whether the input is "jobs". If it is so, the program will loop through an array of jobs and print the jobs that are running. The code checks if a job is completed or not and will print a message accordingly.
We then check if input is "kill". The code will then get the job number after kill and check whether it is running. If it is in running state, it will send SIGKILL to job and mark it as not running. Otherwise it will print error, if specified job is not running.
If the input has "&" in it, it means that the command should run in the background. A separate process will be created using fork() and it will add the command to background jobs list and print job number assigned to this job.
