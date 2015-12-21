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
 *
 * @file include/fraction/fraction.h
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
 * Initializes the fraction manager
 *
 * @param  [out]ppOut            The alloc'ed and initialized fraction manager
 * @param  [ in]maxNumberChecked Biggest number to be checked for primality
 * @return                       0 on success, 1 on failure
 */
int fractionManager_init(fractionManager **ppOut, int maxNumberChecked);

/**
 * Releases all alloc'ed resources for the fraction manager
 *
 * NOTE: This functions also deallocs all fractions retrieved from this fraction
 *       manager, so those numbers musn't be used anymore
 *
 * @param  [ in]ppMng The manager to be dealloc'ed
 */
void fractionManager_clean(fractionManager **ppMng);

/**
 * Initializes a fraction from an integer number
 *
 * @param  [out]ppOut The alloc'ed/initialized fraction
 * @param  [ in]pMng  The fraction manager (so all references are kept)
 * @param  [ in]val   The fraction initial value
 * @return            0 on success, 1 on failure
 */
int fractionManager_igetFraction(fraction **ppOut, fractionManager *pMng,
        int val);

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
        int val, int decimalDigits);

/**
 * Initializes a fraction from a float number
 *
 * @param  [out]ppOut The alloc'ed/initialized fraction
 * @param  [ in]pMng  The fraction manager (so all references are kept)
 * @param  [ in]val   The fraction initial value
 * @return            0 on success, 1 on failure
 */
int fractionManager_fgetFraction(fraction **ppOut, fractionManager *pMng,
        float val);

/**
 * Initializes a fraction from a double number
 *
 * @param  [out]ppOut The alloc'ed/initialized fraction
 * @param  [ in]pMng  The fraction manager (so all references are kept)
 * @param  [ in]val   The fraction initial value
 * @return            0 on success, 1 on failure
 */
int fractionManager_dgetFraction(fraction **ppOut, fractionManager *pMng,
        double val);

/**
 * Releases a fraction to the fraction manager. This enables the manager to
 * recycle fractions that have already been allocated but aren't in use
 *
 * @param  [ in]pFrac The number to be released
 * @return            0 on success, 1 on failure
 */
void fractionManager_releaseFraction(fraction *pFrac);

/**
 * Clones a fraction number into a newly alloc'ed one
 *
 * @param  [out]ppOut The cloned fraction
 * @param  [ in]pSrc  The orignal number
 * @return            0 on success, 1 on failure
 */
int fractionManager_clone(fraction **ppOut, fraction *pSrc);

/**
 * Adds two fractional numbers
 *
 * NOTE: The output may be one of the inputs!
 *
 * @param  [out]pOut The operation's result
 * @param  [ in]pA   One of the summands
 * @param  [ in]pB   The other summand
 */
void fraction_sum(fraction *pOut, fraction *pA, fraction *pB);

/**
 * Subtracts two fractional numbers
 *
 * NOTE: The output may be one of the inputs!
 *
 * @param  [out]pOut The operation's result
 * @param  [ in]pA   The minuend
 * @param  [ in]pB   The subtrahend
 */
void fraction_sub(fraction *pOut, fraction *pA, fraction *pB);

/**
 * Multiplies two fractional numbers
 *
 * NOTE: The output may be one of the inputs!
 *
 * @param  [out]pOut The operation's result
 * @param  [ in]pA   One of the factors
 * @param  [ in]pB   The other factors
 */
void fraction_mul(fraction *pOut, fraction *pA, fraction *pB);

/**
 * Divides two fractional numbers
 *
 * NOTE: The output may be one of the inputs!
 *
 * @param  [out]pOut The operation's result
 * @param  [ in]pA   The minuend
 * @param  [ in]pB   The subtrahend
 */
void fraction_div(fraction *pOut, fraction *pA, fraction *pB);

/**
 * Converts a fractional number to an integer, retrieving only its quotient
 *
 * @param  [out]pOut  The converted fraction
 * @param  [ in]pFrac The fraction
 */
void fraction_iconvert(int *pOut, fraction *pFrac);

/**
 * Converts a fractional number to a decimal fixed point
 *
 * @param  [out]pOut          The converted fraction
 * @param  [ in]pFrac         The fraction
 * @param  [ in]decimalDigits Number of digits in the value that represents the
 *                            decimal part
 */
void fraction_fxconvert(int *pOut, fraction *pFrac, int decimalDigits);

/**
 * Converts a fractional number to a float
 *
 * @param  [out]pOut  The converted fraction
 * @param  [ in]pFrac The fraction
 */
void fraction_fconvert(float *pOut, fraction *pFrac);

/**
 * Converts a fractional number to a double
 *
 * @param  [out]pOut  The converted fraction
 * @param  [ in]pFrac The fraction
 */
void fraction_dconvert(double *pOut, fraction *pFrac);

/**
 * Converts a fractional number through a division, retrieving the number's
 * quotient and remainder
 *
 * @param  [out]pQuotOut The fraction's quotient
 * @param  [out]pRemOut  The fraction's remainder
 * @param  [ in]pFrac    The fraction
 */
void fraction_divConvert(int *pQuotOut, int *pRemOut, fraction *pFrac);

#endif /* __FRACTION_H__ */

