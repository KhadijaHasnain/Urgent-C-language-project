#include "mysync.h"

void process_directory(const char *dirname) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(dirname);

    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Ignore "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}


void synchronize_files(const char *src, const char *dest) {
    DIR *src_dir, *dest_dir;
    struct dirent *entry;
    struct stat src_stat, dest_stat;
    char src_path[PATH_MAX], dest_path[PATH_MAX];
    int src_fd, dest_fd;
    char buffer[4096];
    ssize_t bytes_read, bytes_written;

    src_dir = opendir(src);
    dest_dir = opendir(dest);

    if (src_dir == NULL || dest_dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(src_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        sprintf(src_path, "%s/%s", src, entry->d_name);
        sprintf(dest_path, "%s/%s", dest, entry->d_name);

        if (stat(src_path, &src_stat) != 0) {
            perror("stat");
            continue;
        }

        if (stat(dest_path, &dest_stat) == 0) {
            if (src_stat.st_mtime > dest_stat.st_mtime) {
                src_fd = open(src_path, O_RDONLY);
                dest_fd = open(dest_path, O_WRONLY | O_TRUNC);
                while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
                    bytes_written = write(dest_fd, buffer, bytes_read);
                    if (bytes_written != bytes_read) {
                        perror("write");
                        break;
                    }
                }
                close(src_fd);
                close(dest_fd);
            }
        } else {
            src_fd = open(src_path, O_RDONLY);
            dest_fd = open(dest_path, O_WRONLY | O_CREAT, src_stat.st_mode);
            while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
                bytes_written = write(dest_fd, buffer, bytes_read);
                if (bytes_written != bytes_read) {
                    perror("write");
                    break;
                }
            }
            close(src_fd);
            close(dest_fd);
        }
    }

    closedir(src_dir);
    closedir(dest_dir);
}

void usage(char *program_name) {
    fprintf(stderr, "Usage: %s [-ainoprv] source_directory destination_directory\n", program_name);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        usage(argv[0]);
    }

    char *src = argv[argc - 2]; // Second to last argument is source directory
    char *dest = argv[argc - 1]; // Last argument is destination directory
    int opt;

    while ((opt = getopt(argc, argv, "ainoprv")) != -1) {
        switch (opt) {
            case 'a':
                 process_directory(src);
                 process_directory(dest);
                break;
            case 'i':
                printf(optarg);
                break;
            case 'n':
                printf("Identifying files to be copied (but not actually copying)\n");
                break;
            case 'o':
                printf("Synchronizing files matching pattern: %s\n", optarg);
                break;
            case 'p':
                process_directory(optarg);
                break;
            case 'r':
                printf("Recursively processing directories\n");
                break;
            case 'v':
                printf("Enabling verbose mode\n");
                break;
            default:
                fprintf(stderr, "Usage: %s [-ainoprv] directory1 directory2 [directory3 ...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Assuming directory names are given as arguments
    for (int i = optind + 1; i < argc; i++) {
        synchronize_files(src, argv[i]);
    return 0;
}
}
