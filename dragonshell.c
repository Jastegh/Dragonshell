#include<string.h>

#include <stdlib.h>

#include <unistd.h>
#include <stdio.h>
#include <limits.h>


#include <sys/types.h>
#include <sys/wait.h>


#include <fcntl.h>

#include <signal.h>
#include <termios.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

pid_t pid_fg = -1;  // Global variable to track the current foreground process


/**
 * @brief Tokenize a C string 
 * 
 * @param str - The C string to tokenize 
 * @param delim - The C string containing delimiter character(s) 
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
void tokenize(char* str, const char* delim, char **argv) {
    char* token;
    size_t i = 0;
    token = strtok(str, delim);
    while (token != NULL) {
        argv[i] = token;
        token = strtok(NULL, delim);
        i++;
    }
    argv[i] = NULL;  // Ensure the array is null-terminated
}



//! /////    BUILT IN COMMANDS   //////////   --- 1st TASK
//A - cd command
void cd_command(char *path) {
    if (path == NULL) {
        // TODO : WHY is it not printing 
        // dragonshell: Expected argument to "cd"
        // and printing Invalid argument
        printf("dragonshell: Expected argument to \"cd\"\n");
    } else {
        if (chdir(path) != 0) {
            perror("dragonshell");
        }
    }
}

//B - pwd command
void pwd_command() {
    char *cwd = NULL;
    //size_t size = 0;

    // Use getcwd() with a NULL buffer to let it allocate as much space as needed.
    cwd = getcwd(NULL, 0);
    if (cwd != NULL) {
        printf("%s\n", cwd);
        free(cwd); // Free the allocated memory
    } else {
        perror("dragonshell");
    }
}


//C - exit command
long total_user_time = 0; // Track total user time
long total_sys_time = 0;  // Track total system time

// Function to accumulate process times
void accumulate_times() {
    struct rusage usage;
    getrusage(RUSAGE_CHILDREN, &usage);

    total_user_time += usage.ru_utime.tv_sec;
    total_sys_time += usage.ru_stime.tv_sec;
}


void exit_command() {
    // Terminate background processes gracefully
    if (pid_fg != -1) {
        kill(pid_fg, SIGTERM);  // Gracefully terminate any foreground process
        waitpid(pid_fg, NULL, 0);  // Wait for it to finish
    }

    // Accumulate the times for any remaining processes
    accumulate_times();

    // Print out the total execution times
    printf("User time: %ld seconds\n", total_user_time);
    printf("Sys time: %ld seconds\n", total_sys_time);

    // Exit the shell using _exit
    _exit(0);
}




//! /////////   Function to handle input/output redirection  ///////////    -- TASK 3
void handle_redirection(char **args) {
    int in_fd = -1, out_fd = -1;
    char *inp = NULL;
    char *out = NULL;

    //earching for redirection(<,>) operators
    for (int i= 0; args[i] != NULL;i++){
        if (strcmp(args[i], ">") == 0) {
            out = args[i + 1];  
            args[i] = NULL;     
        } else if (strcmp(args[i], "<") == 0) {
            inp = args[i + 1];  
            args[i] = NULL;     
        }
    }

    // output redirection(>)
    if (out != NULL) {
        out_fd = open(out,O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd < 0) {
            perror("dragonshell:Unable to open output file");
            _exit(EXIT_FAILURE);
        }
        dup2(out_fd, STDOUT_FILENO);  
        close(out_fd);  
    }

    // input redirection(<)
    if (inp != NULL) {
        in_fd = open(inp, O_RDONLY);
        if (in_fd< 0) {
            perror("dragonshell: Unable to open input file");
            _exit(EXIT_FAILURE);
        }
        dup2(in_fd,STDIN_FILENO);  
        close(in_fd);  
    }
}



//! /////   Function to handle executing external programs   ////////////    -- TASK 2
/*
This function searches for the executable in the PATH directories.
as unlike execvp, execve doesn't search for the path
*/
char *find_executable(char *command) {
    char *path = getenv("PATH");
    char *token = strtok(path, ":");
    char full_path[1024];

    while (token != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", token, command);
        if (access(full_path, X_OK) == 0) {
            return strdup(full_path);
        }
        token = strtok(NULL, ":");
    }
    return NULL;
}





void execute_external_command(char **args, int bg) {
    // Create a child process
    pid_t pid = fork();  
    int status;

    if (pid == 0) {
        // Child process
        handle_redirection(args);  
        char *executable;
        
        if (args[0][0] == '/' || args[0][0] == '.'){
            // it is a absolute path provided,we can just use it
            executable = args[0];  
        } else {
            //search
            executable = find_executable(args[0]);
        }

        if (executable==NULL){
            printf("dragonshell: Command not found\n");
            _exit(EXIT_FAILURE);  
        }
        if (execve(executable, args, NULL) == -1) {                         
            perror("dragonshell");
            _exit(EXIT_FAILURE);  
        }

    } else if (pid < 0) {
        // Fork failed
        perror("dragonshell");
    } else {

        // Parent process
        if (bg) {
            // Background execution
            printf("PID %d is sent to background\n",pid);
            // fflush(stdout);  // Ensure the output is flushed

            // // **Print the prompt again**
            // printf("dragonshell> ");
            // fflush(stdout);  // Ensure the prompt is displayed
            // Optionally, you can keep track of background processes if needed
        } else {
            // Foreground execution
            pid_fg = pid;  
            waitpid(pid, &status, 0);  
            pid_fg = -1;  
        }
    }
}




//!  ////////////     PIPE     //////////////////   -- TASK 4
/*
The execute_pipe_command function forks two child processes: one to run cmd1 and the other to run cmd2.

*/
void execute_pipe_command(char **cmd1, char **cmd2) {
    int fd[2];         // an array of size two
    pid_t pid1, pid2;       // PID's of two child processor 

    // Create a pipe
    if (pipe(fd) == -1) {              // if -1 that means it fails
        perror("dragonshell: pipe failed");
        _exit(EXIT_FAILURE);
    }

    // First fork to run cmd1
    pid1 = fork();
    if (pid1 < 0) {
        perror("dragonshell: fork failed");
        _exit(EXIT_FAILURE);
    }

    // Child process 1 - cmd1
    if (pid1 == 0) {         
        close(fd[0]);  
        dup2(fd[1], STDOUT_FILENO);  
        close(fd[1]);  
        handle_redirection(cmd1);  

        char *executable;
        if (cmd1[0][0] == '/' || cmd1[0][0] == '.') {
            executable = cmd1[0];  
        } else {
            executable = find_executable(cmd1[0]);  
        }

        execve(executable, cmd1, NULL);
        perror("dragonshell: execve failed");
        _exit(EXIT_FAILURE);
    }

    pid2 = fork();
    if (pid2 < 0) {
        perror("dragonshell: fork failed");
        _exit(EXIT_FAILURE);
    }

    // Child process 2 - cmd2
    if (pid2 == 0) {
        close(fd[1]);  
        dup2(fd[0], STDIN_FILENO);  
        close(fd[0]);  
        handle_redirection(cmd2); 

        char *executable;
        if (cmd2[0][0] == '/' || cmd2[0][0] == '.') {
            executable = cmd2[0];  
        } else {
            executable = find_executable(cmd2[0]);  
        }
        execve(executable, cmd2, NULL);
        perror("dragonshell: execve failed");
        _exit(EXIT_FAILURE);
    }

    close(fd[0]);  
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void handle_pipe(char *command) {
    char *cmd1[10], *cmd2[10];
    char *pipe_position = strstr(command, "|");        

    if (pipe_position != NULL) {
        // Split the command into two parts
        *pipe_position = '\0';  
        pipe_position++;  
        tokenize(command, " ", cmd1);  
        tokenize(pipe_position, " ", cmd2);  

        execute_pipe_command(cmd1, cmd2);  
    }
}




//!  Handling signals ///////////////   -- Task 5

// Signal handler for SIGINT (Ctrl+C)
void handle_sigint(int sig) {
    if (pid_fg >0) {      
        kill(pid_fg, SIGINT);
    } else {
        printf("\n"); 
        printf("dragonshell> ");
        fflush(stdout);  
    }
}


// Signal handler for SIGTSTP (Ctrl+Z)
void handle_sigtstp(int sig) {
    if (pid_fg >0) {
        kill(pid_fg, SIGTSTP);
    } else {
        printf("\n"); 
        printf("dragonshell> ");
        fflush(stdout);  
     }
}



void sigchld_handler(int sig) {
    int saved_errno = errno; 
    int status;
    pid_t pid;

    // Reap all terminated child processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Check if the terminated process is not the foreground process
        if (pid != pid_fg) {
            // Reprint the prompt
            const char *msg = "\ndragonshell> ";
            write(STDOUT_FILENO, msg, strlen(msg));
        } else {
            // The foreground process terminated
            pid_fg = -1; // Reset the foreground process
        }
    }

    errno = saved_errno; 
}



// //// control C and control Z not working same ///////////

// ///////////// This is also for handling signs tasks ////////////

// // Function to handle signals and set the process group
// void handle_signals_and_run(pid_t pid) {
//     pid_fg = pid;  // Set the current foreground process ID

//     // Put the child in its own process group
//     setpgid(pid, pid);

//     // Give the terminal control to the child process
//     tcsetpgrp(STDIN_FILENO, pid);

//     // Wait for the child process to finish
//     int status;
//     waitpid(pid, &status, WUNTRACED);

//     // After the child finishes or is stopped, return control to the shell
//     tcsetpgrp(STDIN_FILENO, getpid());
//     pid_fg = -1;
// }






int main(int argc, char *argv[]){
    char command[100];     
    char *args[10];     
    

    //! int background;
    

    // Print the welcome message
    printf("Welcome to Dragon Shell!\n\n");


    // Set up signal handling for SIGINT and SIGTSTP    
    signal(SIGINT, handle_sigint);     // Handle Ctrl + C
    signal(SIGTSTP, handle_sigtstp);   // Handle Ctrl + Z
    signal(SIGCHLD, sigchld_handler); // Handle child process state changes

    while (1) {
        int background = 0;  // Default to foreground execution

        printf("dragonshell> ");  // Shell prompt
        fgets(command, 100, stdin);  // Get input from user
        command[strcspn(command, "\n")] = 0;  // Remove newline character
        


        // Check if the command ends with '&'
        int len= strlen(command);
        if (len> 0 && command[len - 1]== '&') {
            background= 1;  // Set background flag
            command[len - 1] = '\0';  // Remove '&' from command
            // Remove any trailing spaces before the '&'
            while (len > 1 && command[len - 2] == ' ') {
                command[len - 2] = '\0';
                len--;
            }
        }


        if (strstr(command, "|")) {
            handle_pipe(command);          
        } else {

            tokenize(command, " ", args);  

            // **Add this check**
            if (args[0] == NULL) {
                continue;  
            }

            if (strcmp(args[0],"cd") == 0) {
                cd_command(args[1]);
            } else if (strcmp(args[0], "pwd") == 0) {
                pwd_command();
            } else if (strcmp(args[0],"exit") == 0) {
                exit_command();
            } else {
                execute_external_command(args,background);          
            }
        }
    }
    return 0;
}

