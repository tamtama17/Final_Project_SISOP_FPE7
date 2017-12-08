#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int getch (void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr (STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr (STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar ();
    tcsetattr (STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

int tryHelp (const char *msg) {
    char m[1024], h[1024] = {"Try 'less --help' for more information."};
    strcat (strcat (strcpy (m, "less: "), msg), "\n");
    puts (strcat (m, h));
    return 0;
}

int getFileSize (char *filename) {
    struct stat st;
    if (stat (filename, &st) == 0)
        return st.st_size;
    else
        return -1;
}

void getTerminalSize (int *row, int *col) {
    struct winsize w;
    ioctl (STDOUT_FILENO, TIOCGWINSZ, &w);
    
    *row = w.ws_row;
    *col = w.ws_col;
}

void countRowColumn (char *filename, int *row, int *col) {
    int lines=0, maxColumn=0;
    int column=0;
    int ch;
    FILE *file = fopen (filename, "r");
    
    while ((ch = fgetc (file)) != EOF) {
        column++;
        if (ch == '\n') {
            lines++;
            if (column > maxColumn)
                maxColumn = column;
            column = 0;
        }
    }
    
    *row = lines;
    *col = maxColumn;
}

int main (int argc, char **argv) {
    if (argc == 1)
        return tryHelp ("Missing filename (\"less --help\" for help)");
    
    if (access (argv[1], F_OK) == -1) {
        char msg[1024];
        sprintf (msg, "%s: No such file or directory", argv[1]);
        return tryHelp (msg);
    }
    
    int row, col;
    getTerminalSize (&row, &col);
    
    int lines, maxColumn;
    countRowColumn (argv[1], &lines, &maxColumn);
    
    FILE *file = fopen (argv[1], "r");
    int sz = getFileSize (argv[1]);
    
    char content[lines][maxColumn];
    int r=0, c=0;
    int temp;
    
    while ((temp = fgetc (file)) != EOF) {
        if (temp == '\n')
            content[r][c] = '\0', r++, c=0;
        else
            content[r][c++] = (char) temp;
    }
    
    int currentRow = 0;
    row--;
    
    int input;
    
    while (1) {
        system ("clear");
        for (int i = 0; i < row; i++)
            puts (content[currentRow+i]);
    
        if (currentRow + row == lines - 1)
            printf ("(END)");
        else
            printf (":");
        input = getch ();
        printf ("\n");
        
        switch (input) {
            case 65:
                if (currentRow > 0)
                    currentRow--;
                break;
                
            case 66:
                printf ("%d %d\n", currentRow, lines);
                if (currentRow + row < lines - 1)
                    currentRow++;
                break;
                
            case 67:
                if (currentRow + row/2 < lines - row - 1)
                    currentRow += row/2;
                else
                    currentRow = lines - row - 1;
                break;
    
            case 68:
                if (currentRow > row/2)
                    currentRow -= row/2;
                else
                    currentRow = 0;
                break;
                
            case 'q':
            case 'Q':
                system ("clear");
                return 0;
            
            case 'e':
            case 'E':
            case 'j':
            case 'N':
                if (currentRow + row < lines - 1)
                    currentRow++;
                break;
    
            case 'y':
            case 'Y':
            case 'k':
            case 'K':
            case 'P':
                if (currentRow > 0)
                    currentRow--;
                break;
    
            case 'f':
            case 'F':
            case 'V':
            case 32:
                if (currentRow > row)
                    currentRow -= row;
                else
                    currentRow = 0;
                break;
    
            case 'b':
                if (currentRow + row < lines - row - 1)
                    currentRow += row;
                else
                    currentRow = lines - row - 1;
                break;
                
            default: break;
        }
    }
    
    return 0;
}
