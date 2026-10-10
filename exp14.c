#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAXQ 50
#define MAXV 100
#define LEN 32

char icode[MAXQ][LEN];
char vars[MAXV][LEN];
int nq = 0, nv = 0;

/* Check whether a string is an integer constant */
int is_number(const char *s)
{
    int i = 0;

    if (s[0] == '-' && s[1] != '\0')
        i = 1;

    if (s[i] == '\0')
        return 0;

    for (; s[i]; i++) {
        if (!isdigit((unsigned char)s[i]))
            return 0;
    }

    return 1;
}

/* Case-insensitive string comparison */
int ieq(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a) !=
            tolower((unsigned char)*b))
            return 0;

        a++;
        b++;
    }

    return *a == *b;
}

/* Add a variable if it does not already exist */
void add_var(const char *s)
{
    int i;

    if (is_number(s))
        return;

    for (i = 0; i < nv; i++) {
        if (ieq(vars[i], s))
            return;
    }

    if (nv < MAXV)
        strcpy(vars[nv++], s);
}

/* Copy n characters and terminate the string */
int copy_n(char *dst, const char *src, int n)
{
    if (n <= 0 || n >= LEN)
        return 0;

    strncpy(dst, src, n);
    dst[n] = '\0';

    return 1;
}
int parse(const char *line, char *res,
          char *a, char *op, char *b)
{
    const char *eq = strchr(line, '=');
    const char *rhs;
    const char *p;

    if (eq == NULL)
        return 0;

    if (!copy_n(res, line, (int)(eq - line)))
        return 0;

    rhs = eq + 1;

    if (*rhs == '\0')
        return 0;

    /* Find a binary operator */
    for (p = rhs + 1; *p; p++) {
        if (strchr("+-*/", *p))
            break;
    }

    if (*p != '\0') {
        *op = *p;

        return copy_n(a, rhs, (int)(p - rhs)) &&
               copy_n(b, p + 1, (int)strlen(p + 1));
    }

    /* Unary minus */
    if (rhs[0] == '-' && !is_number(rhs)) {
        *op = 'n';

        return copy_n(a, rhs + 1,
                      (int)strlen(rhs + 1));
    }

    /* Simple assignment */
    *op = 0;

    return copy_n(a, rhs, (int)strlen(rhs));
}

int main(void)
{
    int i;
    char res[LEN], a[LEN], b[LEN], op;

    printf("Enter the intermediate code (type exit to stop):\n");

    while (nq < MAXQ &&
           scanf("%31s", icode[nq]) == 1) {

        if (strcmp(icode[nq], "exit") == 0)
            break;

        nq++;
    }

    /* Pass 1: Collect variables */
    for (i = 0; i < nq; i++) {
        b[0] = '\0';

        if (!parse(icode[i], res, a, &op, b)) {
            printf("Invalid intermediate code: %s\n",
                   icode[i]);
            return 1;
        }

        add_var(res);
        add_var(a);

        if (op != 0 && op != 'n')
            add_var(b);
    }

    /* Generate assembly code */
    printf("\n8086 Target Code Generation\n");
    printf("---------------------------\n");
    printf(".MODEL SMALL\n");
    printf(".STACK 100H\n");
    printf(".DATA\n");

    for (i = 0; i < nv; i++)
        printf("%s DW 0\n", vars[i]);

    printf(".CODE\n");
    printf("MAIN PROC\n");
    printf("    MOV AX, @DATA\n");
    printf("    MOV DS, AX\n");

    /* Pass 2: Generate target instructions */
    for (i = 0; i < nq; i++) {
        parse(icode[i], res, a, &op, b);

        printf("\n    ; %s\n", icode[i]);

        switch (op) {
            case 0:
                printf("    MOV AX, %s\n", a);
                printf("    MOV %s, AX\n", res);
                break;

            case 'n':
                printf("    MOV AX, %s\n", a);
                printf("    NEG AX\n");
                printf("    MOV %s, AX\n", res);
                break;

            case '+':
                printf("    MOV AX, %s\n", a);
                printf("    ADD AX, %s\n", b);
                printf("    MOV %s, AX\n", res);
                break;

            case '-':
                printf("    MOV AX, %s\n", a);
                printf("    SUB AX, %s\n", b);
                printf("    MOV %s, AX\n", res);
                break;

            case '*':
                printf("    MOV AX, %s\n", a);
                printf("    MOV BX, %s\n", b);
                printf("    IMUL BX\n");
                printf("    MOV %s, AX\n", res);
                break;

            case '/':
                printf("    MOV AX, %s\n", a);
                printf("    CWD\n");
                printf("    MOV BX, %s\n", b);
                printf("    IDIV BX\n");
                printf("    MOV %s, AX\n", res);
                break;

            default:
                printf("    ; Unsupported operator\n");
                break;
        }
    }

    printf("\n    MOV AH, 4CH\n");
    printf("    INT 21H\n");
    printf("MAIN ENDP\n");
    printf("END MAIN\n");

    return 0;
}
