#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    const char* back_name = "Lab3.back";
    unsigned perms = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;
    char *input_data = malloc(sizeof(char) * 50);
    size_t size = 50;
    size_t map_size = 0;
    pid_t cpid;
    int counter = 0;
    char ch = ' ';

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((ch = getchar()) != EOF && ch != '\n') {
        if (counter < size) {
            input_data[counter++] = ch;
        } else {
            size *= 2;
            char *buffer = realloc(input_data, size);
            if (buffer == NULL) {
                perror("realloc failed");
                exit(EXIT_FAILURE);
            } else {
                input_data = buffer;
                input_data[counter++] = ch;
            }
        }
    }
    input_data[counter] = '\0';

    int fd = shm_open(back_name, O_RDWR | O_CREAT, perms);
    if (fd == -1) {
        perror("OPEN");
        exit(EXIT_FAILURE);
    }

    map_size = size + strlen(argv[1]) + 1;
    if (ftruncate(fd, map_size) == -1) {
        perror("ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }

    void *memptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memptr == MAP_FAILED) {
        perror("MMAP");
        close(fd);
        exit(EXIT_FAILURE);
    }

    memset(memptr, '\0', map_size);
    sprintf(memptr, "%s|%s", argv[1], input_data);

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        munmap(memptr, map_size);
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
        munmap(memptr, map_size);
        close(fd);
        execl("./child_program", "child_program", NULL); // Ensure the path to the child executable is correct
        perror("EXECL");
        exit(EXIT_FAILURE);
    } else {
        int status = 0;
        wait(&status);
        free(input_data);
        munmap(memptr, map_size);
        close(fd);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            exit(EXIT_SUCCESS);
        } else {
            exit(EXIT_FAILURE);
        }
    }
}
