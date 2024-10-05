# - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Name : Jastegh Singh
# SID : 1721853
# CCID : jastegh
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Resourses used:
https://www.youtube.com/playlist?list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY

System Calls used:
access()
Checks the accessibility of a file (used in find_executable()).

chdir()
Changes the current working directory (used in cd_command()).

close()
Closes a file descriptor (used in handle_redirection() and execute_pipe_command()).

dup2()
Duplicates a file descriptor to a specified value (used in handle_redirection() and execute_pipe_command()).

execve()
_exit()
fork()
getcwd()
getenv()
getrusage()
kill()
open()
pipe()
signal()
waitpid()
write()


Testing: I tried testing all the tasks with the commands present as exampple in the assignment descriptiona and then use some commands of my own

--> Built in commands: Checked 'pwd' command is displaying the correct directory and changing the directory using 'cd' and using 'pwd' making sure the correct  directory is displayed

--> External Commands: For this I use multiple different commands, to check both absolute and realtive path are working as expected.
For ex:
/usr/bin/ls   and    ls

Also testing using some common external commands such as :
~echo Hello, world!
~cat text.txt

--> Input/Output Redirection: Tested output redirection by executing commands like 'ls > output.txt' to ensure that the output of ls is correctly written to the file output.txt. Verified by checking the contents of output.txt to confirm that it contains the directory listing. 

--> Piping: Tested the pipe functionality by running commands such as 'ls | grep ".c"' to ensure that the output of ls is correctly passed to grep


--> Background Execution: Tested background execution commands with '&' to commands.
oher ex:
~/bin/cat < input.txt &
~/usr/bin/ls &