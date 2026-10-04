#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include "search.h"

int main(int argc, char *argv[])
{
    unsigned int seed = (unsigned int)time(NULL);
    /* 선택적으로 시드를 지정하면 같은 환경에서 결과를 재현할 수 있다. */
    if (argc == 2) {
        char *end;
        errno = 0;
        unsigned long value = strtoul(argv[1], &end, 10);
        if (errno != 0 || end == argv[1] || *end != '\0' ||
            argv[1][0] == '-' || value > UINT_MAX) {
            fprintf(stderr, "Invalid seed.\n");
            return EXIT_FAILURE;
        }
        seed = (unsigned int)value;
    } else if (argc > 2) {
        fprintf(stderr, "Usage: %s [seed]\n", argv[0]);
        return EXIT_FAILURE;
    }
    srand(seed);
    printf("Seed : %u\n", seed);
    printf("Counting rule: each executed key == data or key < data counts once.\n");
    runExperiment();
    return EXIT_SUCCESS;
}
