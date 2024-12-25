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
#include <math.h>

bool is_prime(int num);

int main() {
    const char* back_name = "Lab3.back";
    unsigned perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char filename[20];
    size_t readed_data_id = 0;
    int map_fd = shm_open(back_name, O_RDWR, perms);
    if (map_fd < 0) {
        perror("SHM_OPEN");
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    if (fstat(map_fd, &statbuf) == -1) {
        perror("fstat");
        close(map_fd);
        exit(EXIT_FAILURE);
    }
    const size_t map_size = statbuf.st_size;

    void *memptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, map_fd, 0);
    if (memptr == MAP_FAILED) {
        perror("MMAP");
        close(map_fd);
        exit(EXIT_FAILURE);
    }

    char *input_data = malloc(sizeof(char) * (map_size + 1));
    if (input_data == NULL) {
        perror("malloc");
        munmap(memptr, map_size);
        close(map_fd);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < map_size; i++) {
        if (((char*)memptr)[i] != '|') {
            filename[i] = ((char*)memptr)[i];
        } else {
            filename[i] = '\0';
            readed_data_id = i + 1;
            break;
        }
    }

    // Use the filename variable
    printf("Filename: %s\n", filename);

    for (size_t i = readed_data_id; i < map_size; i++) {
        input_data[i - readed_data_id] = ((char*)memptr)[i];
    }
    input_data[map_size - readed_data_id] = '\0';

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        free(input_data);
        munmap(memptr, map_size);
        close(map_fd);
        exit(EXIT_FAILURE);
    }

    char *token = strtok(input_data, " ");
    while (token != NULL) {
        int num = atoi(token);
        if (num < 0 || is_prime(num)) {
            fclose(file);
            free(input_data);
            munmap(memptr, map_size);
            close(map_fd);
            exit(EXIT_SUCCESS);
        } else {
            fprintf(file, "%d\n", num);
        }
        token = strtok(NULL, " ");
    }

    fclose(file);
    free(input_data);
    munmap(memptr, map_size);
    close(map_fd);
    exit(EXIT_SUCCESS);
}

bool is_prime(int num) {
    if (num <= 1) return false;
    if (num <= 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
    }
    return true;
}
