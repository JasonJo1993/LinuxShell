/*
 * Name: Jason Sarwar
 * CS 631 - Advanced Programming in a UNIX Environment
 * Final Assignment
 * sish - A Simple Shell
 * December 19, 2016
 * execute.c
 */

#include <unistd.h> /* execve, pipe, fork */
#include <stdio.h> /*  */
#include <string.h> /* strlen */
#include <stdlib.h> /* exit */
#include <sys/types.h> /* open */
#include <sys/stat.h> /* open */
#include <fcntl.h> /* open flags */
#include "sish.h"

#define bufmax 4096

int execute(char *line, char *path, int piped, int xFlag) {
  
  int pipe_fd[2];
  pid_t pid;
  int i = 0; // For loops
  int len = strlen(line);
  int n = 0; // To find control operators
  int quotesOn = -1;
  int quotesOn2 = -1;
  char *command = NULL;
  int redirectOut[10];
  int appendOut[10];
  int redirectIn[10];
  int rOut = 0;
  int aOut = 0;
  int rIn = 0;
  char *filename = NULL;
  int status;
  char **tokens = NULL;
  char buffer[bufmax];
  int x = 0; // for reads and writes
  char *tmpfile = "./tmpfilewwefsaxcvxcv.tmp";
  int tmp_fd = 0;

  if(line[0] == '|')
    line[0] = ' ';

  if(line[len - 1] == '|')
    line[len - 1] = ' ';

  for(i = 0; i < len; i++) {
    if(line[i] == 34) // If the character is a "
      quotesOn *= -1;
    if(line[i] == 39) // If the character is a '
      quotesOn2 *= -1;

    if(line[i] == '|' && quotesOn == -1 && quotesOn2 == -1) {
      n = i;
      break;
    }
  }

  if(n == 0) {
    //no more pipes for redirects

    i = 0;
    while(i < len - 2) {
      
      if(line[i] == '>' && line[i + 1] != '>') {
	//redirect output
	
	filename = (char*)malloc(sizeof(char) * (n + 1 + strlen(path)));
	strcpy(filename, path);
	strcat(filename, "/");
	
	line[i] = ' ';
	i++;
	while(line[i] == ' ' || line[i] == '\t') {
	  line[i] = ' ';
	  i++;
	}
	x = i;
	
	while(line[i] != ' ' && line[i] && '\t' && line[i] != '\n' && line[i] != '\0' && line[i] != '<' && line[i] != '>') {
	  i++;
	}
	strncat(filename, &line[x], i - x);
	strcat(filename, "\0");
	redirectOut[rOut] = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	rOut++;
	for( ; x < i; x++)
	  line[x] = ' ';
	
	free(filename);
      }
      else if(line[i] == '>' && line[i + 1] == '>') {
	//append output

	
	filename = (char*)malloc(sizeof(char) * (n + 1 + strlen(path)));
	strcpy(filename, path);
	strcat(filename, "/");
	
	line[i] = ' ';
	i += 2;
	while(line[i] == ' ' || line[i] == '\t') {
	  line[i] = ' ';
	  i++;
	}
	x = i;

	while(line[i] != ' ' && line[i] && '\t' && line[i] != '\n' && line[i] != '\0' && line[i] != '<' && line[i] != '>') {
	  i++;
	}
	strncat(filename, &line[x], i - x);
	strcat(filename, "\0");

	appendOut[aOut] = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	aOut++;
	for( ; x < i; x++)
	  line[x] = ' ';

	free(filename);

      }
      else if(line[i] == '<') {
	//redirect input

	filename = (char*)malloc(sizeof(char) * (n + 1 + strlen(path)));
	strcpy(filename, path);
	strcat(filename, "/");

	line[i] = ' ';
	i++;
	while(line[i] == ' ' || line[i] == '\t') {
	  line[i] = ' ';
	  i++;
	}
	x = i;

	while(line[i] != ' ' && line[i] && '\t' && line[i] != '\n' && line[i] != '\0' && line[i] != '<' && line[i] != '>') {
	  i++;
	}
	strncat(filename, &line[x], i - x);
	strcat(filename, "\0");
	redirectIn[rIn] = open(filename, O_RDONLY | O_CREAT);
	rIn++;
	for( ; x < i; x++)
	  line[x] = ' ';

	free(filename);


      }
      if(line[i] != '<' && line[i] != '>') 
	i++;
    }




    if(pipe(pipe_fd) == -1) {
      fprintf(stderr, "Pipe failed\n");
      return 127;
    }

    if((pid = fork()) < 0) {
      fprintf(stderr, "Fork failed\n");
      return 127;
    }

    if(pid == 0) {
      /* child */

      if(xFlag == 1) {
        fprintf(stderr, "+%s\n", line);
      }


      if( (tokens = tokenize(line)) == NULL) {
	fprintf(stderr, "Tokenize failed\n");
	exit(127);
      }

      close(pipe_fd[0]);

      if(piped == 1) {
	if( (tmp_fd = open(tmpfile, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) ) < 0) {
	  fprintf(stderr, "open failed: %s\n", strerror(tmp_fd));
	  exit(127);
	}
	
	dup2(tmp_fd, STDIN_FILENO);
      }

      for(x = 0; x < rIn; x++) {
	
	dup2(redirectIn[x], STDIN_FILENO);
	close(redirectIn[x]);
	
      }
      
      memset(&buffer[0], 0, bufmax);
      dup2(pipe_fd[1], STDOUT_FILENO);

      if( (execvp(tokens[0], tokens)) == -1) {
	fprintf(stderr, "command not recognized\n");
	exit(127);
      }
      
      freeArray(tokens);
      close(pipe_fd[1]);
      if(piped == 1)
	close(tmp_fd);

      exit(1);
    }
    else {
      /* parent */
      wait(&status);
      close(pipe_fd[1]);
      x = bufmax;

      while(x == bufmax) {
	if( ( x = read(pipe_fd[0], buffer, bufmax)) < 0) {
	  fprintf(stderr, "read from pipe failed\n");
	  return 127;
	}
	if(rOut != 0 || aOut != 0) {
	for(i = 0; i < rOut; i++) {
	  if( (write(redirectOut[i], buffer, x) < 0) ) {
	    fprintf(stderr, "write to redirect file failed");
	    return 127;
	  }	
	}

	for(i = 0; i < aOut; i++) {
	  if( (write(appendOut[i], buffer, x) < 0) ) {
	    fprintf(stderr, "write to append file failed");
	    return 127;
	  }	
	}
	}
	else { 
	  if( (write(STDOUT_FILENO, buffer, x) < 0) ) {
	    fprintf(stderr, "write to append file failed");
	    return 127;
	  }
	       
	}
	
	
      }
      close(pipe_fd[0]);
      memset(&buffer[0], 0, bufmax);
      unlink(tmpfile);
      return status;

    }
  }
  else {
    //n is not 0, there is a pipe

    i = 0;
    while(i < n - 2) {
      
      if(line[i] == '>' && line[i + 1] != '>') {
	//redirect output

	filename = (char*)malloc(sizeof(char) * (n + 1 + strlen(path)));
	strcpy(filename, path);
	strcat(filename, "/");

	line[i] = ' ';
	i++;
	while(line[i] == ' ' || line[i] == '\t') {
	  line[i] = ' ';
	  i++;
	}
	x = i;

	while(line[i] != ' ' && line[i] && '\t' && line[i] != '\n' && line[i] != '\0' && line[i] != '<' && line[i] != '>') {
	  i++;
	}
	strncat(filename, &line[x], i - x);
	strcat(filename, "\0");

	redirectOut[rOut] = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	rOut++;
	for( ; x < i; x++)
	  line[x] = ' ';

	free(filename);
      }
      else if(line[i] == '>' && line[i + 1] == '>') {
	//append output

	
	filename = (char*)malloc(sizeof(char) * (n + 1 + strlen(path)));
	strcpy(filename, path);
	strcat(filename, "/");

	line[i] = ' ';
	i += 2;
	while(line[i] == ' ' || line[i] == '\t') {
	  line[i] = ' ';
	  i++;
	}
	x = i;

	while(line[i] != ' ' && line[i] && '\t' && line[i] != '\n' && line[i] != '\0' && line[i] != '<' && line[i] != '>') {
	  i++;
	}
	strncat(filename, &line[x], i - x);
	strcat(filename, "\0");

	appendOut[aOut] = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	aOut++;
	for( ; x < i; x++)
	  line[x] = ' ';

	free(filename);

      }
      else if(line[i] == '<') {
	//redirect input

	filename = (char*)malloc(sizeof(char) * (n + 1 + strlen(path)));
	strcpy(filename, path);
	strcat(filename, "/");

	line[i] = ' ';
	i++;
	while(line[i] == ' ' || line[i] == '\t') {
	  line[i] = ' ';
	  i++;
	}
	x = i;

	while(line[i] != ' ' && line[i] && '\t' && line[i] != '\n' && line[i] != '\0' && line[i] != '<' && line[i] != '>') {
	  i++;
	}
	strncat(filename, &line[x], i - x);
	strcat(filename, "\0");
	redirectIn[rIn] = open(filename, O_RDONLY | O_CREAT);
	rIn++;
	for( ; x < i; x++)
	  line[x] = ' ';

	free(filename);


      }
      if(line[i] != '<' && line[i] != '>') 
	i++;
    }

    if(pipe(pipe_fd) == -1) {
      fprintf(stderr, "Pipe failed\n");
      return 127;
    }

    if((pid = fork()) < 0) {
      fprintf(stderr, "Fork failed\n");
      return 127;
    }

    if(pid == 0) {
      /* child */
      if( (command = (char*)malloc(sizeof(char) * n)) == NULL) {
	fprintf(stderr, "Malloc failed\n");
	exit(127);
      }

      strncpy(command, line, n);
      command[n] = '\0';

      if(xFlag == 1) {
        fprintf(stderr, "+%s\n", command);
      }

      if( (tokens = tokenize(command)) == NULL) {
	fprintf(stderr, "Tokenize failed\n");
	exit(127);
      }
      free(command);

      close(pipe_fd[0]);

      if( (tmp_fd = open(tmpfile, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) ) < 0) {
	fprintf(stderr, "open failed: %s\n", strerror(tmp_fd));
	exit(127);
      }
      
      i = bufmax;
      while(i == bufmax) {
	if( (i = read(tmp_fd, buffer, bufmax)) < 0) {
	  fprintf(stderr, "read failed\n");
	  exit(127);
	}

	if( (write(STDIN_FILENO, buffer, i)) < 0) {
	  fprintf(stderr, "write failed\n");
	  exit(127);
	}
      }

      for(x = 0; x < rIn; x++) {
	
       dup2(redirectIn[x], STDIN_FILENO);
       close(redirectIn[x]);
	
      }
      
      memset(&buffer[0], 0, bufmax);
      dup2(pipe_fd[1], STDOUT_FILENO); 
      

      if( (execvp(tokens[0], tokens)) == -1) {
	fprintf(stderr, "command not recognized\n");;
	exit(127);
      }

      freeArray(tokens);
      close(pipe_fd[1]);
      close(tmp_fd);
    }
    else {
      /* parent */
      wait(&status);
      printf("status: %i\n", status);
      close(pipe_fd[1]);
      x = bufmax;
      if( (tmp_fd = open(tmpfile, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) ) < 0) {
	fprintf(stderr, "open failed: %s\n", strerror(tmp_fd));
	return 127;
      }

      while(x == bufmax) {
	if( ( x = read(pipe_fd[0], buffer, bufmax)) < 0) {
	  fprintf(stderr, "read from pipe failed\n");
	  return 127;
	}
	for(i = 0; i < rOut; i++) {
	  if( (write(redirectOut[i], buffer, x) < 0) ) {
	    fprintf(stderr, "write to redirect file failed");
	    return 127;
	  }	
	}

	for(i = 0; i < aOut; i++) {
	  if( (write(appendOut[i], buffer, x) < 0) ) {
	    fprintf(stderr, "write to append file failed");
	    return 127;
	  }	
	}

	if( (write(tmp_fd, buffer, x) < 0) ) {
	  fprintf(stderr, "write to stdin failed");
	  return 127;
	}
	

      }
      close(pipe_fd[0]);
      memset(&buffer[0], 0, bufmax);
      close(tmp_fd);
      return execute(&line[n], path, 1, xFlag);
    
    }

  }



}
