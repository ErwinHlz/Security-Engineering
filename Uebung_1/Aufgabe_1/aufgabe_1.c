//
// Created by erwin on 15.04.2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){

  time_t current_time;
  time(&current_time);
  printf("Current time is: %s\n", ctime(&current_time));


  time_t start_time;
  struct tm * current;
  char start_time_string[64];

  time(&start_time);

  current = localtime(&start_time);

  strftime(start_time_string, 64, "%A %B %d %H:%M:%S CEST %Y", current);
  printf("Current time is: %s\n", start_time_string);

  return 0;

  }
