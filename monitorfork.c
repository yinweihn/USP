#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "restart.h"


int main( int argc , char *argv[] )
{
     int bytesread ;
     pid_t childpid ;
     int fd , fd1 , fd2 ;

     if ( argc != 3 ) {
          fprintf( stderr , "Usage: %s file1 file2\n" , argv[0] ) ;
          return  1 ;
     }

     if ( (fd1 = open(argv[1] , O_RDONLY) ) == -1 ) {
          fprintf( stderr , "Failed to open file %s: %s \n" , argv[1] , strerror(errno) ) ;
          return 1 ;
     }
fprintf( stderr , "fd1 = %d \n" , fd1 ) ;
     if ( ( fd2 = open(argv[2]  , O_RDONLY ) ) == -1 ) {
          fprintf( stderr , "Faild to open file %s: %s \n" , argv[2] , strerror(errno) ) ;
          return 1 ;
     }
fprintf( stderr , "fd2 = %d \n" , fd2 ) ;
     if ( (childpid = fork() ) == -1 ) {
          perror("Failed to create child process") ;
          return  1 ;
     }

     if ( childpid > 0  )
          fd = fd1 ;
     else fd = fd2 ;

     fprintf( stderr , "fd = %d \n" , fd ) ;
     
     bytesread = copyfile( fd , STDOUT_FILENO ) ;
     fprintf( stderr , "Bytes read : %d \n" , bytesread ) ;
     
     return 0 ;
}
