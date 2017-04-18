#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
static bool check_pointer_address (const void *p);
static void halt (void);
static void exit (struct intr_frame *);
static void exec (struct intr_frame *);
static void wait (struct intr_frame *);
static void create (struct intr_frame *);
static void remove (struct intr_frame *);
static void open (struct intr_frame *);
static void filesize (struct intr_frame *);
static void read (struct intr_frame *);
static int write (struct intr_frame *);
static void seek (struct intr_frame *);
static void tell (struct intr_frame *);
static void close (struct intr_frame *);

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
    exit (f); 
  }

  int systemCall =  *stack_pointer;

  switch (systemCall)
  {
    case SYS_HALT:
      halt ();
 
    break;
    case SYS_EXIT:
      exit (f); 
    break; 
    case SYS_EXEC:
      exec (f);

    break;
    case SYS_WAIT:
      wait (f);
   
    break;
    case SYS_CREATE:
      create (f);

    break;
    case SYS_REMOVE:
      remove (f);

    break;
    case SYS_OPEN:
      open (f);

    break;
    case SYS_FILESIZE:
      filesize (f);

    break;
    case SYS_READ:
      read (f);

    break;
    case SYS_WRITE:
      write (f);
      return 0;

    break;
    case SYS_SEEK:
      seek (f);

    break;
    case SYS_TELL:
      tell (f);

    break;
    case SYS_CLOSE:
      close (f);
  
    break;
    default:
    //printf ("DEFAULT STATEMENT, thread exit will be called...\n");
    thread_exit ();
  }
  thread_exit ();
}

static bool 
check_pointer_address (const void *p)
{
  if (p == NULL)
  {
    return false;
  }
  if (is_user_vaddr (p))
  {
    void *pointer = pagedir_get_page (thread_current ()->pagedir, p);

    if (pointer == NULL)
    {
       return false;

    }
    else 
    {
       return true;
    }  

  }
  else
  {
    return false;
  }

}

static void
halt (void)
{ 
  shutdown_power_off();
}

static void
exit (struct intr_frame *f UNUSED)
{ 
  if(f->eax == -1)
    thread_exit();
  f->eax = 0;
  // exit (int status)
  printf ("Exit call!\n");
  // write code here


  // eventually exit
  thread_exit ();
}

static void
exec (struct intr_frame *f UNUSED)
{ 
  // exec (const char *file)

  printf ("exec called!\n");
  // f->eax = some numder
}
static void
wait (struct intr_frame *f UNUSED)
{ 
  // wait (pid_t pid)
  printf ("wait called!\n");
}

static void
create (struct intr_frame *f UNUSED)
{ 
  //create (const char *file, unsigned initial_size)
  printf ("create called!\n");
}

static void
remove (struct intr_frame *f UNUSED)
{ 
  // remove (const char *file)
  printf ("create called!\n");
}

static void
open (struct intr_frame *f UNUSED)
{ 
  //open (const char *file)
  printf ("open called!\n");
}

static void
filesize (struct intr_frame *f UNUSED)
{ 
  //filesize (int fd)
  printf ("filesize called!\n");
}

static void
read (struct intr_frame *f UNUSED)
{ 
  //read (int fd, void *buffer, unsigned size)
  printf ("read called!\n");
}

static int 
write (struct intr_frame *f UNUSED)
{ 
  int *stack_pointer = f->esp;
  void** sp_cpy = f->esp;


  int fd = (int) *(sp_cpy + 1);
  if(fd == STDOUT_FILENO) {

    const char* buffer = (char *) *(sp_cpy + 2);
    size_t size = (size_t) *(sp_cpy + 3);
    putbuf(buffer, size);

    f->eax = size;
    return 0;
  }

}

static void
seek (struct intr_frame *f UNUSED)
{
  //seek (int fd, unsigned position)
  printf ("seek called!\n");
}

static void
tell (struct intr_frame *f UNUSED)
{
  //tell (int fd)
  printf ("tell called!\n");
}

static void
close (struct intr_frame *f UNUSED)
{
  //close (int fd)
  printf ("close called!\n");
}
