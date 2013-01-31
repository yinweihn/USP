#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>


#define BUFSIZE 1024
#define CREAT_FLAGS ( O_WRONLY | O_CREAT | O_TRUNC )
#define CREAT_MODE ( S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH )


int main(int argc , char *argv[])
{
     char buf[BUFSIZE] = "Hello";
     pid_t childpid = 0 ;
     int fd ;
     int i , n  ;

     if ( argc != 3 ) {
          fprintf( stderr , "Usage: %s processes filename\n", argv[0] ) ;
          return 1 ;
     }

     fd = open( argv[2] , CREAT_FLAGS , CREAT_MODE ) ;
            
     if ( fd < 0 ) {
          perror( "Failed to open file" ) ;
          return 1;
     }

     n = atoi( argv[1] ) ;
     for ( i = 1 ; i < n ; i++ ) {
          if ( childpid = fork() )
               break ;
     }

     if ( childpid == -1 ) {
          perror("Failed to fork.") ;
          return 1 ;
     }
     //fd = open( argv[2] , CREAT_FLAGS , CREAT_MODE ) ;
   
     sprintf( buf , "i:%d process : %ld " , i , ( long )getpid() ) ;
     write( fd , buf , strlen(buf) ) ;
//     sleep(1) ;
     
     sprintf( buf , "Paren: %ld child : %ld \n" , (long)getpid() , (long)childpid ) ;
     write( fd ,buf , strlen(buf) ) ;
     return 0 ;
}
