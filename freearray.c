

#include <stdlib.h> /* free */

void freeArray(char **array) {
  int n = 0;
  while(array[n] != NULL) {
    free(array[n]);
    n++;
  }
  free(array);
}
