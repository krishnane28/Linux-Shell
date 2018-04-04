# Linux-Shell
Linux Shell for Process Management with selection feature

## PERFORMANCE ANALYSIS:


- I have not created variables in my program in heap. So, all remains in stack under main method frame and no need 
   to 
explicitly free up memeory
 
- My program has two while loops - 
    
    - one while loop which runs as long as user exits from my program  
    
    - other while loop which is inside the while loop mentioned before will run for number of arguments entered by the user
    
      O(n) = N + n -> where N is the number of times the user wants to run this program 
      (till he presses exit or just enter(no
 commands)) and 
                   
      n is the number of tokens in the command entered by the user
   
      (n = 2 if user has entered ps -aux or n = 4 if the user has 
      entered ps -aux | more )




## LIMITATIONS:

 
- My shell implementation works only for one pipe 
 
- Redirection is not implemented 
(I will implement it in future)
 
- Shell builtin commands doesn't work in my shell	
	


## BUGS:
 
 
* If the user doesn't enter any commands and press enter I am capturing it and displaying the appropriate error message 
in
 my program
 
* If the user doesn't enter valid commands, I am displaying appropriate message

ASSUMPTIONS:
  
  
* I have made an assumption that user will not enter more than 80 tokens in a string(which is nothing but the commands)

