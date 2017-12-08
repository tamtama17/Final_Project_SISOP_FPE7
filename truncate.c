#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int tryHelp (const char *msg) {
    char m[1024], h[1024] = {"Try 'truncate --help' for more information."};
    strcat (strcat (strcpy (m, "truncate: "), msg), "\n");
    puts (strcat (m, h));
    return 0;
}

int main (int argc, char **argv) {
    char cwd[1024];
    getcwd (cwd, sizeof (cwd));
    strcat (cwd, "/");
    
    // default controller values
    int size = -1, r = -1;
    FILE *f = NULL;
    int fileSize = -1;
    char *fileContent = NULL, *fileName = NULL;
    int c = 0;
    int o = 0;
    int s = 0;
    int exists = 0;
    
    for (int i = 1; i < argc; i++) {
        if (!strcmp (argv[i], "--help")) {
            FILE *help = fopen ("help/truncate.txt", "r");
            char line[1024];
            while (fgets (line, 1024, help))
                puts (line);
            fclose (help);
            return 0;
        }
        else if (!strcmp (argv[i], "-c") || !strcmp (argv[i], "--no-create")) {
            c = 1;
        }
        else if (!strcmp (argv[i], "-o") || !strcmp (argv[i], "--io-blocks")) {
            o = 1;
        }
        else if (!strcmp (argv[i], "-r") || !strncmp (argv[i], "--reference=", 12)) {
            if (i == argc - 1)
                return tryHelp ("option requires an argument -- 'r'");
            
            char fName[1024];
            if (argv[i+1][0] != '/')
                strcat (strcpy (fName, cwd), argv[i+1]);
            else
                strcpy (fName, argv[i+1]);
            
            FILE *reference = fopen (argv[i+1], "r");
            if (reference) {
                struct stat st;
                if (stat (fName, &st) == 0)
                    r = st.st_size;
                else
                    r = -1;
            }
            
            fclose (reference);
            i++;
        }
        else if (!strcmp (argv[i], "-s") || !strncmp (argv[i], "--size=", 7)) {
            if (i == argc - 1)
                return tryHelp ("option requires an argument -- 's'");
            
            char *sz = argv[i+1];
            
            if (*(sz+1) == '-')
                sz = strchr (argv[i+1], '=') + 1;
            
            while (*sz) {
                if (!isdigit (*sz)) {
                    char error[1024];
                    sprintf (error, "Invalid number: ‘%s’", argv[i+1]);
                    return tryHelp (error);
                }
                
                sz++;
            }
            
            size = atoi (argv[i+1]);
            s = 1;
            i++;
        }
        else {
            if (!f && (exists = access (argv[i], F_OK) != -1)) {
                char fName[1024];
                if (argv[i][0] != '/')
                    strcat (strcpy (fName, cwd), argv[i]);
                else
                    strcpy (fName, argv[i]);
                f = fopen (argv[i], "r");
                if (f) {
                    struct stat st;
                    if (stat (fName, &st) == 0)
                        fileSize = st.st_size;
                    else
                        fileSize = -1;
                }
                fileContent = malloc (fileSize * sizeof (char));
                char temp[1024];
                while (fgets (temp, 1024, f))
                    strcat (fileContent, temp);
            }
            fileName = argv[i];
        }
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
    
    if (!exists && c)
        // file doesn't exists and c (no-create) flag is on
        return 0;
    
    if (o) {
        if (!s)
            return tryHelp ("‘--io-blocks’ was specified but ‘--size’ was not");
        
        struct stat fi;
        stat ("/", &fi);
        size *= fi.st_blksize;
    }
    
    f = fopen (fileName, "w");
    for (int i = 0; i < size; i++)
        if (*fileContent) {
            fputc (*fileContent, f);
            fileContent++;
        }
        else
            fputc ('\0', f);
    fclose (f);
    
    return 0;
}
