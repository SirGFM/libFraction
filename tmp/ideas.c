
struct stFraction {
    int numerator;
    int denominator;
};
typedef struct stFraction fraction;

/**
 * *pVal = pFrac->numerator / pFrac->denominator
 */
void toInt(int *pVal, fraction *pFrac);

/**
 * *pVal = (pFrac->numerator * pow(10, numDec)) / pFrac->denominator
 */
void toFixed(int *pVal, fraction *pFrac, int numDec);

/**
 * *pVal = pFrac->numerator / (float)pFrac->denominator
 */
void toFloat(float *pVal, fraction *pFrac);

/**
 * *pVal = pFrac->numerator / (double)pFrac->denominator
 */
void toDouble(float *pVal, fraction *pFrac);

/**
 * setGCD(pA, pB)
 * pDst->numerator = pA->numerator + pB->numerator
 * pDst->denominator = pA->denominator
 * simplify(pA)
 * simplify(pB)
 * simplify(pDst)
 */
void sum(fraction *pDst, fraction *pA, fraction *pB);

/**
 * setGCD(pA, pB)
 * pDst->numerator = pA->numerator - pB->numerator
 * pDst->denominator = pA->denominator
 * simplify(pA)
 * simplify(pB)
 * simplify(pDst)
 */
void sub(fraction *pDst, fraction *pA, fraction *pB);

/**
 * pDst->numerator = pA->numerator * pB->numerator
 * pDst->denominator = pA->denominator * pB->denominator
 * simplify(pDst)
 */
void mul(fraction *pDst, fraction *pA, fraction *pB);

/**
 * pDst->numerator = pA->numerator * pB->denominator
 * pDst->denominator = pA->denominator * pB->numerator
 * simplify(pDst)
 */
void div(fraction *pDst, fraction *pA, fraction *pB);

/**
 * pA->numerator = pA->numerator * pB->denominator
 * pB->numerator = pB->numerator * pA->denominator
 * pA->denominator = pA->denominator * pB->denominator
 * pB->denominator = pA->denominator
 */
void setGCD(fraction *pA, fraction *pB);

/**
 * primes[] array of ordered primes
 *
 * i = 0
 * while (pA->numerator > 0 && pA->denominator > 0 &&
 *         pA->numerator <= primes[i] && pA->denominator <= primes[i] &&
 *         i < numPrimes) {
 *     if (abs(pA->numerator) % prime[i] == 0 &&
 *             abs(pA->denominator) % prime[i] == 0) {
 *         pA->numerator /= prime[i]
 *         pA->numerator /= prime[i]
 *     }
 *     else {
 *         i++;
 *     }
 * }
 */
void simplify(fraction *pA);

