#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
// buffersize declaration saves cycles
#define buffersize 32*sizeof(uint8_t)
// prototypes
void readfile(char* path);
void parsefile(char* path,int fd);
void standardout(int fd);
void refile(char* path, int fd);
void ehandler(int bytes, int result);

int main(int argc, char* argv[]) {
	// checks if arguments are null.
	// argv[0] is the program itself so argument of 1 = null arguments
	if (argc != 1) {
		// parses each argument
		for (int x = argc-1; x > 0; x--) {
			readfile(argv[x]);
		}
	} else {
		// null is treated same as dash in cat
		readfile("-");
	}
	return EXIT_SUCCESS;
}

void readfile(char* path) {
	if (strcmp(path,"-") != 0) {
		int file = open(path,O_RDONLY);
		if (file != -1) {
			parsefile(path,file);
		} else {
			warn("%s not found",path);
			exit(EXIT_FAILURE);
		}
		close(file);
	} else {
		parsefile(path,STDIN_FILENO);
	}
}

void parsefile(char* path,int fd) {
	// check if flag exists
	if (strcmp(path,"-") != 0) {
		refile(path,fd);
	} else {
		standardout(fd);
	}
}

void standardout(int fd) {
	uint8_t * buffer = malloc(buffersize);
	int bytes = 1;
	int result = 1;
	while (bytes > 0 && result > 0 ) {
		bytes = read(STDIN_FILENO,buffer,buffersize);
		result = write(STDOUT_FILENO,buffer,bytes);
	}
	if ( bytes == -1 || result == -1) {
		ehandler(bytes,result);
	}
	free(buffer);
}

void refile(char* path, int fd) {
	// buffer for reader
	uint8_t * buffer = malloc(buffersize);
	int bytes = read(fd,buffer,buffersize);
	while (bytes > 0) {
		int result = write(STDOUT_FILENO,buffer,bytes);
		bytes = read(fd,buffer,buffersize);
		if ( bytes == -1 || result == -1) {
			ehandler(bytes,result);
		}
	}
	free(buffer);
}

void ehandler(int bytes, int result) {
	if (bytes == - 1) {
		warn("Error during read\n");
	} else if (result == -1) {
		warn("Error during write\n");
	} else {
		warn("Unspecified error\n");
	}
	exit(EXIT_FAILURE);
}