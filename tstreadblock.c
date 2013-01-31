#include <stdio.h>
#include <stdlib.h>
#include "restart.h"

struct point
{
     int x ;
     int y ;
} ;

int main()
{
     //struct point pt ;
     int pt ;
     pt = 0 ;
     if ( readblock( STDIN_FILENO , &pt , sizeof( int ) ) <= 0  )
          fprintf( stderr , "Cannot read a point\n" ) ;
     else fprintf( stderr , "x = %d , y = %d\n" , pt , pt ) ;
     return 0 ;
}
