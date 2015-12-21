/**
 * Handles generation of list of sequential prime numbers
 *
 * This is done through a bit-based sieve. This sieve start with all bits set to
 * 0 (which means its a prime number). Then, for every 0 bit found, all bits in
 * positions multiple of the current one are set to 1. As soon as this operation
 * is finished, the sieve is searched for the next 0 bit and the process is
 * repeated.
 *
 * @file src/prime.c
 */
#include <fraction_internal/prime.h>

#include <stdlib.h>
#include <string.h>

/**
 * Create a list of primes that goes up to the prime closest to maxNumberChecked
 * rounded up to the nearest 8 multiple
 *
 * @param  [out]ppList           The list of sequencial primes
 * @param  [out]pLen             How many numbers there are in the list
 * @param  [ in]maxNumberChecked Biggest number to be checked for primality
 * @return                       0 on success, 1 on failure
 */
int prime_genPrimeList(int **ppList, int *pLen, int maxNumberChecked) {
    /** Map of prime numbers */
    unsigned char *pBitSieve;
    int i, len, *pList, sieveLen;

    pBitSieve = 0;
    pList = 0;
    len = 0;

    /* Get the number of bytes required for the sieve */
    sieveLen = maxNumberChecked >> 4;
    if (sieveLen == 0 || maxNumberChecked % 8 != 0) {
        sieveLen++;
    }
    /* Alloc and initialize the sieve */
    pBitSieve = (unsigned char*)malloc(sieveLen);
    if (!pBitSieve) {
        return 1;
    }
    memset(pBitSieve, 0x0, sieveLen);

    /* Search for all prime bits so their multiples may be marked */
    i = 0;
    while (i >> 3 < sieveLen) {
        /* If a 0 bit was found, its a new prime number */
        if (!(pBitSieve[i >> 3] & (1 << (i & 7)))) {
            int j;

            /* Increase the number of primes found */
            len++;
            /* Get the index of the next odd multiple of 'i' */
            j = i + (i << 1) + 3;

            /* Mark all odd multiple of 'i' */
            while (j < maxNumberChecked) {
                pBitSieve[j >> 3] |= 1 << (j & 7);
                j += (i << 1) + 3;
            }
        }
        /* Go to the next number */
        i++;
    }

    /* Alloc the return buffer */
    pList = (int*)malloc(sizeof(int) * (len + 1));
    if (!pList) {
        free(pBitSieve);
        return 1;
    }

    pList[0] = 2;
    /* Loop through the sieve and store all prime numbers */
    i = 0;
    len = 1;
    while (i >> 3 < sieveLen) {
        /* If a 0 bit was found, its a prime number */
        if (!(pBitSieve[i >> 3] & (1 << (i & 7)))) {
            pList[len] = (i << 1) + 3;
            len++;
        }
        /* Go to the next number */
        i++;
    }

    /* Set the return parameters */
    *pLen = len;
    *ppList = pList;
    /* Clear the local memory */
    pList = 0;
    free(pBitSieve);

    return 0;
}

