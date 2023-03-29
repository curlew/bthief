/**
 * @file nss.hxx
 * @brief NSS & NSPR types.
 * @date 2023-03-29
 * See https://github.com/nss-dev/nss & https://github.com/servo/nspr.
 * (modified)
 */

#ifndef NSS_HXX_INCLUDED
#define NSS_HXX_INCLUDED

#include <cstdint>

// ----  nss/lib/util/seccomon.h  ------------------------------------------------------------------

typedef enum {
    siBuffer = 0,
    siClearDataBuffer = 1,
    siCipherDataBuffer = 2,
    siDERCertBuffer = 3,
    siEncodedCertBuffer = 4,
    siDERNameBuffer = 5,
    siEncodedNameBuffer = 6,
    siAsciiNameString = 7,
    siAsciiString = 8,
    siDEROID = 9,
    siUnsignedInteger = 10,
    siUTCTime = 11,
    siGeneralizedTime = 12,
    siVisibleString = 13,
    siUTF8String = 14,
    siBMPString = 15
} SECItemType;

typedef struct {
    SECItemType type;
    unsigned char *data;
    unsigned int len;
} SECItem;

typedef enum {
    SECWouldBlock = -2,
    SECFailure = -1,
    SECSuccess = 0
} SECStatus;

// ----  nspr/pr/include/prtypes.h  ----------------------------------------------------------------

typedef int PRIntn;
typedef PRIntn PRBool;

// ----  nspr/lib/ds/plarena.h  --------------------------------------------------------------------

typedef uint32_t PRUint32;
typedef uint64_t PRUword;

typedef struct PLArena {
    PLArena *next;
    PRUword base;
    PRUword limit;
    PRUword avail;
} PLArena;

struct PLArenaPool {
    PLArena  first;
    PLArena  *current;
    PRUint32 arenasize;
    PRUword  mask;
};

// ----  nss/lib/util/secitem.h  -------------------------------------------------------------------

using SECITEM_AllocItem = SECItem *(PLArenaPool *, SECItem *, unsigned int);
using SECITEM_ZfreeItem = void(SECItem *, PRBool);

// ----  nss/lib/nss/nss.h  ------------------------------------------------------------------------

#define SECMOD_DB "secmod.db"
#define NSS_INIT_READONLY 0x1
using NSS_Initialize = SECStatus(const char *, const char *, const char *, const char *, PRUint32);
using NSS_Shutdown = SECStatus(void);

// ----  nss/lib/pk11wrap/pk11sdr.h  ---------------------------------------------------------------

using PK11SDR_Decrypt = SECStatus(SECItem *, SECItem *, void *);

#endif // ifndef NSS_HXX_INCLUDED
