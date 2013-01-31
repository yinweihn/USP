#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int setnonblock( int fd )
{
     int fdflag ;

     if ( ( fdflag = fcntl( fd , F_GETFL ,0 ) ) == -1 )
          return -1 ;
     fdflag |= O_NONBLOCK ;
     if ( fcntl( fd , F_SETFL , fdflag ) == -1 )
          return -1 ;
     return 0 ;
}



int setblock( int fd )
{
     int fdflag ;

     if ( ( fdflag = fcntl( fd , F_GETFL ,0 ) ) == -1 )
          return -1 ;
     fdflag &= ~O_NONBLOCK ;
     if ( fcntl( fd , F_SETFL , fdflag ) == -1 )
          return -1 ;
     return 0 ;
}
