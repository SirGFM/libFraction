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
 *
 * @param  [out]ppList           The list of sequencial primes
 * @param  [out]pLen             How many numbers there are in the list
 * @param  [ in]maxNumberChecked Biggest number to be checked for primality
 * @return                       0 on success, 1 on failure
 */
int prime_genPrimeList(int **ppList, int *pLen, int maxNumberChecked);

