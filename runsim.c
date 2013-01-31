#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "restart.h"


int main( int argc , char *argv[] )
{

     int pr_limit ; // max number of process 
     int pr_count ; // the number of active process
     char str[20] ; // save the read commond line
     char *pos ;
     char ** myargv ;
     pid_t childpid ;

     if ( argc != 2 ) {
          fprintf( stderr , "Usage : %s string\n" , argv[0] ) ;
          return 1 ;
     }

     pr_limit = atoi( argv[1] ) ;
     pr_count = 0 ;

     while (1) {
          if ( pr_limit == pr_count ) {
               while ( r_wait(NULL) > 0 ) ;
               pr_count -- ;
          }

          if ( fgets( str , 50 , stdin ) == NULL ) break ;
          int len = strlen(str) ;
          if (  str[ len -1 ] == '\n' )
               str[len-1] = '\0' ;
          
          childpid = fork() ;
          if ( childpid == -1 ) {
               fprintf( stderr , "Failed to create process.\n" ) ;
          }
          if ( childpid == 0 ) {
               if ( makeargv( str , " " , &myargv ) == -1 ) {
                    perror( "Failed to construct argv" ) ;
                    return -1 ;
               }
               else {
                    execvp( myargv[0] , &myargv[0] ) ;
                    perror( "Child failed to exec command"  )  ;
               }
               freemakeargv( myargv ) ;

               pr_count++ ;
               if ( waitpid( -1 , NULL , WNOHANG ) > 0 ) //pid == -1 表是等待任一进程
                    pr_count -- ;
          }
          while ( r_wait( NULL ) > 0 ) ;
     }
     return 0 ;
}
