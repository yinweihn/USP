#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#define OPEN_FLAGS ( O_WRONLY | O_CREAT | O_APPEND )
#define FILE_PERMS ( S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH )

typedef struct list {
     char *entry ;
     int len ;
     struct list *next ;
} list ;

static int fd = -1 ;
static list *first = NULL ;
static list *last = NULL ;

/* **************** private functions ************ */

/* This is the same as write , bur restarts if interrupted by a signal */

static ssize_t my_write( int fd , void *buf , size_t size )
{
     ssize_t bytes ;
     while ( (bytes = write( fd , buf , size ) ) == -1 && errno == EINTR ) ;
     return bytes ;
}


/* Insert an entry with the given len field , but allocate extra bytes */
/* Return point to the new entry on sucess or NULL on failure */
static list *insert_new_entry( int len , int extra )
{
     char *new_str ;
     list *new_entry ;

     new_entry = (list*)malloc( sizeof( list ) + len + extra ) ;
     if ( new_entry == NULL ) {
          return NULL ;
     }

     new_str = (char*)new_entry + sizeof(list) ;
     new_entry->entry = new_str ;
     new_entry->len = len ;
     if ( last == NULL )
          first = new_entry ;
     else last->next = new_entry ;
     last = new_entry ;
     return new_entry ;
}
          

/* Return the sun of the length of all the entries */
static int get_lenght()
{
     int len = 0 ;
     list *current ;
     for ( current = first ; current != NULL ; current = current->next )
          len += current->len ;
     return len ;
}

/* Clear the list and free all space  */
static void clear()
{
     list *current ;
     list *free_entry ;
     for ( current = first ; current != NULL ; current = current->next , free(free_entry) ) {
          free_entry = current ;
     }
     first = NULL ;
     last = NULL ;
}

/* ************** Public function ***************** */

/* Open the given file for logging */
/* If sucessful , return 0 , Otherwise , return -1 with errno set */
int atomic_log_open( char *fn )
{
     while ( fd = open(fn , OPEN_FLAGS , FILE_PERMS) , fd == -1 && errno == EINTR  ) ;
     if ( fd < 0 )
          return -1 ;
     return 0 ;
}


/* Insert the given array with given size in the list */
/* If successful , return 0 , Otherwise , return -1 with errno set */
int atomic_log_array( char *s , int len )
{
     list *new_entry ;

     if ( fd < 0 )  {
          errno = EINVAL ;
          return -1 ;
     }
     new_entry = insert_new_entry( len , 0 ) ;
     if ( new_entry == NULL )
          return -1 ;
     (void)memcpy( new_entry ->entry , s , len ) ;
     return 0 ;
}


/* Insert the given string in the first */
/* Do not include the string terminator */
/* if successful , return 0 , Otherwise , return -1 with errno set  */
int atomic_log_string ( char *s )
{
     return atomic_log_array( s , strlen(s) ) ;
}

/* Insert an entry in the list . */
/* The syntax is similar to printf . */
/* Include the string terminator but do not count it in the lenght . */
/* If successful , return 0 , Otherwise, return -1 with errno set .   */
int atomic_log_printf( char *fmt , ... )
{
     va_list ap ;
     char ch ;
     int len ;
     list *new_entry ;

     if (fd < 0 ) {
          errno = EINVAL ;
          return -1  ;
     }
     va_start( ap , fmt ) ;
     len = vsnprintf( &ch , 1 , fmt , ap ) ;
     new_entry = insert_new_entry( len ,1 ) ;
     if ( new_entry == NULL )
          return -1 ;
     vsprintf( new_entry->entry , fmt , ap ) ;
     return 0 ;
}

/* Attempt to log the entire list with a single write
   Clear the list if successful .
   If successful , return 0 , Otherwise , return -1 with errno set
   If eht entire list cannot be logged with a single write , this is 
   considered a failure .
 */
int atomic_log_send()
{
     char *buf ;
     list *current ;
     int len ;

     if ( fd < 0 ) {
          errno = EINVAL ;
          return -1 ;
     }

     len = get_lenght() ;
     if ( len == 0 ) {
          return 0 ;
     }

     buf = ( char* )malloc(len) ;
     if ( buf == NULL )
          return -1 ;
     for ( current = first ,len = 0 ;
           current != NULL ;
           current = current -> next )
     {
          (void)memcpy( buf+len , current->entry , current->len ) ;
           len += current->len ;
     }

     if ( my_write( fd , buf , len ) != len ) {
          free( buf) ;
          errno = EAGAIN ;
          return -1 ;
     }
     free( buf ) ;
     clear() ;
     return 0 ;
}

/* Close the log file. Any data not yet logged is lost. */
int atomic_log_close()
{
     int retval ;
     clear() ;
     while ( retval = close(fd) , fd == -1 && errno == EINTR ) ;
     return retval ;
}

