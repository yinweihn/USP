#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "restart.h"

int main( int argc , char **argv )
{
     pid_t  childpid ;
     char delim[] = " \t" ;
     char ** myargv ;

     if (  argc != 2 ) {
          fprintf( stderr , "Usage: %s\n" , argv[0] ) ;
          return 1 ;
     }
     childpid = fork() ;
     if (childpid == -1  ) {
          perror( "Failed to fork." ) ;
          return 1 ;
     }
     if ( childpid == 0 ) {
          if ( setsid() == -1 ){
               perror( "Child failed to become a session leader .") ; 
          }
          else if ( makeargv( argv[1] , delim , &myargv ) == -1 ) {
               fprintf( stderr , "Child failed to construct argument array\n" ) ;
          }
          else {
               execvp( myargv[0]  , &myargv[0] ) ;
               perror( "Child failed to exec command" ) ;
          }
     }
     return 0 ;
}
