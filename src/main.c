#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <zconf.h>
#include <zlib.h>

int git_init();
int git_cat_file(char *read_mode, char *hash_code);
unsigned char* decompress_object(unsigned char *buffer, long file_size, uLongf *out_size);


int main(int argc, char *argv[]) {
    // Disable buffering so all stdout/stderr messages are printed immediately.
    // (Without this, some debug/output text might stay in a buffer and not show up right away.)
    setbuf(stdout, NULL); setbuf(stderr, NULL);
 
    if (argc < 2) {
        fprintf(stderr, "Usage: ./mygit <command> [<args>]\n");
        return 1;
    }
    
    const char *command = argv[1];
    
    if (strcmp(command, "init") == 0) {
    
        return git_init();    
        
    } else if(strcmp(command, "cat-file") == 0){

        if (argc != 4) {
            fprintf(stderr, "Usage: ./mygit cat-file <mode> <hash>\n");
            return 1;
        }

        return git_cat_file(argv[2], argv[3]);

    } 
    else {
        fprintf(stderr, "Unknown command %s\n", command);
        return 1;
    }
    return 0;
}


int git_init(){
    // You can use print statements as follows for debugging, they'll be visible when running tests.
    fprintf(stderr, "Logs from your program will appear here!\n");

    // Uncomment this block to pass the first stage
    
    if (mkdir(".git", 0755) == -1 || 
        mkdir(".git/objects", 0755) == -1 || 
        mkdir(".git/refs", 0755) == -1) {
        fprintf(stderr, "Failed to create directories: %s\n", strerror(errno));
        return 1;
    }
    // 
    FILE *headFile = fopen(".git/HEAD", "w");
    if (headFile == NULL) {
        fprintf(stderr, "Failed to create .git/HEAD file: %s\n", strerror(errno));
        return 1;
    }
    fprintf(headFile, "ref: refs/heads/main\n");
    fclose(headFile);
    
    printf("Initialized git directory\n");
    return 0;
}

int git_cat_file(char *read_mode, char *hash_code){

    char hash_folder_name[3];
    char *hash_file_name;

    strncpy(hash_folder_name, hash_code, 2); // copy first two characters from hash
    hash_folder_name[2] = '\0';
    
    hash_file_name = &hash_code[2]; // hash file name starts from index 2

    fprintf(stdout, "this is the read_mode : %s\n", read_mode);
    fprintf(stdout, "this is the hash : %s\n", hash_code);
    fprintf(stdout, "this is the file name : %s\n", hash_file_name);
    fprintf(stdout, "this is the has folder name : %s\n", hash_folder_name);

    char path_to_object[100];
    snprintf(path_to_object, sizeof(path_to_object),".git/objects/%s/%s",hash_folder_name, hash_file_name);
    fprintf(stderr, "path to object : %s\n", path_to_object);

    FILE *fp = fopen(path_to_object, "rb");
    if(fp == NULL){
        fprintf(stderr, "Failed to open object file: %s\n",path_to_object);    
        return 1;
    }

    fseek(fp, 0, SEEK_END); // go to end of the file
    long file_size = ftell(fp); // get the filesize
    rewind(fp); // move back to start of file

    unsigned char *buffer = malloc(file_size);
    if( buffer == NULL){
        fprintf(stderr, "Error in allocating the memory");
        
        fclose(fp);
        return 1;
    }
    fread(buffer, 1, file_size, fp);
    fclose(fp);

    // decompress data

    uLongf decompressed_size = 4096;
    unsigned char *decompressed_data = decompress_object(buffer, file_size, &decompressed_size);

    char *ptr = memchr(decompressed_data, '\0', decompressed_size) + 1;
    size_t content_length = decompressed_size - (ptr - (char *)decompressed_data);

    fwrite(ptr, 1, content_length, stdout);

    free(buffer);
    free(decompressed_data);
    return 0;
}



unsigned char* decompress_object(unsigned char *buffer, long file_size, uLongf *out_size) {
    Bytef *decompressed = malloc(*out_size);
    if (!decompressed) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    int res = uncompress(decompressed, out_size, buffer, file_size);
    if (res != Z_OK) {
        fprintf(stderr, "Decompression failed\n");
        free(decompressed);
        return NULL;
    }
    return decompressed;
}