/**
 * Defines fractional numbers and functions to operate on them. A fractional
 * number is composed by a numerator and a denominator, and both are represented
 * as integer.
 *
 * The current implementation isn't really optimized for speed. After every
 * operation, the resulted number is simplified, which requires a list of
 * primes. Perhaps, keeping both numerator and denominator as lists of primes
 * would be more optimized for speed. However, a list of primes is required for
 * the current implementation.
 *
 * The lib has an "unexported" module for generating lists of primes. It's
 * initialized with the main context, and its precision (i.e., maximum
 * calculated prime) may be set.
 *
 * If, at some point, the need to use these numbers in a more conventional way
 * arise, they can be exported to intege (dicarding the decimal part), to
 * decimal fixed point, to float-point numbers and to doubles.
 */
#include <fraction/fraction.h>
#include <fraction_internal/prime.h>

#include <stdlib.h>
#include <string.h>

/** Buffer of fractions, from which new references are recycled */
struct stFractionBuffer {
    /** All fractions alloc'ed on this buffer */
    fraction *pFractions;
    /** Number of alloc'ed objects */
    int numFractions;
    /** Number of used objects */
    int usedFractions;
};
typedef struct stFractionBuffer fractionBuffer;

/** Keep references to all fraction lists and the list of primes */
struct stFractionManager {
    /** Store all fraction buffers */
    fractionBuffer **ppFractionBuffers;
    /** Number of buffers */
    int numFractionBuffers;
    /** List of sequential prime numbers */
    int *pPrimes;
    /** Number of primes in the list */
    int numPrimes;
    /** Linked list of released fractions */
    fraction *pFreeFractions;
};

/** Fractional number */
struct stFraction {
    /** The fraction's numerator */
    int numerator;
    /** The fraction's denominator */
    int denominator;
    /** Next released fraction, if this is on pFreeFractions LL */
    fraction *pNext;
    /** Reference to the manager that alloc'ed this object */
    fractionManager *pManager;
};

/**
 * Initializes the fraction manager
 *
 * @param  [out]ppOut            The alloc'ed and initialized fraction manager
 * @param  [ in]maxNumberChecked Biggest number to be checked for primality
 * @return                       0 on success, 1 on failure
 */
int fractionManager_init(fractionManager **ppOut, int maxNumberChecked) {
    fractionBuffer *pCurBuffer;
    fractionManager *pMng;
    int irv;

#define INIT_ASSERT(val) \
  do { \
    if (!(val)) { \
      fractionManager_clean(&pMng); \
      return 1; \
    } \
  } while (0)

    /* Alloc the main manager */
    pMng = (fractionManager*)malloc(sizeof(fractionManager));
    INIT_ASSERT(pMng);
    memset(pMng, 0x0, sizeof(fractionManager));

    /* Create the list of primes */
    irv = prime_genPrimeList(&(pMng->pPrimes), &(pMng->numPrimes),
            maxNumberChecked);
    INIT_ASSERT(irv == 0);

    /* Alloc a list of fraction buffer */
    pMng->numFractionBuffers = 1;
    pMng->ppFractionBuffers = (fractionBuffer**)malloc(sizeof(fractionBuffer*));
    INIT_ASSERT(pMng->ppFractionBuffers);
    pMng->ppFractionBuffers[0] = 0;

    /* "Pre-alloc" the first buffer of fractions */
    pCurBuffer = (fractionBuffer*)malloc(sizeof(fractionBuffer));
    INIT_ASSERT(pCurBuffer);
    memset(pCurBuffer, 0x0, sizeof(fractionBuffer));
    /* Store it in the list */
    pMng->ppFractionBuffers[0] = pCurBuffer;

    pCurBuffer->numFractions = 512;
    pCurBuffer->pFractions = (fraction*)malloc(sizeof(fraction) *
            pCurBuffer->numFractions);
    INIT_ASSERT(pCurBuffer->pFractions);

#undef INIT_ASSERT

    /* Return the alloc'ed object */
    *ppOut = pMng;
    pMng = 0;
    return 0;
}

/**
 * Releases all alloc'ed resources for the fraction manager
 *
 * NOTE: This functions also deallocs all fractions retrieved from this fraction
 *       manager, so those numbers musn't be used anymore
 *
 * @param  [ in]ppMng The manager to be dealloc'ed
 */
void fractionManager_clean(fractionManager **ppMng) {
    fractionManager *pMng;

    /* Check that the object was initialized */
    if (!ppMng || !(*ppMng)) {
        return;
    }
    pMng = *ppMng;

    /* Clear all fraction buffers */
    if (pMng->ppFractionBuffers) {
        int i;

        i = 0;
        while (i < pMng->numFractionBuffers) {
            if (pMng->ppFractionBuffers[i]) {
                if (pMng->ppFractionBuffers[i]->pFractions) {
                    free(pMng->ppFractionBuffers[i]->pFractions);
                }
                free(pMng->ppFractionBuffers[i]);
            }
            i++;
        }

        free(pMng->ppFractionBuffers);
    }

    /* Clear the list of primes */
    if (pMng->pPrimes) {
        free(pMng->pPrimes);
    }

    /* Clear the manager itself */
    free(pMng);
    *ppMng = 0;
}

/**
 * Alloc/retrieve a new fraction
 *
 * @param  [out]ppOut The alloc'ed fraction
 * @param  [ in]pMng  The fraction manager (so all references are kept)
 * @return            0 on success, 1 on failure
 */
static int fractionManager_getNewFraction(fraction **ppOut,
        fractionManager *pMng) {
    fractionBuffer *pCurBuffer;

    /* Try to recycle a reference */
    if (pMng->pFreeFractions) {
        *ppOut = pMng->pFreeFractions;
        pMng->pFreeFractions = pMng->pFreeFractions->pNext;

        return 0;
    }

    pCurBuffer = pMng->ppFractionBuffers[pMng->numFractionBuffers - 1];
    /* Try to retrieve an already alloc'ed fraction */
    if (pCurBuffer->usedFractions < pCurBuffer->numFractions) {
        *ppOut = &(pCurBuffer->pFractions[pCurBuffer->usedFractions]);
        pCurBuffer->usedFractions++;

        return 0;
    }

    /* Otherwise, expand the buffer */
    pMng->numFractionBuffers++;
    pMng->ppFractionBuffers = (fractionBuffer**)realloc(pMng->ppFractionBuffers,
            sizeof(fractionBuffer*) * pMng->numFractionBuffers);
    if (!pMng->ppFractionBuffers) {
        return 1;
    }
    pMng->ppFractionBuffers[pMng->numFractionBuffers - 1] = 0;

    /* Alloc a new buffer and store it in the list */
    pCurBuffer = (fractionBuffer*)malloc(sizeof(fractionBuffer));
    if (!pCurBuffer) {
        return 1;
    }
    memset(pCurBuffer, 0x0, sizeof(fractionBuffer));
    pMng->ppFractionBuffers[pMng->numFractionBuffers - 1] = pCurBuffer;

    pCurBuffer->numFractions = 512;
    pCurBuffer->pFractions = (fraction*)malloc(sizeof(fraction) *
            pCurBuffer->numFractions);
    if (!pCurBuffer->pFractions) {
        return 1;
    }

    /* Retrieve a reference from this new buffer */
    *ppOut = &(pCurBuffer->pFractions[0]);
    pCurBuffer->usedFractions++;
    return 0;
}

/**
 * Simplify a fraction
 *
 * @param  [ in]pFrac The fraction
 */
static void fraction_simplify(fraction *pFrac) {
    fractionManager *pMng;
    int i;

    pMng = pFrac->pManager;

    /* Check all feasible prime */
    i = 0;
    /* TODO OPTIMIZATION: run loop while pPrimes[i] < sqrt(num) */
    while (i < pMng->numPrimes && pMng->pPrimes[i] < pFrac->numerator && 
            pMng->pPrimes[i] < pFrac->denominator) {
        /* Divide both numerator and denominator if the prime is common to
         * both */
        while (pFrac->numerator % pMng->pPrimes[i] == 0 &&
                pFrac->denominator % pMng->pPrimes[i] == 0) {
            pFrac->numerator /= pMng->pPrimes[i];
            pFrac->denominator /= pMng->pPrimes[i];
        }
        i++;
    }
}

/**
 * Initializes a fraction from an integer number
 *
 * @param  [out]ppOut The alloc'ed/initialized fraction
 * @param  [ in]pMng  The fraction manager (so all references are kept)
 * @param  [ in]val   The fraction initial value
 * @return            0 on success, 1 on failure
 */
int fractionManager_igetFraction(fraction **ppOut, fractionManager *pMng,
        int val) {
    int irv;

    /* Retrieve a unused referece */
    irv = fractionManager_getNewFraction(ppOut, pMng);
    if (irv != 0) {
        return 1;
    }

    /* Initialize it */
    (*ppOut)->numerator = val;
    (*ppOut)->denominator = 1;
    (*ppOut)->pNext = 0;
    (*ppOut)->pManager = pMng;
    fraction_simplify(*ppOut);

    return 0;
}

/**
 * Initializes a fraction from a decimal fixed point number
 *
 * @param  [out]ppOut         The alloc'ed/initialized fraction
 * @param  [ in]pMng          The fraction manager (so all references are kept)
 * @param  [ in]val           The fraction initial value
 * @param  [ in]decimalDigits Number of digits in the value that represents the
 *                            decimal part
 * @return                    0 on success, 1 on failure
 */
int fractionManager_fxGetFraction(fraction **ppOut, fractionManager *pMng,
        int val, int decimalDigits) {
    int divisor, irv;

    /* Retrieve a unused referece */
    irv = fractionManager_getNewFraction(ppOut, pMng);
    if (irv != 0) {
        return 1;
    }

    divisor = 1;
    while (decimalDigits > 0) {
        divisor *= 10;
        decimalDigits--;
    }

    /* Initialize it */
    (*ppOut)->numerator = val;
    (*ppOut)->denominator = divisor;
    (*ppOut)->pNext = 0;
    (*ppOut)->pManager = pMng;
    fraction_simplify(*ppOut);

    return 0;
}

/**
 * Initializes a fraction from a float number
 *
 * @param  [out]ppOut The alloc'ed/initialized fraction
 * @param  [ in]pMng  The fraction manager (so all references are kept)
 * @param  [ in]val   The fraction initial value
 * @return            0 on success, 1 on failure
 */
int fractionManager_fgetFraction(fraction **ppOut, fractionManager *pMng,
        float val) {
    int irv;

    /* Retrieve a unused referece */
    irv = fractionManager_getNewFraction(ppOut, pMng);
    if (irv != 0) {
        return 1;
    }

    /* Initialize it */
    (*ppOut)->numerator = val * 10000;
    (*ppOut)->denominator = 10000;
    (*ppOut)->pNext = 0;
    (*ppOut)->pManager = pMng;
    fraction_simplify(*ppOut);

    return 0;
}

/**
 * Initializes a fraction from a double number
 *
 * @param  [out]ppOut The alloc'ed/initialized fraction
 * @param  [ in]pMng  The fraction manager (so all references are kept)
 * @param  [ in]val   The fraction initial value
 * @return            0 on success, 1 on failure
 */
int fractionManager_dgetFraction(fraction **ppOut, fractionManager *pMng,
        double val) {
    int irv;

    /* Retrieve a unused referece */
    irv = fractionManager_getNewFraction(ppOut, pMng);
    if (irv != 0) {
        return 1;
    }

    /* Initialize it */
    (*ppOut)->numerator = val * 10000;
    (*ppOut)->denominator = 10000;
    (*ppOut)->pNext = 0;
    (*ppOut)->pManager = pMng;
    fraction_simplify(*ppOut);

    return 0;
}

/**
 * Releases a fraction to the fraction manager. This enables the manager to
 * recycle fractions that have already been allocated but aren't in use
 *
 * @param  [ in]pFrac The number to be released
 */
void fractionManager_releaseFraction(fraction *pFrac) {
    /* Append it to the list of freed fractions */
    pFrac->pNext = pFrac->pManager->pFreeFractions;
    pFrac->pManager->pFreeFractions = pFrac;
}

/**
 * Clones a fraction number into a newly alloc'ed one
 *
 * @param  [out]ppOut The cloned fraction
 * @param  [ in]pSrc  The orignal number
 * @return            0 on success, 1 on failure
 */
int fractionManager_clone(fraction **ppOut, fraction *pSrc) {
    int irv;

    /* Retrieve a unused referece */
    irv = fractionManager_getNewFraction(ppOut, pSrc->pManager);
    if (irv != 0) {
        return 1;
    }

    /* Initialize it */
    (*ppOut)->numerator = pSrc->numerator;
    (*ppOut)->denominator = pSrc->denominator;
    (*ppOut)->pNext = 0;
    (*ppOut)->pManager = pSrc->pManager;

    return 0;
}

/**
 * Find the least common denominator to both fractions, and set it as the base
 * for both fractions
 *
 * @param  [ in]pA A fraction
 * @param  [ in]pB The other fraction
 */
static void fraction_setLCD(fraction *pA, fraction *pB) {
    fractionManager *pMng;
    int denA, denB, i;

    pMng = pA->pManager;

    /* Retrieve both numbers denominators */
    denA = pA->denominator;
    denB = pB->denominator;

    /* Check which prime is missing in each denominator and multiply the other
     * fraction */
    i = 0;
    while (i < pMng->numPrimes && (denA != 1 || denB != 1)) {
        if (pMng->pPrimes[i] % denA == 0 && pMng->pPrimes[i] % denB == 0) {
            /* If both numbers are divisible by this prime, do nothing */
            denA /= pMng->pPrimes[i];
            denB /= pMng->pPrimes[i];
        }
        else if (pMng->pPrimes[i] % denA == 0 && pMng->pPrimes[i] % denB != 0) {
            /* Only A is divisible*/
            
            /* Remove it from A's denominator */
            denA /= pMng->pPrimes[i];
            /* Multiply B by this prime */
            pB->numerator *= pMng->pPrimes[i];
            pB->denominator *= pMng->pPrimes[i];
        }
        else if (pMng->pPrimes[i] % denA != 0 && pMng->pPrimes[i] % denB == 0) {
            /* Only B is divisible*/
            
            /* Remove it from B's denominator */
            denB /= pMng->pPrimes[i];
            /* Multiply A by this prime */
            pA->numerator *= pMng->pPrimes[i];
            pA->denominator *= pMng->pPrimes[i];
        }
        else if (pMng->pPrimes[i] % denA != 0 && pMng->pPrimes[i] % denB != 0) {
            /* Done with this prime, move to the next one */
            i++;
        }
    }
}

/**
 * Adds two fractional numbers
 *
 * NOTE: The output may be one of the inputs!
 *
 * @param  [out]pOut The operation's result
 * @param  [ in]pA   One of the summands
 * @param  [ in]pB   The other summand
 */
void fraction_sum(fraction *pOut, fraction *pA, fraction *pB) {
    fraction_setLCD(pA, pB);

    /* Set the result's denominator */
    pOut->denominator = pA->denominator;
    /* Add the numerator */
    pOut->numerator = pA->numerator + pB->numerator;

    fraction_simplify(pA);
    fraction_simplify(pB);
    fraction_simplify(pOut);
}

/**
 * Subtracts two fractional numbers
 *
 * NOTE: The output may be one of the inputs!
 *
 * @param  [out]pOut The operation's result
 * @param  [ in]pA   The minuend
 * @param  [ in]pB   The subtrahend
 */
void fraction_sub(fraction *pOut, fraction *pA, fraction *pB) {
    fraction_setLCD(pA, pB);

    /* Set the result's denominator */
    pOut->denominator = pA->denominator;
    /* Add the numerator */
    pOut->numerator = pA->numerator - pB->numerator;

    fraction_simplify(pA);
    fraction_simplify(pB);
    fraction_simplify(pOut);
}

/**
 * Multiplies two fractional numbers
 *
 * NOTE: The output may be one of the inputs!
 *
 * @param  [out]pOut The operation's result
 * @param  [ in]pA   One of the factors
 * @param  [ in]pB   The other factors
 */
void fraction_mul(fraction *pOut, fraction *pA, fraction *pB) {
    pOut->numerator = pA->numerator * pB->numerator;
    pOut->denominator = pA->denominator * pB->denominator;

    fraction_simplify(pOut);
}

/**
 * Divides two fractional numbers
 *
 * NOTE: The output mustn't be pB!
 *
 * @param  [out]pOut The operation's result
 * @param  [ in]pA   The minuend
 * @param  [ in]pB   The subtrahend
 */
void fraction_div(fraction *pOut, fraction *pA, fraction *pB) {
    pOut->numerator = pA->numerator * pB->denominator;
    pOut->denominator = pA->denominator * pB->numerator;

    fraction_simplify(pOut);
}

/**
 * Converts a fractional number to an integer, retrieving only its quotient
 *
 * @param  [out]pOut  The converted fraction
 * @param  [ in]pFrac The fraction
 */
void fraction_iconvert(int *pOut, fraction *pFrac) {
    *pOut = pFrac->numerator / pFrac->denominator;
}

/**
 * Converts a fractional number to a decimal fixed point
 *
 * @param  [out]pOut          The converted fraction
 * @param  [ in]pFrac         The fraction
 * @param  [ in]decimalDigits Number of digits in the value that represents the
 *                            decimal part
 */
void fraction_fxconvert(int *pOut, fraction *pFrac, int decimalDigits) {
    int multiplier;

    multiplier = 1;
    while (decimalDigits > 0) {
        multiplier *= 10;
        decimalDigits--;
    }

    *pOut = pFrac->numerator * multiplier / pFrac->denominator;
}

/**
 * Converts a fractional number to a float
 *
 * @param  [out]pOut  The converted fraction
 * @param  [ in]pFrac The fraction
 */
void fraction_fconvert(float *pOut, fraction *pFrac) {
    *pOut = pFrac->numerator / (float)pFrac->denominator;
}

/**
 * Converts a fractional number to a double
 *
 * @param  [out]pOut  The converted fraction
 * @param  [ in]pFrac The fraction
 */
void fraction_dconvert(double *pOut, fraction *pFrac) {
    *pOut = pFrac->numerator / (double)pFrac->denominator;
}

/**
 * Converts a fractional number through a division, retrieving the number's
 * quotient and remainder
 *
 * @param  [out]pQuotOut The fraction's quotient
 * @param  [out]pRemOut  The fraction's remainder
 * @param  [ in]pFrac    The fraction
 */
void fraction_divConvert(int *pQuotOut, int *pRemOut, fraction *pFrac) {
    *pQuotOut = pFrac->numerator / pFrac->denominator;
    *pRemOut = pFrac->numerator % pFrac->denominator;
}

