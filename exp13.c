#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX 50

struct expr {
    char op[4], op1[16], op2[16], res[16];
    int flag;  /* 1 = instruction folded away */
} arr[MAX];

int n;

/* Check whether a string is an integer constant */
int is_const(const char *s)
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

/* Input three-address code */
void input(void)
{
    int i;

    printf("\nEnter the number of expressions: ");

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX) {
        printf("Invalid number (1 to %d allowed)\n", MAX);
        exit(1);
    }

    printf("\nEnter the input (op op1 op2 result):\n");
    printf("Use '.' as op2 for '=' operator.\n");

    for (i = 0; i < n; i++) {
        if (scanf("%3s %15s %15s %15s",
                  arr[i].op,
                  arr[i].op1,
                  arr[i].op2,
                  arr[i].res) != 4) {
            printf("Invalid input\n");
            exit(1);
        }

        arr[i].flag = 0;
    }
}

/* Propagate a constant to later instructions.
   Stop when the result variable is redefined.
*/
void change(int p, const char *val)
{
    int i;

    for (i = p + 1; i < n; i++) {

        /* Replace occurrences in operand 1 */
        if (strcmp(arr[i].op1, arr[p].res) == 0)
            strcpy(arr[i].op1, val);

        /* Replace occurrences in operand 2 */
        if (strcmp(arr[i].op, "=") != 0 &&
            strcmp(arr[i].op2, arr[p].res) == 0)
            strcpy(arr[i].op2, val);

        /* Stop when the variable is redefined */
        if (strcmp(arr[i].res, arr[p].res) == 0)
            break;
    }
}

/* Constant folding and constant propagation */
void constant(void)
{
    int i, a, b, r, ok;
    char val[16];

    for (i = 0; i < n; i++) {
        char op = arr[i].op[0];
        ok = 0;

        /* Handle assignment */
        if (strcmp(arr[i].op, "=") == 0) {

            if (is_const(arr[i].op1)) {
                r = atoi(arr[i].op1);
                ok = 1;
            }

        } else if (strlen(arr[i].op) == 1 &&
                   is_const(arr[i].op1) &&
                   is_const(arr[i].op2)) {

            a = atoi(arr[i].op1);
            b = atoi(arr[i].op2);

            switch (op) {
                case '+':
                    r = a + b;
                    ok = 1;
                    break;

                case '-':
                    r = a - b;
                    ok = 1;
                    break;

                case '*':
                    r = a * b;
                    ok = 1;
                    break;

                case '/':
                    if (b != 0) {
                        r = a / b;
                        ok = 1;
                    }
                    break;

                default:
                    ok = 0;
            }
        }

        /* Replace the expression with its constant value */
        if (ok) {
            snprintf(val, sizeof(val), "%d", r);

            arr[i].flag = 1;
            change(i, val);
        }
    }
}

/* Display optimized code */
void output(void)
{
    int i;

    printf("\nOptimized code is:\n");

    for (i = 0; i < n; i++) {
        if (!arr[i].flag) {
            printf("%s %s %s %s\n",
                   arr[i].op,
                   arr[i].op1,
                   arr[i].op2,
                   arr[i].res);
        }
    }
}

int main(void)
{
    input();
    constant();
    output();

    return 0;
}
