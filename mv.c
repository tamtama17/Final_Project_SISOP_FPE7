#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/stat.h>

int showError (const char *err) {
    puts (err);
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
    printf ("copy: %s\n", copy);
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
        
        if (isDir (target)) {
            if (tgt[strlen (tgt) - 1] != '/')
                strcat (tgt, "/");
            strcat (tgt, getFileName (src[a]));
        }
        
        if (!strcmp (src[a], tgt))
            return showError ("same file");
        
        FILE *source = fopen (src[a], "r");
        FILE *destination = fopen (tgt, "w");
        char buffer[1024];
        
        while (fgets (buffer, 1024, source))
            fputs (buffer, destination);
        
        fclose (source);
        fclose (destination);
        remove (src[a]);
    }
//    int overwrite = -1;
//    int b = 0;
//    int s = 0;
//    int S = -1;
//    int t = 0;
//    int T = 0;
//    int u = 0;
//    int v = 0;
//    int Z = 0;
//    char src[argc][128];
//    int srcSize = 0;
//    char tgt[128];
//
//    for (int a = 1; a < argc; a++) {
//        if (!strcmp (argv[a], "-b") || !strncmp (argv[a], "--backup", 8))
//            S = 0;
//        else if (!strcmp (argv[a], "-f") || !strncmp (argv[a], "--force", 7))
//            overwrite = 0;
//        else if (!strcmp (argv[a], "-i") || !strncmp (argv[a], "--interactive", 14))
//            overwrite = 1;
//        else if (!strcmp (argv[a], "-n") || !strncmp (argv[a], "--no-clobber", 12))
//            overwrite = 2;
//        else if (!strcmp (argv[a], "-s") || !strncmp (argv[a], "--strip-trailing-slashes", 24))
//            s = 1;
//        else if (!strcmp (argv[a], "-S") || !strncmp (argv[a], "--suffix", 8)) {
//            S = 1;
//
//            int isShort = isShortOption (argv[a]);
//
//            char opt[128];
//            strcpy (opt, getOption (argv[a]));
//
//            if (isShort && a == argc-1)
//                return showError (opt);
//
//            char arg[128];
//            if (isShort) strcpy (arg, argv[a]);
//            else strcpy (arg, getLongArgument (argv[a]));
//
//            getSuffix (arg, &S);
//
//            if (S == -2)
//                return showError (opt);
//
//            if (isShort)
//                a++;
//        }
//        else if (!strcmp (argv[a], "-t") || !strncmp (argv[a], "--target-directory", 18)) {
//            t = 1;
//
//            int isShort = isShortOption (argv[a]);
//
//            char opt[128];
//            strcpy (opt, getOption (argv[a]));
//
//            if (isShort && a == argc-1)
//                return showError (opt);
//
//            char arg[128];
//            if (isShort) strcpy (arg, argv[a]);
//            else strcpy (arg, getLongArgument (argv[a]));
//
//            strcpy (tgt, arg);
//
//            if (isShort)
//                a++;
//        }
//        else if (!strcmp (argv[a], "-T") || !strncmp (argv[a], "--no-target-directory", 21))
//            T = 1;
//        else if (!strcmp (argv[a], "-v") || !strncmp (argv[a], "--verbose", 9))
//            v = 1;
//        else if (!strcmp (argv[a], "-Z") || !strncmp (argv[a], "--context", 9))
//            Z = 1;
//        else if (!strcmp (argv[a], "*")) {
//
//
//        }
//        else
//            strcpy (src[srcSize++], argv[a]);
//    }
//
//    char *lastSrc = src[srcSize-1];
//
//    if (srcSize == 0)
//        return showError ("missing file operand");
//
//    if (srcSize == 1)
//        return showError ("missing destination operand");
//
//    if (T && isDir (lastSrc))
//        return showError ("no source but last src is directory");
//
//    if (!t) {
//        if (!isDir (lastSrc))
//            return showError ("last src not a directory");
//        strcpy (tgt, lastSrc);
//        srcSize--;
//    }
//
//    for (int a = 0; a < srcSize; a++) {
////        if (s)
////            removeTrailingSlash (src[a]);
////
//        char target[1024];
//        strcpy (target, tgt);
//
//        if (isDir (target)) {
//            if (S == 0)
//                appendFileName (target, getFileName (src[a]));
//        }
////
////        if (isExists (target)) {
//
//        if (S)
////            if (S == -1) {
////                if (overwrite == 2)
////                    continue;
////                else if (overwrite == 1) {
////                    printf ("Overwrite %s? (Y/n): ", target);
////                    char choice;
////                    scanf ("%c", &choice);
////                    if (choice != 'Y' && choice != 'y)
//        //                continue;
////                }
////            }
////        }
////        else
//
//    }
    
    return 0;
}
