#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "list.h"
#include "process.h"


static void syscall_handler (struct intr_frame *);
static void* check_pointer_address (const void *p);
static void halt (void) NO_RETURN;
static void exit (int status);
static tid_t exec (const char *file);
static int wait (tid_t);
static bool create (const char *file, unsigned initial_size);
static bool remove (const char *file);
static int open (const char *file);
static int filesize (int fd);
static int read (int fd, void *buffer, unsigned length);
static int write (int fd, const void *buffer, unsigned length);
static void seek (int fd, unsigned position);
static unsigned tell (int fd);
static void close (int fd);
static void close_all_files(struct list* files);
 
struct proc_file {
	struct file* ptr;
	int fd;
	struct list_elem elem;
};

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf ("system call handler !\n");

  int * stack_pointer = f->esp;

  if (!check_pointer_address (stack_pointer))
  {
    printf ("not valid address, calling thread exit...\n");
    exit (-1); 
  }
    printf ("System call handler!\n");

  int systemCall =  *stack_pointer;

  switch (systemCall)
  {
    case SYS_HALT:
      halt ();
 
    break;
    case SYS_EXIT:
      check_pointer_address (stack_pointer + 1);
      exit (*(stack_pointer + 1)); 
    break; 
    case SYS_EXEC:
      check_pointer_address (stack_pointer + 1);
      check_pointer_address (*(stack_pointer + 1));
      f->eax = exec (*(stack_pointer + 1));

    break;
    case SYS_WAIT:
      check_pointer_address (stack_pointer + 1);
      f->eax = wait (*(stack_pointer + 1));
   
    break;
    case SYS_CREATE:
      check_pointer_address (stack_pointer + 5);
      check_pointer_address (*(stack_pointer + 4));
      acquire_filesys_lock();
      f->eax = create (*(stack_pointer + 4),*(stack_pointer + 5));
		release_filesys_lock();

    break;
    case SYS_REMOVE:
      /*check_pointer_address (stack_pointer + 1);
      check_pointer_address (*(stack_pointer + 1));
      acquire_filesys_lock();
      if((*(stack_pointer + 1))==NULL)
        f->eax = false;
      else
        f->eax = true;
		release_filesys_lock();
      remove (f);*/

    break;
  case SYS_OPEN:
    check_pointer_address (stack_pointer + 1);
    check_pointer_address (*(stack_pointer + 1));

    acquire_filesys_lock();
    struct file* fptr = filesys_open (*(stack_pointer + 1));
    release_filesys_lock();
    if(fptr==NULL)
      f->eax = -1;
    else
    {
      struct proc_file *pfile = malloc(sizeof(*pfile));
      pfile->ptr = fptr;
      pfile->fd = thread_current()->fd_count;
      thread_current()->fd_count++;
      list_push_back (&thread_current()->files, &pfile->elem);
      f->eax = pfile->fd;
    }

    break;
    case SYS_FILESIZE:

    break;
    case SYS_READ:

    break;
    case SYS_WRITE:
//      check_pointer_address (stack_pointer + 7);
      check_pointer_address (*(stack_pointer + 6));
      f->eax = write (*(stack_pointer + 5), *(stack_pointer + 6), (stack_pointer + 7));

    break;
    case SYS_SEEK:

    break;
    case SYS_TELL:

    break;
    case SYS_CLOSE:
  
    break;
    default:
    printf ("DEFAULT STATEMENT, thread exit will be called...\n");
    thread_exit ();
  }
}

void* 
check_pointer_address (const void *p)
{
  if (p == NULL)
  {
    exit (-1);
    return 0;
  }
  if (is_user_vaddr (p))
  {
    void *pointer = pagedir_get_page (thread_current ()->pagedir, p);

    if (pointer == NULL)
    {
       exit (-1);
       return 0;

    }
    else 
    {
       return pointer;
    }  

  }
  else
  {
    exit (-1);
    return 0;
  }

}

void
halt (void)
{ 
  shutdown_power_off();
}

void
exit (int status)
{ 
  struct list_elem *e;
  struct list child_process = thread_current()->parent->child_process;
  for (e = list_begin (&child_process); e != list_end (&child_process);
           e = list_next (e))
  {
    struct child *f = list_entry (e, struct child, elem);
    if(f->tid == thread_current()->tid)
    {
      f->used = true;
      f->exit_error = status;
    }
  }

  thread_current()->exit_error = status;

  if(thread_current()->parent->waiting_on_child == thread_current()->tid)
    sema_up(&thread_current()->parent->child_lock);

  thread_exit();
}

tid_t 
exec (const char *file)
{ 
  acquire_filesys_lock();
  char * fn_cp = malloc (strlen(file)+1);
  strlcpy(fn_cp, file, strlen(file)+1);
  
  char * save_ptr;
  fn_cp = strtok_r(fn_cp," ",&save_ptr);

  struct file* f = filesys_open (fn_cp);

  if(f==NULL)
  {
    release_filesys_lock();
    return -1;
  }
  else
  {
    file_close(f);
    release_filesys_lock();
    return process_execute(file);
  }
  // exec (const char *file)

  printf ("exec called!\n");
  // f->eax = some numder
}
int 
wait (tid_t pid)
{ 
  // wait (pid_t pid)
  printf ("wait called!\n");
  return process_wait (pid);
}

bool 
create (const char *file, unsigned initial_size)
{ 
  printf ("create called!\n");
  return filesys_create (file, initial_size);
  //create (const char *file, unsigned initial_size)
}

bool 
remove (const char *file)
{ 
  // remove (const char *file)
  printf ("create called!\n");
}

int 
open (const char *file)
{ 
  //open (const char *file)
  printf ("open called!\n");
}

int 
filesize (int fd)
{ 
  //filesize (int fd)
  printf ("filesize called!\n");
}

int 
read (int fd, void *buffer, unsigned length)
{ 
  //read (int fd, void *buffer, unsigned size)
  printf ("read called!\n");
}

int 
write (int fd, const void *buffer, unsigned length)
{ 
  printf ("System call write was made to!!\n");
  if(fd == STDOUT_FILENO) {
    putbuf(buffer, length);
    return length;
  }
  return 0;

}

void
seek (int fd, unsigned position)
{
  //seek (int fd, unsigned position)
  printf ("seek called!\n");
}

unsigned 
tell (int fd)
{
  //tell (int fd)
  printf ("tell called!\n");
}

void
close (int fd)
{
  //close (int fd)
  printf ("close called!\n");
}

void 
close_all_files (struct list* files)
{
  struct list_elem *e;

  while(!list_empty(files))
  {
    e = list_pop_front(files);

    struct proc_file *f = list_entry (e, struct proc_file, elem);
          
    file_close(f->ptr);
    list_remove(e);
    free(f);

  }
}
