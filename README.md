# Dragonshell (UNIX Shell)

## </> Overview 
**Dragonshell** is a custom UNIX-style command-line shell implemented in **C**. It incorporates core operating system concepts like process control, signal handling, and inter-process communication, offering a functional and interactive shell environment. 

Key features include:
- Built-in commands: `cd`, `pwd`, and `exit`.
- Support for background execution using `&`.
- Input/output redirection (`>` and `<`).
- Inter-process communication with piping (`|`).
- Reliable command execution using system calls like `fork`, `execve`, and `pipe`.



## 🛠️ Features
- **Built-in Commands:**
  - `cd`: Change the current directory.
  - `pwd`: Display the current working directory.
  - `exit`: Terminate the shell gracefully.
- **External Commands:**
  - Supports absolute and relative paths.
  - Examples: `/usr/bin/ls`, `ls`, `echo Hello, world!`
- **Input/Output Redirection:**
  - Example: `ls > output.txt` writes the directory listing to `output.txt`.
- **Piping:**
  - Example: `ls | grep ".c"` passes the output of `ls` to `grep`.
- **Background Execution:**
  - Example: `ls &` runs the command in the background.



## 🧪 Testing
Extensive testing was performed to ensure functionality and reliability:

### Built-in Commands
- Verified `pwd` correctly displays the current directory.
- Used `cd` to navigate directories and confirmed correctness with `pwd`.

### External Commands
- Tested absolute and relative paths:
  - Example: `/usr/bin/ls` and `ls`.
- Verified functionality of common commands:
  - Example: `echo Hello, world!`, `cat text.txt`.

### Input/Output Redirection
- Tested output redirection:
  - Example: `ls > output.txt` writes output to `output.txt`.
  - Verified file contents to confirm correct redirection.
  
### Piping
- Tested command chaining:
  - Example: `ls | grep ".c"` ensures output from `ls` is piped to `grep`.

### Background Execution
- Tested background commands:
  - Example: `ls &` runs `ls` in the background.
  - Example: `/bin/cat < input.txt &`.


## 📂 Project Structure
- `dragonshell.c`: Main source file for the shell.
- `README.md`: Documentation for the project (this file).
- Additional helper files or scripts, if any.


## 🚀 How to Build and Run
1. **Clone the Repository:**
    ```bash
    git clone https://github.com/yourusername/dragonshell.git
    cd dragonshell
    ```

2. **Build the Shell:**
    Use the provided `Makefile` to compile and link the project:
    - To build the executable:
      ```bash
      make
      ```
    - To clean up compiled files:
      ```bash
      make clean
      ```

3. **Run the Shell:**
    ```bash
    ./dragonshell
    ```


## 💡 Future Enhancements
- Add support for advanced job control (e.g., `fg`, `bg` commands).
- Improve error handling for unsupported commands.
- Implement a configuration file for customizable shell settings.


## 📜 License
This project is licensed under the [Apache 2.0 License](LICENSE).

