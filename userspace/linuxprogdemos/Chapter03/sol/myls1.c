#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

/* 
 * Display file details similar to the ls command 
 * it recognises -i (inode), -l (long ) options
 * program shows directory contents if given a directory name
 */

#define TRUE 1
#define FALSE 0
#define MAXBUF 1024

void showFile (char *);
void showDirectory (char *);

int  lflag = 0, iflag = 0;

int main (int argc, char *argv[])
{
    int i;
    char ch;

    opterr = 0;

    while (( ch = getopt(argc, argv, "li")) != EOF) {

	switch (ch) {
	case 'l':  
	    lflag++; break;
	case 'i':  
	    iflag++; break;
	case '?':  
	    fprintf(stderr, "Unknown option %c\n", optopt);
	    fprintf (stderr, "Usage: %s [-ail] pathnames...\n", argv[0]);
	    exit(1);
        }
    }

    /* 
     * If no filename given list '.' 
     */
    if (optind == argc)  {
	argv[optind] = ".";
	argc += 1;
    }

    /* 
     * Process each argument
     */
    for (i = optind; i < argc; i++) {
	struct stat buffer;

	/* 
	 * get status details 
	 */
	if (stat(argv[i],&buffer) < 0) {
	    perror(argv[i]);
	    continue;
	}
    
	/* 
	 * list contents if it is a directory 
	 */
	if (S_ISDIR(buffer.st_mode)) {
	    if ( strcmp(argv[i], ".") )
		printf ("Directory %s:\n", argv[i]);
	    showDirectory(argv[i]);
	    printf("\n");

	} else {

	    /* 
	     * list entry if it's not a directory 
	     */
	    showFile(argv[i]);
	}
    }

    return 0;
}

/*
 * Display details for all entries in a directory
 */
void showDirectory (char *dirName)
{
  DIR *theDir;
  struct dirent *dentp;

  if ( (theDir = opendir(dirName)) != NULL ) {

    while(dentp=readdir(theDir)) {

      char pathName[MAXBUF];
      sprintf(pathName, "%s/%s", dirName, dentp->d_name);
      showFile(pathName);

    }
    closedir(theDir);

  } else
    perror(dirName);
}


/*
 * Display details of one file
 */
void showFile (char *path)
{
  struct stat buffer;
  int rwx;
  char ch;
  char *fileName;

  /* 
   * get status details 
   */
  if (stat(path,&buffer) < 0) {
      perror(path);
      return;
  }
   
  /* 
   * In the case where a pathname is supplied, we only want
   * to print the last component, ie the filename
   */
  if ( (fileName = strrchr(path, '/')) ) {
    if ( *(fileName+1) )
      fileName++;
  } else
    fileName = path;

     
  /* 
   * inode number (if required)
   */
  if (iflag)
    printf ("%6d  ", buffer.st_ino);
    
  /* 
   * type and access permissions (if required)
   */
  switch ( buffer.st_mode & S_IFMT )  {
  case S_IFDIR: 
      ch='d';     break;
  case S_IFREG: 
      ch='-';     break;
  case S_IFLNK: 
      ch='l';     break;
  case S_IFIFO: 
      ch='f';     break;
  case S_IFCHR: 
      ch='c';     break;
  case S_IFBLK: 
      ch='b';     break;
  default:      
      ch='?'; 
  }

  if (lflag) {
    char outstr[10];

    putchar(ch);
    
    rwx = buffer.st_mode & ( S_IRWXU | S_IRWXG | S_IRWXO );
    /* user */
    outstr[0] = ( (rwx & S_IRUSR) ? 'r' : '-');    
    outstr[1] = ( (rwx & S_IWUSR) ? 'w' : '-');    
    outstr[2] = ( (rwx & S_IXUSR) ? 'x' : '-');
    /* group */
    outstr[3] = ( (rwx & S_IRGRP) ? 'r' : '-');    
    outstr[4] = ( (rwx & S_IWGRP) ? 'w' : '-');    
    outstr[5] = ( (rwx & S_IXGRP) ? 'x' : '-');    
    /* everyone else */
    outstr[6] = ( (rwx & S_IROTH) ? 'r' : '-');    
    outstr[7] = ( (rwx & S_IWOTH) ? 'w' : '-');    
    outstr[8] = ( (rwx & S_IXOTH) ? 'x' : '-');
    outstr[9] = '\0';
    printf("%s  ", outstr);
  }

  /* 
   * the filename (always)
   */
  printf ("%s ", fileName);
    
  putchar('\n');
}
