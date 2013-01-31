#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "restart.h"


int main()
{
     pid_t child ;
     sigset_t mask , omask ;

     if ( ( sigfillset( &mask ) == -1 ) ||
          sigprocmask( SIG_SETMASK , &mask , &omask ) == -1 ) {
          perror( "Failed to block the signals" ) ;
          return 1 ;
     }

     if ( ( child = fork() ) == -1 ) {
          perror( "Failed to fork child" ) ;
          return 1 ;
     }
     if ( child == 0 ) {
          execl( "/bin/ls" , "ls" , "-l" , NULL ) ;
          perror( "Child failded to exec" ) ;
          return 1 ;
     }
     if ( sigprocmask( SIG_SETMASK , &omask , NULL ) == -1 ) {
          perror( "Parent failed to restore signal mask" ) ;
          return 1 ;
     }
   
     if ( r_wait( NULL ) == -1 ) {
          perror( "Parent failed to wait for child" ) ;
          return 1 ;
     }
     return 0 ;
}
