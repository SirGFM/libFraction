/**
 * Defines fractional numbers and functions to operate over them. A fractional
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
#ifndef __FRACTION_STRUCT__
#define __FRACTION_STRUCT__

/** Manager that stores all fraction number references and primes */
typedef struct stFractionManager fractionManager;
/** A fraction number */
typedef struct stFraction fraction;

#endif /* __FRACTION_STRUCT__ */

#ifndef __FRACTION_H__
#define __FRACTION_H__

/**
 * Initialize the fraction manager
 *
 * @param  [out]ppOut           The alloc'ed and initialized fraction manager
 * @param  [ in]maxPrimeChecked Biggest number to be checked for primality
 * @return                      0 on success, 1 on failure
 */
int fractionManager_init(fractionManager **ppOut, int maxPrimeChecked);

/**
 * Release all alloc'ed resources for the fraction manager
 *
 * NOTE: This functions also deallocs all fractions retrieved from this fraction
 *       manager, so those numbers musn't be used anymore
 *
 * @param  [ in]ppMng The manager to be dealloc'ed
 */
void fractionManager_clean(fractionManager **ppMng);

int fractionManager_igetFraction(fraction **ppOut, fractionManager *pMng,
        int val);
int fractionManager_fxGetFraction(fraction **ppOut, fractionManager *pMng,
        int val, int decimalDigits);
int fractionManager_fgetFraction(fraction **ppOut, fractionManager *pMng,
        float val);
int fractionManager_dgetFraction(fraction **ppOut, fractionManager *pMng,
        double val);

/**
 * Releases a fraction to the fraction manager. This enables the manager to
 * recycle fractions that have already been allocated but aren't in use
 *
 */
int fractionManager_releaseFraction(fractionManager *pMng, fraction *pFrac);
int fractionManager_clone(fraction **ppOut, fractionManager *pMng,
        fraction *pSrc);

void fraction_sum(fraction *pOut, fraction *pA, fraction *pB);
void fraction_sub(fraction *pOut, fraction *pA, fraction *pB);
void fraction_div(fraction *pOut, fraction *pA, fraction *pB);
void fraction_mul(fraction *pOut, fraction *pA, fraction *pB);

void fraction_iconvert(int *pOut, fraction *pFrac);
void fraction_fxconvert(int *pOut, fraction *pFrac, int decimalDigits);
void fraction_fconvert(float *pOut, fraction *pFrac);
void fraction_dconvert(double *pOut, fraction *pFrac);

#endif /* __FRACTION_H__ */

