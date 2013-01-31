#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>


#ifndef ETIME
#define ETIME ETIMEDOUT
#endif

void freemakeargv( char ** argv ) ;
int makeargv( const char*s , const char *delimiters , char ***argvp ) ;

pid_t r_wait( int *stat_loc ) ;
pid_t r_waitpid( pid_t pid , int *stat_loc , int options ) ;

int readline( int fd , char *buf , int nbyte ) ;

ssize_t r_read( int fd , void *buf , size_t size ) ;
ssize_t r_write( int fd , void *buf , size_t size ) ;
int readwrite( int fromfd , int tofd ) ;

ssize_t readblock( int fd , void *buf , size_t size ) ;
int readwriteblock( int fromfd , int tofd , char *buf , int size ) ;

int copyfile( int fromfd , int tofd ) ;


int r_open2( const char* path , int oflag ) ;
int r_open3( const char* path , int oflag , mode_t mode ) ;

int r_close( int fildes ) ;

int r_dup2( int fildes , int fildes2 ) ;


struct timeval add2currenttime( double seconds ) ;
ssize_t readtimed( int fd , void *buf , size_t nbyte , double seconds ) ;
int waitfdtimed( int fd , struct timeval end ) ;
