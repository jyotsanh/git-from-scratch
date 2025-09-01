#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <zconf.h>
#include <zlib.h>

int git_init();
int make_git_dir();

int git_cat_file(char *read_mode, char *hash_code);
unsigned char* decompress_object(unsigned char *buffer, long file_size, uLongf *out_size);
unsigned char* read_hash_file(FILE *fptr, long *file_size); // reads hash file and return read buffer
long get_file_size(FILE *fptr); // get the file size from which file has been open

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

    int status = make_git_dir(); // makes git init dir in current path.
    
    FILE *headFileptr = fopen(".git/HEAD", "w");
    if (headFileptr == NULL) {
        fprintf(stderr, "Failed to create .git/HEAD file: %s\n", strerror(errno));
        return 1;
    }
    fprintf(headFileptr, "ref: refs/heads/main\n");
    fclose(headFileptr);
    
    printf("Initialized git directory\n");
    return 0;
}

int git_cat_file(char *read_mode, char *hash_code){

    char hash_folder_name[3]; // assign 3 bytes of contigious memory
    char *hash_file_name;

    strncpy(hash_folder_name, hash_code, 2); // copy first two characters from hash
    hash_folder_name[2] = '\0'; // null terminate the string
    
    hash_file_name = &hash_code[2]; // hash file name starts from index 2

    fprintf(stdout, "this is the read_mode : %s\n", read_mode);
    fprintf(stdout, "this is the hash : %s\n", hash_code);
    fprintf(stdout, "this is the file name : %s\n", hash_file_name);
    fprintf(stdout, "this is the hash folder name : %s\n", hash_folder_name);

    char path_to_hash_file[100];
    snprintf(path_to_hash_file, sizeof(path_to_hash_file),".git/objects/%s/%s",hash_folder_name, hash_file_name);
    
    fprintf(stderr, "path to object : %s\n", path_to_hash_file);

    FILE *fptr = fopen(path_to_hash_file, "rb");
    if(fptr == NULL){
        fprintf(stderr, "Failed to open object file: %s\n",path_to_hash_file);    
        return 1;
    }

    long file_size = get_file_size(fptr);
    

    unsigned char *buffer = read_hash_file(fptr, &file_size);
    fclose(fptr);
    // decompress data

    uLongf decompressed_size = 4096;
    unsigned char *decompressed_data = decompress_object(buffer, file_size, &decompressed_size);
    free(buffer);
    
    char *ptr = memchr(decompressed_data, '\0', decompressed_size) + 1;
    size_t content_length = decompressed_size - (ptr - (char *)decompressed_data);

    fwrite(ptr, 1, content_length, stdout);
    free(decompressed_data);
    return 0;
}

int make_git_dir(){
    // 0755 -> -rwx-rx-rx
    if (mkdir(".git", 0755) == -1 || mkdir(".git/objects", 0755) == -1 || mkdir(".git/refs", 0755) == -1) {
        fprintf(stderr, "Failed to create directories: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}


unsigned char* read_hash_file(FILE *fptr, long *file_size){
    
    fseek(fptr, 0, SEEK_END); // go to end of the file
    *file_size = ftell(fptr); // get the filesize
    rewind(fptr); // move back to start of file
    
    unsigned char* buffer = malloc(*file_size);
    fread(buffer, 1, *file_size, fptr);
    
    return buffer;
}

long get_file_size(FILE *fptr){
    fseek(fptr, 0, SEEK_END); // go to end of the file
    long file_size = ftell(fptr); // get the filesize
    rewind(fptr); // move back to start of file

    return file_size;
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

