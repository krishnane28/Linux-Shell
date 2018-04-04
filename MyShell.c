#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>

#define MAX_LINE 80

int main()
{
 // This variable contains the data read from the mentioned stream
 char input_from_buffer[MAX_LINE]; 
 int should_run = 1;
 // This gets the command entered by the user
 char* user_command[MAX_LINE]; 
 char* user_command_pipeinput[MAX_LINE];
 char* user_command_pipeoutput[MAX_LINE];
 int total_arguments;
 // This variable contains the main directory where the executable file is present
 char* main_directory = "/bin/";  
 
 // This variable contains the absolute path of the executable file for the command entered by the user
 char exe_abs_path[50];
 int exit_signal = 1;
 
 //This variable is used to check whether the user command contains piping
 int redirection_pipe = 0;
 
 //This variable is used to check whether the user command contains piping
 //char redirection = ' ';
 
 while(should_run)
 {	
  // This is the shell prompt
  printf("UBOS_1003893>");
  
  // This function is used to flush the stdout stream in order to avoid duplicate data 
  fflush(stdout);

  // char* fgets(char* str,int n, FILE* file); - get the data from the stdin stream and used for cheking whether user
  // has entered exit command or left it empty and make the program to exit accordingly
  if(*(fgets(input_from_buffer, MAX_LINE, stdin)) == '\n')
  {    
    should_run = 0;
    exit_signal = 0;
    printf("The process is terminated because you have not entered any commands\n");
  }
  
  // This will check whether the user has entered "exit" command
  if(strcmp(input_from_buffer, "exit\n") == 0)
  {    
    printf("The process is terminated\n");		
    should_run = 0;
    exit_signal = 0;
  }
  
  // This if condition checks whether the buffer contains the newline character and replaces it with '\0' because newline 
  // is a termination character in the shell and if not replaced execvp will not execute successfully 
  if(strlen(input_from_buffer) != 1)
  {
   if(input_from_buffer[strlen(input_from_buffer) - 1] == '\n')
   {
    input_from_buffer[strlen(input_from_buffer) - 1] = '\0';
   }
  }

  // This variable is used to get the individual tokens from the entire string and store the each token in the user_command 
  // variable so that it can be given to the execvp system call
  char* command_parts;

  // strtok function splits the string into tokens based on the mentioned delimiter as one of its arguments
  command_parts = strtok(input_from_buffer," ");
  
  // This variable i counts the number of arguments entered by the user
  int i = 0;
  
  int j = 0;
  
  int k = 0;

  char pipe_command = ' ';
  
  // Here the entire string entered by the user is splitted into tokens and is stored in user_command variable and if it
  // has pipe or redirection command in it it stores the tokens after | or > into user_command_pipeinput and 
  // user_command_pipeoutput for pipe or redirection commands
  while(command_parts != NULL)
  {
   //This if statement checks whether the user commands contains pipes or redirection
   if(*command_parts == '|' || *command_parts == '>')
   {
    redirection_pipe = 1;
    k = i;
   }
   if(redirection_pipe == 0)
   {
    user_command[i] = command_parts;
    user_command_pipeinput[i] = command_parts;
   }
   else
   {
    if(pipe_command == '|')
    {
     if(*command_parts == '|' || *command_parts == '>')
     {
       j = 0;
     }
     else
     {
      user_command_pipeoutput[j] = command_parts;
      j++;
     }
    }
   }

   // NULL as the first argument because strtok maintains a static pointer to the previous passed string 
   command_parts = strtok(NULL, " "); 
   
   //This if condition checks whether the tokens from the command which is a string entered by the user has pipe command 
   //in it and if available it initializes the pipe_command variable to | so that later I can get the tokens after the pipe
   //command to be given to the execvp system call to the processes
   if(command_parts != NULL)
   {
    if(*command_parts != '|' || *command_parts == '>')
    {
     pipe_command = '|';
     //if(*command_parts == '>')
     //{
      //redirection == '>';
     //}
    }
   }
    i++;
  }
  
  
  user_command[i] = NULL;
  if(redirection_pipe == 1)
  {
   //j++;
   user_command_pipeinput[k] = NULL; // write to pipe
   user_command_pipeoutput[j] = NULL; // Read from pipe
  }
 
  // i stores the total number of tokens in the input string and is stored in total_arguments variable 
  total_arguments = i;
  
  pipe_command = ' ';

  // The below two string functions is used to get the absolute path for the executable file for the command entered by the
  // user and exe_abs_path is used in execv system call 
  strcpy(exe_abs_path, main_directory);
  strcat(exe_abs_path, user_command[0]);   

 // Here any new line character is replaced by '\0' because new line is a termination character in the shell and if passed
 // in as commands to execv system call produces unexpected results
 for(i = 0; i < strlen(exe_abs_path); i++)
 {
  if(exe_abs_path[i] == '\n') 
  {
   exe_abs_path[i] = '\0';
  }
 } 

      // The reason I created two file descriptors are by default the shell uses two file descriptors i.e. stdin and stdout
      // for a process to communicate with each other and my program doesn't have this feature 
      int fd[2];
      //pipe system call creates a pipe with read end and write end which is nothing but a buffer for processes to 
      //communicate with each other
      // this call creates a pipe with fd[0] and fd[1] file descriptors
      // fd[0] is the read end of the pipe and fd[1] is the write end of the pipe
      pipe(fd);
  
 if(exit_signal == 1)
 {
  if(redirection_pipe == 1)
  {
      if(fork() == 0)        
      {
      //This process only writes to the file descriptor       

       //1 is a file descriptor called stdout and it has to be closed to be replaced with other file descriptor so that 
       //my program can write to that file descriptor for other process to read from it
       close(1);
       //dup system call creates a copy of the specified file descriptor and stores it in the first free location of the file 
       //descriptor table  which is 1 because we have closed the file descriptor 1 
       dup(fd[1]); 
       //Close the read end of the pipe so that this process writes content to the pipe        
       close(fd[0]);  
       //close the file descriptor because already we have created a duplicate of this file descriptor    
       close(fd[1]);
       execvp(user_command_pipeinput[0], user_command_pipeinput);  
       fprintf(stderr, "Child Process failed to execute execvp() system call\n");     
       _exit(1);
      }	
      if(fork() == 0) 
      {
       //This process reads from the file descriptor

       //0 is a file descriptor called stdin and it has to be closed to be replaced with other file descriptor so that 
       //my program can read from the specified file descriptor for other process to read from it
       close(0);   
       //dup system call creates a copy of the specified file descriptor and stores it in the first free location of the file 
       //descriptor table  which is 0 because we have closed the file descriptor 0 
       dup(fd[0]);  
       //close the file descriptor because already we have created a duplicate of this file descriptor  
       close(fd[0]);
       //Close the write end of the pipe so that this process writes content to the pipe   
       close(fd[1]);
       //if(redirection == '>')
       //{
         //fprintf(stderr, "Redirection not implemented \n");
       //}
       //else
       //{
        execvp(user_command_pipeoutput[0], user_command_pipeoutput);
        fprintf(stderr, "Child Process failed to execute execvp() system call\n");
       //}
       _exit(1);
      }     
      close(fd[0]);
      close(fd[1]);
      wait(0);
      wait(0);
      redirection_pipe = 0;
    }
  else
  {
  int pid = fork();
  if(pid < 0)
  {
   printf("Child process cannot be created\n");
  }
  else if(pid == 0)
  {
     //execv(exe_abs_path, user_command);
     execvp(user_command[0], user_command);
     fprintf(stderr, "Child Process failed to execute execvp() system call\n");
     _exit(1);
  } 
  }
  wait(NULL);
  printf("Child process exited \n");
 }
}
 return 0;
}
