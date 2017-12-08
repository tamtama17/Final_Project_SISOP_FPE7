#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

int showError (const char *err) {
    char m[1024], h[1024] = {"Try 'mv --help' for more information."};
    strcat (strcat (strcpy (m, "mv: "), err), "\n");
    puts (strcat (m, h));
    return 0;
}

int isShortOption (const char *arg) {
    return arg[1] == '-';
}

char *getOption (const char *arg) {
    char *opt = malloc (1024 * sizeof (char));
    for (int i = 0; *arg && *arg != '='; i++, arg++)
        opt[i] = *arg;
    return opt;
}

char *getLongArgument (const char *arg) {
    return strchr (arg, '=') + 1;
}

void addAll (char **src, int *srcSize, char *dirName) {
    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir (dirName)) != NULL) {
        while ((ent = readdir (dir)) != NULL)
            strcpy (src[(*srcSize)++], ent->d_name);
        closedir (dir);
    }
}

char *getDirName (const char *file) {
    char *lastslash = malloc (1024 * sizeof (char));
    strcpy (lastslash, file);
    *strrchr (lastslash, '/') = '\0';
    return lastslash;
}

int isDir (const char *dir) {
    struct stat sb;
    return stat (dir, &sb) == 0 && S_ISDIR (sb.st_mode);
}

char *getFileName (const char *path) {
    char *copy = malloc (1024 * sizeof (char));
    strcpy (copy, path);
    char *lastslash = strrchr (copy, '/');
    if (lastslash)
        return lastslash + 1;
    
    return copy;
}

void getSuffix (const char *arg, int *S) {
    if (!strcmp (arg, "none") || !strcmp (arg, "off"))
        // no backup
        *S = -1;
    else if (!strcmp (arg, "numbered") || !strcmp (arg, "t"))
        // numbered
        *S = 1;
    else if (!strcmp (arg, "existing") || !strcmp (arg, "nil"))
        // numbered if any backup exist, simple otherwise
        *S = 2;
    else
        // suffix not valid
        *S = -2;
}

int main (int argc, char **argv) {
    if (argc == 1)
        return showError ("missing file operand");
    
    if (argc == 2)
        return showError ("missing target file operand");
    
    char **src = malloc (argc * sizeof (char*));
    for (int a = 0; a < argc; a++)
        src[a] = malloc (256 * sizeof (char));
    int srcSize = 0;
    char target[256];
    int t = 0;
    
    for (int a = 1; a < argc; a++, srcSize++) {
        if (!strcmp (argv[a], "*")) {
            addAll (src, &srcSize, getDirName (argv[a]));
            strcpy (target, argv[argc-1]);
            t = 1;
            break;
        }
        else
            strcpy (src[srcSize], argv[a]);
    }
    
    if (!t) {
        strcpy (target, src[srcSize-1]);
        srcSize--;
    }
    
    if (srcSize > 1 && !isDir (target))
        return showError ("target is not a directory");
    
    for (int a = 0; a < srcSize; a++) {
        char tgt[256];
        strcpy (tgt, target);
        
        if (access (src[a], F_OK) == -1)
            showError ("file does not exists");
        
        if (isDir (target)) {
            if (tgt[strlen (tgt) - 1] != '/')
                strcat (tgt, "/");
            strcat (tgt, getFileName (src[a]));
        }
        
        if (!strcmp (src[a], tgt))
            return showError ("same file");
        
        rename (src[a], tgt);
        remove (src[a]);
    }
    
    return 0;
}
