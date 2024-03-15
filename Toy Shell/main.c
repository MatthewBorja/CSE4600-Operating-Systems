/***************************************************************************//**
  @file         main.c
  @author       Stephen Brennan
  @date         Thursday,  8 January 2015
  @brief        LSH (Libstephen SHell)
*******************************************************************************/

/*
  Matthew Borja
  007819520
  March 2, 2023
  Purpose: Write a shell called myshell that runs on a Linux operating system.
 */

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Global Variables
char* shellname = "myshell";
char* terminator = ">";
int n = 0;			//number of new names.
char* newNames[10] = {};	//New name aliases array of size 10.
char* oldNames[10] = {};	//Default command names array of size 10.
//Constants
const int MAXLEN = 24;
/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int setshellname(char **args);
int stop(char **args);
int setterminator(char **args);
int newname(char **args);
int listnewnames(char **args);
int savenewnames(char **args);
int readnewnames(char **args);
/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "setshellname",
  "stop",
  "setterminator",
  "newname",
  "listnewnames",
  "savenewnames",
  "readnewnames"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &setshellname,
  &stop,
  &setterminator,
  &newname,
  &listnewnames,
  &savenewnames,
  &readnewnames
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/*
   Stop function: Terminates execution of current myshell session.
   Stop has no parameters and will not interpret any parameters.
   Return value: 0 to exit the program successfully.
 */
int stop(char **args)
{
  printf("Stopping %s now...\n", shellname);
  return EXIT_SUCCESS;
}

/*
   SetTerminator - Set terminator in myshell to a new symbol.
   SetTerminator requires one parameter. It requires a new character
   to replace the current terminator.
   Return Value: 1 to continue executing.
 
 */
int setterminator(char **args)
{
  if (args[1] == NULL)
    terminator = ">";
  else
    terminator = args[1];
  return 1;
}

/*
  newname - Manage the alias list.
  1 argument only: delete newname alias.
  2 arguments: set new alias with first arg, in reference to second arg.
  Return Value: 1 to continue executing.
 */
int newname(char **args)
{
  if (args[1] == NULL)
    fprintf(stderr, "newname expects arguments.\n");
  else
  {
    if (args[2] == NULL) //Option 3, utilize linear search.
    {
      int i = 0;
      while (i < n && strcmp(newNames[i], args[1]) != 0)
	i++;
      if (i < n) //Find matched newName
      {
        newNames[i] = newNames[n-1];
        oldNames[i] = oldNames[n-1];
        n--;
      }
    }
    else //Option 1, 2:
    {
      int i = 0;
      while (i < n && strcmp(args[2], oldNames[i]) != 0)
        i++;
      if (i < n) //Option 2: args[2] has an existing newName.
        newNames[i] = args[1];
      else //Option 1: Want to add a newname.
      {
        oldNames[n] = args[2];
	newNames[n] = args[1];
	n++;
      }
    }
  }
  return 1;
}

/*
   listnewnames - Output all aliases that have been defined.
   listnewnames requires no parameters. Parameters will not affect execution.
   Return Value: 1 to continue executing.
 */
int listnewnames(char **args)
{
  if (n > 0) //If there are aliases, output them.
  {
    for (int i = 0; i < n; i++)
    {
      printf("%s %s\n", newNames[i], oldNames[i]);
    }
  }
  else	    //Inform user no aliases exist.
  {
    printf("There are no new aliases. Assign a new one with \'newname <new name> <old name>\'.\n");
  }
  return 1;
}

/*
   savenewnames - Store all currently defined aliases in the file.
   savenewnames requires one parameter, the name of the file it will save to.
   Return value: 1 to continue executing.
 */
int savenewnames(char **args)
{
  if (args[1] == NULL)
  { 
    fprintf(stderr, "savenewnames expects arguments.\n\'savenewnames <file name>\'\n");
  }
  else if (n == 0)
  {
    printf("There are no aliases to save. Assign a new one with \'newname <newname> <oldname>\'.\n");
  }
  else
  {
    FILE *fp;
    char *filename = args[1];
    fp = fopen(filename, "w");
    for(int i = 0; i < n; i++)
    {
      fprintf(fp, "%s\n%s\n", newNames[i], oldNames[i]);
    }
    fclose(fp);
  }
  return 1;
}

/*
   readnewnames - Reads all aliases in file.
   readnewnames requires one parameter, the name of the file to read from.
   Return value: 1 to continue executing.
 */
int readnewnames(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "readnewnames expects arguments.\n\'readnewnames <file name>\'\n");
  }
  else
  {
    FILE *fp;
    char *filename = args[1];
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
      printf("File \"%s\" does not exist.\n", filename);
    }
    else
    {
      int i = 0;
      int j = 0;
      rewind(fp);
      while (!feof(fp) && !ferror(fp))
      {
	
		if(fgets(newNames[i], MAXLEN, fp) != NULL)
		{
			i++;
		}
		if(fgets(oldNames[j], MAXLEN, fp) != NULL)
		{
			j++;
		}
		n++;
      }
    }
    fclose(fp);
  }
  return 1;
}
/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

/*
   SetShellName - Set shell name to user preference
     or if no arguments then to default myshell.
 */
int setshellname(char **args)
{
    if (args[1] == NULL)
	    shellname = "myshell";
    else
	    shellname = args[1];
    return 1; 

}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i = 0;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }
  
  for (int j = 0; j < n; j++)
  {
    if (strcmp(args[0], newNames[j]) == 0)
      args[0] = oldNames[j];
  }
  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("%s %s ", shellname, terminator);
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    //free(line);
    //free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
