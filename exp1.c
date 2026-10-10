#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main()
{
    FILE *f1;
    char c;
    char str[50];
    int i = 0, num, linecount = 1;

    f1 = fopen("input.txt", "r");

    if (f1 == NULL)
    {
        printf("Error opening file.\n");
        return 1;
    }

    while ((c = getc(f1)) != EOF)
    {
        /* Check for numbers */
        if (isdigit(c))
        {
            num = c - '0';
            c = getc(f1);

            while (isdigit(c))
            {
                num = num * 10 + (c - '0');
                c = getc(f1);
            }

            printf("%d is a number\n", num);
            ungetc(c, f1);
        }

        /* Check for identifiers and keywords */
        else if (isalpha(c))
        {
            i = 0;
            str[i++] = c;
            c = getc(f1);

            while (isalnum(c) || c == '_' || c == '$')
            {
                str[i++] = c;
                c = getc(f1);
            }

            str[i] = '\0';

            if (strcmp(str, "for") == 0 ||
                strcmp(str, "while") == 0 ||
                strcmp(str, "do") == 0 ||
                strcmp(str, "int") == 0 ||
                strcmp(str, "float") == 0 ||
                strcmp(str, "char") == 0 ||
                strcmp(str, "double") == 0 ||
                strcmp(str, "static") == 0 ||
                strcmp(str, "switch") == 0 ||
                strcmp(str, "case") == 0)
            {
                printf("%s is a keyword\n", str);
            }
            else
            {
                printf("%s is an identifier\n", str);
            }

            ungetc(c, f1);
        }

        /* Ignore spaces and tabs */
        else if (c == ' ' || c == '\t')
        {
            continue;
        }

        /* Count lines */
        else if (c == '\n')
        {
            linecount++;
        }

        /* Special symbols */
        else
        {
            printf("%c is a special symbol\n", c);
        }
    }

    printf("Total number of lines: %d\n", linecount);

    fclose(f1);
    return 0;
}
