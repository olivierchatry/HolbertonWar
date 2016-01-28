#include "stdio.h"


typedef int   int32;
typedef short int16;
typedef char  int8;


typedef struct core_s {
  char* data;
  int   data_size;

  int   id;
  int   start_address;
} core_t;

typedef struct process_s {
  struct core_s* core;

} process_t;

int main(int ac, char** av) {

}
