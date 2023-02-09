#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include<libgen.h>
#include <sys/wait.h>
#include<fcntl.h>
#include<stdbool.h>
#include<signal.h>


struct job {
  bool running;
  pid_t pid;
  char cmd[255];
};
struct job jobs[255];
int next_job = 1;

int main(int argc, char *argv[]) 
{
	char current_working_directory[1024];
	char input[1000];
	char *command_arguments[100];
	//int status;
	int cmd=0;
	char hist[10][100];
	//int next_job=1;
	char *cmd1;
	
	while (1) 
	{
		int status;
    		pid_t pid2=waitpid(-1,&status,WNOHANG);
    		//next_job=next_job-1;
    		while(pid2>0)
    		{
    			printf("[%d] Done: %s \n", next_job, cmd1);
    			pid2=waitpid(-1,&status,WNOHANG);
    		}
        	getcwd(current_working_directory, sizeof(current_working_directory));
        	printf("%s$ ", basename(current_working_directory));
    		fgets(input, sizeof(input), stdin);
    		if (input == NULL) 
    		{
        		break;
    		}
    		input[strcspn(input, "\n")] = 0;

    		if (!isatty(fileno(stdin))) 
    		{
        		printf("%s\n", input);
        		fflush(stdout);
    		}
    		
    			
               //Code for handling "jobs" command
    		int cmp=strcmp(input,"jobs");
    		//printf("%d\n",cmp);
    		if(cmp==0)
		{
			//int status;
			strcpy(hist[cmd % 10], input);
  			cmd++;
			
			for (int i = 0; i < next_job; i++) {
				if (jobs[i].running == true) {
      				printf("%d: %s \n", i, jobs[i].cmd);
      				//continue;
    				}
				int status;
				pid_t pid2 = waitpid(jobs[i].pid, &status, WNOHANG);
				if (pid2 > 0) {
				if(i!=0){
				printf("[%d] Done: %s \n", i, jobs[i].cmd);
				jobs[i].running = false;
				}
				}
				
				
				
				
			}
					
			
			continue;
		}
		
    		//int cmp2=strcmp(input,"kill");
    		//printf("%d\n",cmp2);
    		//Code for handling kill
    		if(strstr(input,"kill") != NULL)
		{
  			strcpy(hist[cmd % 10], input);
  			cmd++;
  			char *parsedInput = input + 5;
  			//printf("%s\n",parsedInput);
   // 5 is the length of "kill "
  			int job;
  			if(sscanf(parsedInput,"%d",&job) == 1)
  			{
  				//printf("%d\n",job);
    				if (job >= 0 && job < next_job && jobs[job].running == true) {
      				int pid = jobs[job].pid;
      				
      				if (kill(pid, SIGKILL) == 0) {
        			//printf("[%d] Done: %s\n", job, jobs[job].cmd);
        			jobs[job].running = false;
        			continue;
      			} 
      			else {
        			printf("kill");
      				}
    			} 
    			else {
    			printf("wshell: no such background job: %d\n", job);
    			continue;
  		     }
  			} 
  		
		}


    		//Code for handling &
    		if (input[strlen(input) - 1] == '&')
		{
			strcpy(hist[cmd % 10], input);
  			cmd++;
    			strcpy(hist[cmd % 10], input);
    			cmd++;
    			int status;
    			pid_t pid2=waitpid(-1,&status,WNOHANG);
    			//next_job=next_job-1;
    			while(pid2>0)
    			{
    				printf("[%d] Done: %s \n", next_job, cmd1);
    				pid2=waitpid(-1,&status,WNOHANG);
    			}
    			pid_t pid;
    			char *copy = strdup(input);
    			cmd1 = strtok(copy, "&");
    			cmd1[strlen(cmd1) - 1] = '\0';
    			printf("[%d]\n", next_job);
    			pid = fork();
    			if (pid == -1)
    			{
        			perror("fork");
        			exit(1);
    			}
    			if (pid == 0)
    			{
    				
        			char *arg[255];
        			int i = 0;
        			arg[i] = strtok(cmd1, " ");
        			while (arg[i] != NULL)
        			{
            				i++;
            				arg[i] = strtok(NULL, " ");
        			}
        			printf("%s",arg[0]);
        			execvp(arg[0], arg);
        			//sleep(3);
        			//int status;
        			
        			
        			//continue;
    			}
    			else
    			{
    				fflush(stdout);
        			int status;  
        			waitpid(pid, &status, WNOHANG);
        			jobs[next_job].running = true;
    				jobs[next_job].pid = pid;
    				strcpy(jobs[next_job].cmd, cmd1);
        			next_job = (next_job + 1) % 256;
        			
        			continue;
    			}
    			
    
		}
		
		
    		

    		//Code for handling >
    		if(strstr(input," > ")!=NULL)
		{
    			strcpy(hist[cmd % 10], input);
    			cmd++;
    			char *command1, *file;
    			char *copy = strdup(input);
    			command1 = strtok(copy,">");
    			command1[strlen(command1)-1]='\0';
    			file = strtok(NULL, "> ");
    			file = strtok(file, "\n");

    			pid_t pid = fork();
    			if(pid == 0)
    			{
        			int file_descriptor = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0666 | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        			if(file_descriptor == -1)
        			{
            				printf("Error opening file: %s\n", strerror(errno));
            				exit(1);
        			}
        		fclose(stdout);
        		dup2(file_descriptor, 1);
        		close(file_descriptor);
        		execlp("/bin/sh", "sh", "-c", command1, (char *)0);
    			}
    			else
    			{
        			int status;
        			waitpid(pid, &status, 0);
        			continue;
    			}

    			free(copy);
		}

		//Code for handling >>
		if(strstr(input,">>")!=NULL)
		{
    			strcpy(hist[cmd % 10], input);
    			cmd++;
    			char *command1, *file;
    			char *copy = strdup(input);
    			command1 = strtok(copy,">>");
    			command1[strlen(command1)-1]='\0';
    			file = strtok(NULL, ">> ");
    			file = strtok(file, "\n");

    			pid_t pid = fork();
    			if(pid == 0)
    			{
        			int file_descriptor = open(file, O_APPEND | O_WRONLY);
        			if(file_descriptor == -1)
        			{
            				printf("Error opening file: %s\n", strerror(errno));
            				exit(1);
        			}
        			fclose(stdout);
        			dup2(file_descriptor, 1);
        			close(file_descriptor);
        			execlp("/bin/sh", "sh", "-c", command1, (char *)0);
    			}
    			else
    			{
        			int status;
        			waitpid(pid, &status, 0);
        			continue;
    			}

    			free(copy);
		}

    		//Code for handling |
    		if(strstr(input," | ")!=NULL)
		{
  			//int non_space=0;
  
  			strcpy(hist[cmd % 10], input);
  			cmd++;
  			int file_descriptor2[2];
  			pipe(file_descriptor2);
  			char *command1, *command2;
  			char *copy=strdup(input);
  			command1=strtok(copy,"|");
  			command2=strtok(NULL,"|");
  			//char *p=command2;
  			//memmove(command2, command2+1, strlen(command2));
  			//printf("%s\n",command2);
  
  			pid_t pid=fork();
  			if(pid==0)
  			{
    				close(file_descriptor2[0]);
    				char *exec=strtok(command1," ");
    				char *arg=strtok(NULL," ");
    				printf("%s\n",exec);
    				printf("%s\n",arg);
    				dup2(file_descriptor2[1],1);
    				close(file_descriptor2[1]);
    				execlp(exec,exec,arg,(char *) NULL);
    
    
    				exit(0); // add exit to prevent child process from continuing execution
  			}
  			else
  			{
    				close(file_descriptor2[1]);
    				pid_t pid2 = fork();
    				if(pid2==0)
    				{
      					char *exec = strtok(command2," -l");
      					char *arg = strtok(NULL," ");
      					printf("%s\n",exec);
      					printf("%s\n",arg);
      					dup2(file_descriptor2[0],0);
      					close(file_descriptor2[0]);
      					execlp(exec,exec,arg,(char *) NULL);
    
      					exit(0); // add exit to prevent child process from continuing execution
    				}
    				else
    				{
      					close(file_descriptor2[0]);
      					int status1, status2; // add two variables to store waitpid results
      					waitpid(pid, &status1, 0);
      					waitpid(pid2, &status2, 0);
      					continue;
    				}
  			}
  
  
  			free(copy);
  
		}
		//Code for handling &&
		if (strstr(input, "&&") != NULL) 
    		{
  			strcpy(hist[cmd % 10], input);
  			cmd++;
  //int status;
  			char *cmd1, *cmd2;
  			char *copy = strdup(input);
  			cmd1 = strtok(copy, "&&");
  			cmd2 = strtok(NULL, "&&");
  
  			pid_t pid = fork();
  			if (pid == 0) 
  			{
    				char *exec = strtok(cmd1, " ");
    				char *arg = strtok(NULL, "");
    				execlp(exec, exec, arg, (char *) NULL);
  			} 
  			else 
  			{
    				int wstatus;
    				waitpid(pid, &wstatus, 0);
    				if (WEXITSTATUS(wstatus) == 0) 
    				{
      					pid = fork();
      					if (pid == 0) 
      					{
        					char *exec = strtok(cmd2, " ");
        					char *arg = strtok(NULL, "");
        					execlp(exec, exec, arg, (char *) NULL);
        					//fflush(stdout);
        					continue;
      					}
       				else 
       				{
        					waitpid(pid, &wstatus, 0);
        					continue;
      					}
    				}
    				else
    				{
    					continue;
    				}
  			}
  			free(copy);
		}
		//Code for handling ||
 		if(strstr(input, "||") !=NULL) 
 		{
    			strcpy(hist[cmd % 10], input);
  			cmd++;
  			//int status;
  			char *cmd1, *cmd2;
  			char *copy = strdup(input);
  			cmd1 = strtok(copy, "||");
  			cmd2 = strtok(NULL, "||");
  
  			pid_t pid = fork();
  			if (pid == 0) 
  			{
    				char *exec = strtok(cmd1, " ");
    				char *arg = strtok(NULL, "");
    				execlp(exec, exec, arg, (char *) NULL);
    				//continue;
  			} 
  			else 
  			{
    				int wstatus;
    				waitpid(pid, &wstatus, 0);
    				if (WEXITSTATUS(wstatus) != 0) 
    				{
      					pid = fork();
      				if (pid == 0) 
      				{
        				char *exec = strtok(cmd2, " ");
        				char *arg = strtok(NULL, "");
        				execlp(exec, exec, arg, (char *) NULL);
        				//fflush(stdout);
        				continue;
      				} 
      				else 
      				{
        				waitpid(pid, &wstatus, 0);
        				continue;
      				}
    				}			
    				else
    				{   
    					continue;
    				}
  			}
  			free(copy);
    
    
		}
	        //Code for handling exit, echo,cd, pwd, history and other commands.
    		if (strcmp(input, "exit") == 0) 
    		{
    			strcpy(hist[cmd%10],input);
    			cmd++;
        		exit(0);
    		} 
    		else if (strstr(input, "echo") != NULL) 
    		{
    			strcpy(hist[cmd%10],input);
    			cmd++;
        		printf("%s\n", input + 5);
    		} 
    		else if (strstr(input, "cd") != NULL) 
    		{
    			strcpy(hist[cmd%10],input);
    			cmd++;
        		if (strstr(input + 3, " ") != NULL) 
        		{
            			printf("wshell: cd: too many arguments\n");
        		} 
        		else 
        		{
            			if (strlen(input) == 2) 
            			{
                			char *home_env = getenv("HOME");
                			if (home_env != NULL) 
                			{
                    				status = chdir(home_env);
                			} 
                			else 
                			{
                    				perror("$HOME not set");
                    				status = -1;
                			}
            			} 
            			else 
            			{
                			status = chdir(input + 3);
                			if (status == -1) 
                			{
                    				printf("wshell: no such directory: %s\n", input + 3);
                			}
            			}
        		}
    		} 
    		else if (strstr(input, "pwd") != NULL) 
    		{
    			strcpy(hist[cmd%10],input);
    			cmd++;
        		getcwd(current_working_directory, sizeof(current_working_directory));
        		if (current_working_directory != NULL) 
        		{
            			printf("%s\n", current_working_directory);
        		}
    		}
    
    		else if(strstr(input,"history") != NULL)
    		{
    			strcpy(hist[cmd%10],input);
    			cmd++;    		
    			int start=cmd-10;
    			if(start<0)
    			start=0;    		
    			for(int i=start;i<cmd;i++)
    			{
    				printf("%s\n",hist[i%10]);
    			}
    		
    		}
    	
    

    
     		else 
     		{
     		if(strstr(input,"sleep 2")!=NULL)
     			{
     				sleep(2);
     				
     			}
     		
     		
     			
     			strcpy(hist[cmd%10],input);
    			cmd++;
    			int status;
    			pid_t pid2=waitpid(-1,&status,WNOHANG);
    			next_job=next_job-1;
    			while(pid2>0)
    			{
    				printf("[%d] Done: %s \n", next_job, cmd1);
    				pid2=waitpid(-1,&status,WNOHANG);
    			}
        		int i = 0;
        		char *token = strtok(input, " ");
        		while (token != NULL) 
        		{
            			command_arguments[i++] = token;
            			token = strtok(NULL, " ");
        		}
        		command_arguments[i] = NULL;
        		pid_t pid = fork();
        		if (pid == 0) 
        		{
            			if (execvp(command_arguments[0], command_arguments) == -1) 
            			{
                			printf("wshell: could not execute command: %s\n",input);
            			}
            			exit(0);
        		} 
        		else if (pid < 0) 
        		{
            			perror("wshell: error in fork");
        		} 
        		else 
        		{
            			waitpid(pid, &status, 0);
        		}
        		
    		}
    		
	}	
return 0;
}

