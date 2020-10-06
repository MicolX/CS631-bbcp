/*
 * bbcp can copy file from [source] to [target], byte by byte.
 * author: Mingyao Xiong
 * version: 2
 */


#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define BUFFSIZE	1024


int
main(int argc, char **argv) 
{

	char *source = argv[1];			
	char *target = argv[2];     	
	int sourceFD;               	
	int targetFD;              
	char buf[BUFFSIZE];             
	int readSize;                   
	int writeSize; 	
	struct stat sourceSB;
	struct stat targetSB;

	if (argc != 3) {
		fprintf(stderr, "Usage: bbcp [source] [target].\n");
		exit(EXIT_FAILURE);
	}


	if ((sourceFD = open(source, O_RDONLY|O_REGULAR)) < 0) {
		fprintf(stderr, "bbcp [source]: Failed to open the source file. Error: %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if ((targetFD = open(target, O_WRONLY|O_CREAT|O_TRUNC|O_REGULAR, 0666)) < 0) {
		if ((targetFD = open(target, O_DIRECTORY)) < 0) {
			fprintf(stderr, "bbcp [target]: target MUST be either a regular file or a directory. Error: %s.\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else {	
			if ((targetFD = openat(targetFD, basename(source), O_WRONLY|O_CREAT, 0666)) < 0) {
				fprintf(stderr, "bbcp [target]: Failed to open the target file. Error: %s.\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}

	if (fstat(sourceFD, &sourceSB) == -1) {
		fprintf(stderr, "Failed to get stat of source: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fstat(targetFD, &targetSB) == -1) {
		fprintf(stderr, "Failed to get stat of target: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if ((sourceSB.st_ino == targetSB.st_ino) && (sourceSB.st_dev == targetSB.st_dev)) {
		fprintf(stderr, "source and target are identical(Not copied)\n");
		exit(EXIT_FAILURE);
	}

	while ((readSize = read(sourceFD, buf, sizeof(buf))) > 0) {
		if ((writeSize = write(targetFD, buf, readSize)) != readSize) {
			fprintf(stderr, "Error, read size and write size are different.");
			exit(EXIT_FAILURE);
		}
	}

	if (readSize == -1) {
		fprintf(stderr, "Failed to read from source file.\nError: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	(void)close(sourceFD);
	(void)close(targetFD);
	exit(EXIT_SUCCESS);
}


