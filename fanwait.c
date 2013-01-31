#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "restart.h"

int main( int argc , char *argv[] )
{

     pid_t childpid ;
     int i , n  ;

     if ( argc != 2 ) {
          fprintf( stderr ,  "Usage : %s\n" , argv[0] )   ;
          return 1 ;
     }

     n = atoi( argv[1] ) ;

     for ( i = 1 ; i < n ; i++  )
          if ( ( childpid = fork() ) <= 0  )
               break ;

     while ( r_wait(NULL) > 0 ) ;

     fprintf( stderr , "i : %d Process ID : %ld , Parent ID : %ld \
child ID %ld \n" , i , (long)getpid() , (long)getppid() , (long)childpid ) ;
     
     return 0 ;

}


