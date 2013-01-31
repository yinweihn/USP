#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main( int argc , char **argv )
{
     int sleep_time ;
     int times ;
     int i ;
     
     if ( argc != 3 ) {
          fprintf(stderr , "Usage : %s \n" , argv[0] ) ;
          return 1 ; 
     }   

     sleep_time = atoi( argv[1] ) ;
     times = atoi( argv[2] )   ;

     for ( i = 0 ; i  < times ; i ++ )
          sleep( sleep_time ) ;

     fprintf(stderr , "Process ID :%ld\n" , (long)getpid() ) ;
     
     return 0 ;
}
