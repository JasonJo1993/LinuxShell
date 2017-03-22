/*
 * Name: Jason Sarwar
 * CS 631 - Advanced Programming in a UNIX Environment
 * Final Assignment
 * sish - A Simple Shell
 * December 19, 2016
 * main.c
 */

#include <unistd.h> /* getopt, execve, getcwd */
#include <stdio.h> /* fprintf, printf */
#include <stdlib.h> /* setenv, getenv */
#include <string.h> /* strlen */
#include "sish.h"
#include <sys/stat.h> /* open */
#include <fcntl.h> /* open */

#define bufmax 1024

int main(int argc, char *argv[], char *envp[]) {

  int options = 0; // For getopt
  int i = 0; // For while loops
  int xFlag = 0;
  char buffer[bufmax];
  char PATH[bufmax];
  char *shell_backup = NULL;
  char *cFlag = NULL;
  char **tokens = NULL;
  int lastexitstatus = 0;

  while( (options = getopt(argc, argv, "+xc:")) != -1) {
    switch(options) {
    case 'x':
      xFlag = 1;
      break;
    case 'c':
      cFlag = optarg;
      break;
    case '?':
      return 127;
    default:
      fprintf(stderr, "Internal error\n");
      return 127;
    }
  }

  if(cFlag != NULL) {
    if(xFlag == 1) {
      fprintf(stderr, "+%s\n", cFlag);
    }
    if( (tokens = tokenize(cFlag)) == NULL) {
      fprintf(stderr, "command tokenization failed\n");
      return 127;
    }

    if( (execvp(tokens[0], tokens)) == -1) {
      fprintf(stderr, "execvp failed\n");
      return 127;
    }
    else {
      freeArray(tokens);
      return 0; //fix this
    }
  }

  shell_backup = getenv("SHELL");

  if( (getcwd(PATH, bufmax)) == NULL) {
    fprintf(stderr, "getcwd failed\n");
    return 127;
  }

  if( (setenv("SHELL", PATH, 1)) == -1) {
    fprintf(stderr, "setenv failed\n");
    return 127;
  }

  while(1) {
    printf("%s$ ", PATH);
    fflush(stdout);
    i = read(STDIN_FILENO, buffer, bufmax);
    if(i == 0)
      continue;
    
    else if(i < 0) {
      fprintf(stderr, "read from stdin failed: %s\n", strerror(i));
      return 127;
    }
    
    else {
      buffer[i - 1] = '\0';
      if( strcmp(buffer, "exit") == 0)
	break;
      else if( strcmp(buffer, "cd") == 0) {

	if(chdir(getenv("HOME")) == -1) {
	  fprintf(stderr, "chdir failed\n");
	  return 127;
	}

	if( (getcwd(PATH, bufmax)) == NULL) {
	  fprintf(stderr, "getcwd failed\n");
	  return 127;
	}
      }
      else if( strncmp(buffer, "cd ", 3) == 0) {

	if( (tokens = tokenize(buffer)) == NULL) {
	  fprintf(stderr, "tokenize failed\n");
	  return 127;
	}

	if(tokens[1] != NULL) {
	  if(chdir(tokens[1]) == -1) {
	    fprintf(stderr, "chdir failed\n");
	    return 127;
	  }
	}

	else {

	  if(chdir(getenv("HOME")) == -1) {
	    fprintf(stderr, "chdir failed\n");
	    return 127;
	  }

	  if( (getcwd(PATH, bufmax)) == NULL) {
	    fprintf(stderr, "getcwd failed\n");
	    return 127;
	  }

	}
	freeArray(tokens);
      }
      else if( strcmp(buffer, "echo") == 0) {
	printf("\n");
      }
      else if( strcmp(buffer, "echo $?") == 0) {
	printf("%i\n", lastexitstatus);
      }
      else if( strcmp(buffer, "echo $$") == 0) {
	printf("%i\n", getpid());
      }
      else if( strncmp(buffer, "echo ", 5) == 0) {
	printf("%s\n", &buffer[5]);
      }
      else {
	lastexitstatus = execute(buffer, PATH, 0, xFlag);
	printf("%i\n", lastexitstatus);
      }
    }


  }

  if(shell_backup != NULL) {
    if( (setenv("SHELL", shell_backup, 1)) == -1) {
      fprintf(stderr, "setenv failed\n");
      return 127;
    }
  }

  return lastexitstatus;
}
