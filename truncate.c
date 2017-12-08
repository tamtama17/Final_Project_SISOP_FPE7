#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

char *getOption (const char *arg) {
    char *opt = malloc (1024 * sizeof (char));
    for (int i = 0; *arg && *arg != '='; i++, arg++)
        opt[i] = *arg;
    return opt;
}

int tryHelp (const char *msg) {
    char m[1024], h[1024] = {"Try 'truncate --help' for more information."};
    strcat (strcat (strcpy (m, "truncate: "), msg), "\n");
    puts (strcat (m, h));
    return 0;
}

int parseSize (const char *sz) {
    char *s = (char *) sz;
    int m = 1;
    
    char *mc[] = {
        "K", "KB", "M", "MB", "G", "GB", "T", "TB", "P", "PB", "E", "EB", "Z", "ZB", "Y", "YB",
    };
    
    while (*s) {
        if (!isdigit (*s)) {
            int valid = 0;
            for (int i = 0; i < 16; i++) {
                if (!strcmp (mc[i], s)) {
                    valid = 1;
                    int base = 0;
                    if (i % 2 == 0) base = 1024;
                    else base = 1000;
                    m = base;
                    for (int j = 0; j < i/2; j++)
                        m *= base;
                    break;
                }
            }
            
            if (!valid)
                return -2;
            *s = '\0';
            break;
        }
        
        s++;
    }
    
    return atoi (sz) * m;
}

int main (int argc, char **argv) {
    // default controller values
    int size = -1, r = -1;
    char *fileName[argc];
    int fileCount = 0;
    int c = 0;
    int o = 0;
    int s = 0;
    
    for (int i = 1; i < argc; i++) {
        if (!strcmp (argv[i], "--help")) {
            FILE *help = fopen ("help/truncate.txt", "r");
            char line[1024];
            while (fgets (line, 1024, help))
                puts (line);
            fclose (help);
            return 0;
        }
        else if (!strcmp (argv[i], "-c") || !strcmp (argv[i], "--no-create"))
            c = 1;
        else if (!strcmp (argv[i], "-o") || !strcmp (argv[i], "--io-blocks"))
            o = 1;
        else if (!strcmp (argv[i], "-r") || !strncmp (argv[i], "--reference=", 12)) {
            if (i == argc-1)
                return tryHelp ("option requires an argument -- 'r'");
            
            char *arg;
            if (argv[i][1] != '-')
                arg = argv[i+1];
            else
                arg = strchr (argv[i], '=') + 1;
            
            FILE *reference = fopen (argv[i+1], "r");
            if (reference) {
                struct stat st;
                if (stat (arg, &st) == 0)
                    r = st.st_size;
                else
                    r = -1;
            }
            
            fclose (reference);
            i++;
        }
        else if (!strcmp (argv[i], "-s") || !strncmp (argv[i], "--size=", 7)) {
            int isshort = argv[i][1] != '-';
            
            if (isshort && i == argc-1)
                return tryHelp ("option requires an argument -- 's'");
            
            char *sz;
            
            if (isshort)
                sz = argv[i+1];
            else
                sz = strchr (argv[i+1], '=') + 1;
            
            size = parseSize (sz);
            
            if (size == -2) {
                char error[1024];
                sprintf (error, "Invalid number: ‘%s’", argv[i+1]);
                return tryHelp (error);
            }
            
            s = 1;
            i++;
        }
        else
            fileName[fileCount++] = argv[i];
    }
    
    if (!fileName)
        // user didn't entered file name
        return tryHelp ("missing file operand");
    
    if (r >= 0) {
        if (s)
            return tryHelp ("you must specify a relative ‘--size’ with ‘--reference’");
        size = r;
    }
    
    if (size == -1)
        // user didn't specify size or reference
        return tryHelp ("you must specify either ‘--size’ or ‘--reference’");
    
    if (o) {
        if (!s)
            return tryHelp ("‘--io-blocks’ was specified but ‘--size’ was not");
        
        struct stat fi;
        stat ("/", &fi);
        size *= fi.st_blksize;
    }
    
    for (int i = 0; i < fileCount; i++) {
        if (access (fileName[i], F_OK) == -1 && c)
            // file doesn't exists and c (no-create) flag is on
            continue;
    
        FILE *f = fopen (fileName[i], "r");
        
        int fileSize;
        if (f) {
            struct stat st;
            if (stat (fileName[i], &st) == 0)
                fileSize = st.st_size;
            else
                fileSize = -1;
        }
    
        char *fileContent = malloc (fileSize * sizeof (char));
        char temp[1024];
        while (fgets (temp, 1024, f))
            strcat (fileContent, temp);
        
        fclose (f);
        f = fopen (fileName[i], "w");
    
        for (int j = 0; j < size; j++) {
            if (*fileContent)
                fputc (*(fileContent++), f);
            else
                fputc ('\0', f);
        }
        
        fclose (f);
    }
    
    return 0;
}
