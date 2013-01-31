#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "restart.h"

#define BKLSIZE PIPE_BUF
#define MILLION 1000000L
#define D_MILLION 1000000.0


//private funtions of restart.c
static int gettimeout( struct timeval end , struct timeval *timeoutp )
{
     gettimeofday( timeoutp , NULL ) ;
     timeoutp -> tv_sec = end.tv_sec - timeoutp -> tv_sec ;
     timeoutp -> tv_usec = end.tv_usec - timeoutp -> tv_sec ;
     if ( timeoutp -> tv_usec >= MILLION ) {
          timeoutp -> tv_sec++ ;
          timeoutp -> tv_usec -= MILLION ;
     }
     if (  timeoutp -> tv_usec < 0  ) {
          timeoutp -> tv_sec -- ;
          timeoutp -> tv_usec += MILLION ;
     }
     if ( timeoutp -> tv_sec < 0 ||
          ( timeoutp -> tv_sec == 0 && timeoutp -> tv_usec == 0  ) ) {
          errno = ETIME ;
          return -1 ;
     }
     return 0 ;
}



//restart functions

void freemakeargv( char **argv )
{
     if (argv == NULL ) return ;
     if ( (*argv) != NULL )
          free( *argv ) ;
     free( argv ) ;
}

int makeargv( const char *s , const char *delimiters , char ***argvp  )
{
     int error ;
     int i;
     int numtokens ;
     const char *snew ;
     char *t ;

     if ( s == NULL || delimiters == NULL || argvp == NULL ) {
          error = EINVAL ;
          return -1 ;
     }

     *argvp = NULL ;

     snew = s + strspn( s , delimiters ) ;
     if ( (t = malloc( strlen(snew)+1) ) == NULL )
          return -1 ;


     strcpy( t , snew ) ;
     numtokens = 0 ;

     if ( strtok(t , delimiters) != NULL  )
          for ( numtokens = 1 ; strtok( NULL ,delimiters) != NULL ; numtokens ++ ) ;

     if ( (*argvp = malloc( (numtokens+1)*sizeof(char*) ) ) == NULL ) {
          error = errno ;
          free(t) ;
          errno = error ;
          return -1 ;
     }

     if ( numtokens == 0 )
          free(t) ;
     else {
          strcpy(t , snew) ;
          **argvp = strtok( t , delimiters ) ;
          for ( i = 1;  i < numtokens; i++ )
               *( *(argvp)+i ) =  strtok( NULL , delimiters ) ;
     }
     *( (*argvp) + numtokens ) = NULL ;
     return numtokens ;
}




pid_t r_wait( int *stat_loc  )
{
     int retval ;
     while ( ( (retval = wait(stat_loc)) == -1 ) && ( errno == EINTR ) )
          ;
     return retval ;
}

pid_t r_waitpid( pid_t pid , int *stat_loc , int options )
{
     pid_t retval ;
     while( (retval = waitpid(pid , stat_loc , options) ) == -1 && errno == EINTR ) ;
     return retval ;
}


int readline( int fd , char *buf , int nbyte )
{
     int numread = 0 ;
     int returnval ;

     while ( numread < nbyte -1 ) {
          returnval = read( fd , buf + numread , 1 ) ;
          if ( (returnval == -1) && ( errno == EINTR ) )
               continue ;
          if ( (returnval == 0) && (numread == 0) )
               return 0 ;
          if ( returnval == 0 )
               break ;
          if ( returnval == -1 )
               return -1 ;
          numread++ ;
          if ( buf[numread-1] == '\n' ) {
               buf[numread] = '\0' ;
               return numread ;
          }
     }
     errno = EINVAL ;
     return -1 ;
}

ssize_t r_read( int fd , void *buf , size_t size )
{
     ssize_t retval ;

     while ( retval = read( fd , buf , size ) , retval == -1 && errno == EINTR ) ;
     return retval ;
}

ssize_t r_write( int fd , void *buf , size_t size )
{
     char *bufp ;
     size_t bytestowrite ;
     ssize_t byteswritten ;
     size_t totalbytes ;

     for ( bufp = buf , bytestowrite = size , totalbytes = 0 ;
           bytestowrite > 0 ;
           bufp += byteswritten , bytestowrite -= byteswritten ) {
          if ( byteswritten == -1 && errno != EINTR )
               return -1 ;
          if ( byteswritten == -1 )
               byteswritten = 0 ;
          totalbytes += byteswritten ;
     }
     return totalbytes ;
}


int readwrite( int fromfd , int tofd )
{
     char buf[BKLSIZE] ;
     int bytesread ;
     
     if ( (bytesread = r_read( fromfd , buf, BKLSIZE ) ) < 0 )
          return -1 ;
     if ( bytesread == 0 )
          return 0 ;
     if ( r_write( tofd , buf , BKLSIZE ) < 0 )
          return -1 ;
     return bytesread ;
}


ssize_t readblock( int fd , void *buf , size_t size )
{
     char *bufp ;
     size_t bytestoread ;
     ssize_t bytesread ;
     size_t totalbytes ;

     for ( bufp = buf , bytestoread = size , totalbytes = 0 ;
           bytestoread > 0 ;
           bufp += bytesread , bytestoread -= bytesread ) {
          bytesread = read( fd , buf , bytestoread ) ;
          if ( bytesread == 0 && totalbytes == 0 )
               return 0 ;
          if ( bytesread == 0 ) {
               errno = EINVAL ;
               return -1 ;
          }

          if ( bytesread == -1 && errno != EINTR )
               return -1 ;
          if ( bytesread == -1  )
               bytesread = 0 ;
          totalbytes += bytesread ;
     }
     return totalbytes ;
}

int readwriteblock( int fromfd , int tofd , char *buf , int size )
{
     int bytesread ;
     
     bytesread = readblock( fromfd , buf , size ) ;
     if ( bytesread != size )
          return bytesread ;
     return r_write( tofd , buf , size ) ;
}
     
int copyfile( int fromfd , int tofd )
{
     int bytesread ;
     int totalbytes = 0 ;
     while ( (bytesread = readwrite(fromfd , tofd )) > 0 )
          totalbytes += bytesread ;
     return totalbytes ;
}


int r_open2( const char* path , int oflag )
{
     int retval ;
     while ( retval = open(path ,oflag ) , retval == -1 && errno == EINTR ) ;
     return retval ;
}

int r_open3( const char* path , int oflag , mode_t mode )
{
     int retval ;
     while ( retval = open( path , oflag , mode ) , retval == -1 && errno == EINTR ) ;
     return retval ;
}

int r_close( int fildes )
{
     int retval ;
     while ( retval = close( fildes ) , retval == -1 && errno == EINTR ) ;
     return retval ;
}

int r_dup2( int fildes , int fildes2 )
{
     int retval ;
     while ( retval = dup2( fildes , fildes2 ) , retval == -1 && errno == EINTR ) ;
     return retval ;
}



struct timeval add2currenttime( double seconds )
{
     struct timeval newtime ;

     gettimeofday( &newtime , NULL ) ;
     newtime.tv_sec += (int)seconds ;
     newtime.tv_usec += (int)( ( seconds - (int)seconds )*D_MILLION + 0.5) ;
     if ( newtime.tv_usec >= MILLION ) {
          newtime.tv_sec ++ ;
          newtime.tv_usec -= MILLION ;
     }
     return newtime ;
}

ssize_t readtimed( int fd , void *buf , size_t nbytes , double seconds )
{
     struct timeval timedone ;

     timedone = add2currenttime( seconds )  ;
     if ( waitfdtimed( fd , timedone ) == -1)
          return (ssize_t)-1 ;
     return r_read( fd , buf , nbytes ) ;
}

int waitfdtimed( int fd , struct timeval end )
{
     fd_set readset ;
     int retval ;
     struct timeval timeout ;

     if ( fd < 0 || fd >= FD_SETSIZE ) {
          errno = EINVAL ;
          return -1 ;
     }

     FD_ZERO( &readset ) ;
     FD_SET( fd , &readset ) ;
     if ( gettimeout( end , &timeout ) == -1 )
          return -1 ;
     while ( ( retval = select( fd+1 , &readset , NULL, NULL ,NULL )) == -1
             && errno == EINTR ) {
          if ( gettimeout( end , &timeout ) == -1 )
               return -1 ;
          FD_ZERO( &readset ) ;
          FD_SET( fd , &readset ) ;
          
     }

     if ( retval == 0 ) {
          errno = ETIME ;
          return -1 ;
     }
     return 0 ;
}
