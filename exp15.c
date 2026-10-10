#include <stdio.h>

#define MAX 10
#define MAXD 1024

int n, m;
int nfa[MAX][MAX];
int dst[MAXD], dt[MAXD][MAX];
int nd = 1;
char sym[MAX];

int main(void)
{
    int i, j, k, t;
    int from, to, nt, s, nf;
    int fin = 0, mask;
    char c;

    printf("Number of states and symbols: ");
    scanf("%d %d", &n, &m);

    if (n < 1 || n > MAX || m < 1 || m > MAX) {
        printf("Invalid number of states or symbols.\n");
        return 1;
    }

    printf("Symbols: ");
    for (k = 0; k < m; k++)
        scanf(" %c", &sym[k]);

    printf("Number of transitions: ");
    scanf("%d", &nt);

    printf("Enter transitions (from symbol to):\n");

    for (i = 0; i < nt; i++) {
        scanf("%d %c %d", &from, &c, &to);

        if (from < 0 || from >= n ||
            to < 0 || to >= n) {
            printf("Invalid state in transition.\n");
            return 1;
        }

        for (k = 0; k < m; k++) {
            if (sym[k] == c) {
                nfa[from][k] |= (1 << to);
                break;
            }
        }

        if (k == m) {
            printf("Invalid input symbol: %c\n", c);
            return 1;
        }
    }

    printf("Start state, number of final states, final states: ");
    scanf("%d %d", &s, &nf);

    if (s < 0 || s >= n || nf < 0 || nf > n) {
        printf("Invalid start state or number of final states.\n");
        return 1;
    }

    for (i = 0; i < nf; i++) {
        scanf("%d", &t);

        if (t < 0 || t >= n) {
            printf("Invalid final state.\n");
            return 1;
        }

        fin |= (1 << t);
    }

    /* Initial DFA state */
    dst[0] = 1 << s;

    /* Subset construction */
    for (i = 0; i < nd; i++) {
        for (k = 0; k < m; k++) {
            mask = 0;

            /* Find all NFA states reachable on symbol k */
            for (j = 0; j < n; j++) {
                if (dst[i] & (1 << j))
                    mask |= nfa[j][k];
            }

            /* Find or create the corresponding DFA state */
            for (t = 0; t < nd && dst[t] != mask; t++)
                ;

            if (t == nd && mask != 0) {
                if (nd >= MAXD) {
                    printf("Too many DFA states.\n");
                    return 1;
                }

                dst[nd++] = mask;
            }

            dt[i][k] = (mask == 0) ? -1 : t;
        }
    }

    /* Print the DFA transition table */
    printf("\nDFA Transition Table:\n");
    printf("State\tNFA States\t");

    for (k = 0; k < m; k++)
        printf("%c\t", sym[k]);

    printf("\n");

    for (i = 0; i < nd; i++) {
        printf("%c%s\t{", 'A' + i,
               (dst[i] & fin) ? "*" : "");

        for (j = 0; j < n; j++) {
            if (dst[i] & (1 << j))
                printf("q%d ", j);
        }

        printf("}\t\t");

        for (k = 0; k < m; k++) {
            if (dt[i][k] == -1)
                printf("-\t");
            else
                printf("%c\t", 'A' + dt[i][k]);
        }

        printf("\n");
    }

    printf("\nA = start state, * = final state, - = no transition\n");

    return 0;
}
