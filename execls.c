#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
     pid_t childpid  ;

     childpid = fork() ;

     if ( childpid == -1 ) {
          perror( "Failed to fork!" ) ;
          return 1 ;
     }

     if ( childpid == 0 ) {
          execl( "/bin/ls" , "ls" , "-l" , NULL ) ;
          perror( "Child failed o exec ls." ) ;
          return 1;
     }

     if ( childpid != wait(NULL) ) {
          perror( "Parent failed o wait due to signal or error" ) ;
          return 1 ;
     }
     return 0 ;
}

