/**
 * Simple test to check whether subtraction works
 *
 * @file tst/frac_sub.c
 */
#include <fraction/fraction.h>

#include <assert.h>
#include <stdlib.h>
#include <time.h>

static fractionManager *pFMng = 0;

void do_clean() {
    fractionManager_clean(&pFMng);
}

int main(int argc, char *argv[]) {
    int irv, num;

    num = 500;
    if (argc == 2) {
        char *pTmp;

        num = 0;
        pTmp = argv[1];
        while (*pTmp) {
            num = num * 10 + (*pTmp) - '0';
            pTmp++;
        }
    }

    /* Register a function to clear the manager, even on assert failure */
    atexit(do_clean);

    irv = fractionManager_init(&pFMng, 1000000/*maxNumberChecked*/);
    assert(irv == 0);

    srand(time(0));

    while (num > 0) {
        fraction *pA, *pB;
        int a, b, val;

        a = rand() / 10;
        b = rand() / 10;

        irv = fractionManager_igetFraction(&pA, pFMng, a);
        assert(irv == 0);
        irv = fractionManager_igetFraction(&pB, pFMng, b);
        assert(irv == 0);
        fraction_sub(pA, pA, pB);
        assert(irv == 0);
        fraction_iconvert(&val, pA);
        assert(val == a - b);

        fractionManager_releaseFraction(pA);
        fractionManager_releaseFraction(pB);

        num--;
    }

    return 0;
}

