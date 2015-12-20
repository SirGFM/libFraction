
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

