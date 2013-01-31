#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

int main()
{
     char c = '@' ;
     int fd ;
     
     if ( (fd = open( "tst2.data" , O_RDONLY )) == -1 ) {
          perror("Failed to open file" ) ;
          return 1 ;
     }
     if ( fork() == -1 ) {
          perror( "Failed to fork!" ) ;
          return 1 ;
     }
     read( fd , &c , 1 ) ;
     printf( "Process %ld got %c\n" , (long)getpid() , c ) ;
     return 0 ;
}
