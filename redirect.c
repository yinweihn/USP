#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "restart.h"

#define CREAT_FLAGS ( O_WRONLY | O_CREAT | O_APPEND )
#define CREAT_MODE ( S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH )

int main()
{
     int fd ;

     fd = open( "tst.data" , CREAT_FLAGS , CREAT_MODE ) ;
     if ( fd == -1 ) {
          perror("Failed to open tst2.data") ;
          return 1 ;
     }
     if ( dup2( fd , STDOUT_FILENO ) == -1 ) {
          perror( "Failed to redirect standard output" ) ;
          return 1 ;
     }
     if ( r_close(fd) == -1 ) {
          perror( "Failed to close the file" ) ;
          return 1 ;
     }
     if ( write(STDOUT_FILENO , "OK" , 2) == -1 ) {
          perror( "Failed in writing to file" ) ;
          return 1 ;
     }
     return 0 ;
}
