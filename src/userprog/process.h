#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

struct child_parent {
  struct list_elem child;
  uint8_t child_parent_status;
  tid_t child_tid;
  uint8_t exit_code;
  struct semaphore child_dead;
};

struct child_exec {
  bool success;
  char* cmd_line;
  struct semaphore child_loaded;
  struct child_parent* child_parent;
};

#endif /* userprog/process.h */
