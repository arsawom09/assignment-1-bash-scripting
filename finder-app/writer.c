#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Open syslog with LOG_USER facility
    openlog("writer", LOG_PID | LOG_CONS, LOG_USER);

    // Check if arguments are exactly 2
    if (argc != 3) {
        syslog(LOG_ERR, "Invalid number of arguments: expected 2, got %d", argc - 1);
        fprintf(stderr, "Error: Two arguments required: <writefile> <writestr>\n");
        closelog();
        return 1;
    }

    const char *writefile = argv[1];
    const char *writestr = argv[2];

    // Log debug message before writing
    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);

    // Open file for writing
    FILE *fp = fopen(writefile, "w");
    if (fp == NULL) {
        syslog(LOG_ERR, "Failed to open file: %s", writefile);
        perror("Error opening file");
        closelog();
        return 1;
    }

    // Write string to file
    if (fputs(writestr, fp) == EOF) {
        syslog(LOG_ERR, "Failed to write string to file: %s", writefile);
        perror("Error writing to file");
        fclose(fp);
        closelog();
        return 1;
    }

    fclose(fp);
    closelog();
    return 0;
}
