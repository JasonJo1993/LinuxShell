/*
 * Name: Jason Sarwar
 * CS 631 - Advanced Programming in a UNIX Environment
 * Final Assignment
 * sish - A Simple Shell
 * December 19, 2016
 * sish.h
 */

#ifndef _SISH_H_
#define _SISH_H_

#ifndef NULL
#define NULL ((void*) 0)
#endif

char **tokenize(char*);
int execute(char*, char*, int, int);
void freeArray(char**);
#endif
