/*
 * Name: Jason Sarwar
 * CS 631 - Advanced Programming in a UNIX Environment
 * Final Assignment
 * sish - A Simple Shell
 * December 19, 2016
 * tokenize.c
 */

#include <string.h> /* strncpy */
#include <stdlib.h> /* malloc */
#include <stdio.h> /* fprintf */
#include "sish.h"

char **tokenize(char *strLine) {

  char **ret = NULL;
  int *lengths = NULL;
  int *starts = NULL;
  int totalLen = strlen(strLine);
  int i = 0;
  int j = 0;
  int noOfTokens = 0;
  int quotesOn = -1;
  int quotesOn2 = -1;

  if(totalLen <= 0) {
    return ret;
  }

  lengths = (int*)malloc(sizeof (int) * totalLen);
  starts = (int*)malloc(sizeof (int) * totalLen);

  if(lengths == NULL || starts == NULL) {
    fprintf(stderr, "Error in Malloc\n");
  }

  starts[0] = 0;

  for(j = 0; j < totalLen; j++) {
    if( strLine[j] == 34 )
      quotesOn *= -1;
    else if( strLine[j] == 39 )
      quotesOn2 *= -1;
    else if( (strLine[j] == ' ' || strLine[j] == '\t') && j > 0 \
	     && quotesOn == -1 && quotesOn2 == -1) {
      if(strLine[j - 1] != ' ' && strLine[j - 1] != '\t') {
	lengths[noOfTokens] = j - starts[noOfTokens];
	noOfTokens++;
      }
    }
    else if(j > 0 && quotesOn == -1 && quotesOn2 == -1) {
      if(strLine[j - 1] == ' ' || strLine[j - 1] == '\t') {
	starts[noOfTokens] = j;
      }
    }
  }

  if(starts[noOfTokens] != 0 || noOfTokens == 0) {
    lengths[noOfTokens] = j - starts[noOfTokens];
    noOfTokens++;
  }

  ret = (char**)malloc(sizeof (char*) * (noOfTokens + 1));

  if(ret == NULL) {
    fprintf(stderr, "Error in Malloc\n");
  }

  for(i = 0; i < noOfTokens; i++) {
    ret[i] = (char*)malloc(sizeof (char) * (lengths[i] + 1));
    if( (strncpy(ret[i], &strLine[starts[i]], lengths[i])) == NULL) {
      fprintf(stderr, "Error in strncpy\n");
    }
    ret[i][lengths[i]] = '\0';
  }
  
  free(lengths);
  free(starts);

  ret[noOfTokens] = NULL;
  
  return ret;

}
