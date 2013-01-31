#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "atomic_logger.h"

#define BUFSIZE 1024

int main( int argc , char *argv[] )
{
     if ( argc != 3 ) {
          fprintf( stderr , "Usage: %s process filename.\n", argv[0] ) ;
          return 1 ;
     }

     char buf[BUFSIZE] ;
     pid_t childpid = 0 ;
     int i , n ;

     n = atoi( argv[1] ) ;
     for ( i = 1; i < n; i++ ) {
          if ( childpid = fork() )
               break ;
     }
     
     if ( childpid == -1 ) {
          perror( "Failed to create fork" ) ;
          return 1 ;
     }
         
     if ( atomic_log_open( argv[2] ) == -1 ) {
          fprintf(stderr , "Failed to open log file" ) ;
          return 1 ;
     }

	 
     /* sprintf( buf , "i = %d process :%ld " , i , (long)getpid() ) ; */
     /* atomic_log_array(buf , strlen(buf) ) ; */
     /* sprintf( buf , " parent %ld child : %ld \n" , (long)getpid() , (long)childpid ) ; */
     /* atomic_log_string( buf ) ; */

     atomic_log_printf( "i: %d process : %ld " , i , (long)getpid() ) ;
     atomic_log_printf( "Parent: %ld child ID : %ld\n", (long)getpid() , (long)childpid ) ;
     
     if ( atomic_log_send() == -1 ) {
          fprintf( stderr , "Failed to send to log file" ) ;
          return 1 ;
     }
     atomic_log_close() ;
     return 0 ;
}
