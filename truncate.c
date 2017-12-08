#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main (int argc, char **argv) {
    char args[1024];
    scanf ("%s", args);
    
    char args2[1024];
    scanf ("%s", args2);
    
    if (argc == 1) {
        puts ("truncate: you must specify either ‘--size’ or ‘--reference’");
        puts ("Try 'truncate --help' for more information.");
    }
    
    char cwd[1024];
    getcwd (cwd, sizeof (cwd));
    
    // default controller values
    int s = -1, r = -1;
    FILE *f = NULL;
    int fileSize = -1;
    char *fileContent = NULL, *fileName = NULL;
    int c = 1;
    int o = 0;
    int exists = 0;
    
    for (int i = 0; i < argc; i++) {
        if (!strcmp (argv[i], "--help")) {
            FILE *help = fopen ("help/truncate.txt", "r");
            char line[1024];
            while (fgets (line, 1024, help))
                puts (line);
            fclose (help);
            return 0;
        }
        else if (!strcmp (argv[i], "-c") || !strcmp (argv[i], "--no-create")) {
            c = 0;
        }
        else if (!strcmp (argv[i], "-o") || !strcmp (argv[i], "--io-blocks")) {
            o = 1;
        }
        else if (!strcmp (argv[i], "-r") || !strncmp (argv[i], "--reference=", 12)) {
            char fName[1024];
            if (argv[i+1][0] != '/')
                strcat (strcpy (fName, cwd), argv[i+1]);
            else
                strcpy (fName, argv[i+1]);
            printf ("reference name: %s\n", fName);
            FILE *reference = fopen (argv[i+1], "r");
            if (reference) {
                struct stat st;
                if (stat (fName, &st) == 0)
                    r = st.st_size;
                else
                    r = -1;
            }
            fclose (reference);
        }
        else if (!strcmp (argv[i], "-s") || !strncmp (argv[i], "--size=", 7)) {
            char *sz = argv[i+1];
            
            if (*(sz+1) == '-')
                sz = strchr (argv[i+1], '=') + 1;
            
            while (sz) {
                if (!isdigit (*sz)) {
                    printf ("truncate: Invalid number: ‘%s’", argv[i+1]);
                    return 0;
                }
                
                sz++;
            }
            
            s = atoi (sz);
            i++;
        }
        else {
            if (!f && (exists = access (argv[i], F_OK) != -1)) {
                char fName[1024];
                if (argv[i][0] != '/')
                    strcat (strcpy (fName, cwd), argv[i+1]);
                else
                    strcpy (fName, argv[i+1]);
                printf ("file name: %s\n", fName);
                f = fopen (argv[i], "r");
                if (f) {
                    struct stat st;
                    if (stat (fName, &st) == 0)
                        fileSize = st.st_size;
                    else
                        fileSize = -1;
                }
                fileContent = malloc (fileSize * sizeof (char));
                while (fgets (fileContent, 1024, f));
            }
            fileName = argv[i];
        }
    }
    
    if (!fileName) {
        // user didn't entered file name
        puts ("truncate: missing file operand");
        puts ("Try 'truncate --help' for more information.");
    }
    
    int size = r < s && r >= 0 ? r : s;
    if (size == -1) {
        // user didn't specify size or reference
        puts ("truncate: you must specify either ‘--size’ or ‘--reference’");
        puts ("Try 'truncate --help' for more information.");
    }
    
    if (!exists && c)
        // file doesn't exists and c (no-create) flag is on
        return 0;
    
    if (o) {
        struct stat fi;
        stat ("/", &fi);
        size *= fi.st_blksize;
    }
    
    f = fopen (fileName, "w");
    for (int i = 0; i < size && *fileContent; i++)
        if (i < size)
            fputc (*fileContent, f);
        else
            fputc ('\00', f);
    fclose (f);
    
    return 0;
}
