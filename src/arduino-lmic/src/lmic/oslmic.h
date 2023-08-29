/*
 * Copyright (c) 2014-2016 IBM Corporation.
 * Copyright (c) 2018, 2019 MCCI Corporation
 * All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//! \file
#ifndef _oslmic_h_
#define _oslmic_h_

// Dependencies required for the LMIC to run.
// These settings can be adapted to the underlying system.
// You should not, however, change the lmic merely for porting purposes.[hc]

#include "config.h"

#ifndef _lmic_env_h_
# include "lmic_env.h"
#endif

#ifndef _oslmic_types_h_
# include "oslmic_types.h"
#endif

LMIC_BEGIN_DECLS


#include <string.h>
#include "hal.h"
#define EV(a,b,c) /**/
#define DO_DEVDB(field1,field2) /**/
#if !defined(CFG_noassert)
#define ASSERT(cond) if(!(cond)) hal_failed(__FILE__, __LINE__)
#else
#define ASSERT(cond) /**/
#endif

#define os_clearMem(a,b)   memset(a,0,b)
#define os_copyMem(a,b,c)  memcpy(a,b,c)

typedef     struct osjob_t osjob_t;
typedef      struct band_t band_t;
typedef   struct chnldef_t chnldef_t;
typedef   struct rxsched_t rxsched_t;
typedef   struct bcninfo_t bcninfo_t;
typedef        const u1_t* xref2cu1_t;
typedef              u1_t* xref2u1_t;

// int32_t == s4_t is long on some platforms; and someday
// we will want 64-bit ostime_t. So, we will use a macro for the
// print formatting of ostime_t.
#ifndef LMIC_PRId_ostime_t
# include <inttypes.h>
# define LMIC_PRId_ostime_t	PRId32
#endif

#define TYPEDEF_xref2rps_t     typedef         rps_t* xref2rps_t
#define TYPEDEF_xref2rxsched_t typedef     rxsched_t* xref2rxsched_t
#define TYPEDEF_xref2chnldef_t typedef     chnldef_t* xref2chnldef_t
#define TYPEDEF_xref2band_t    typedef        band_t* xref2band_t
#define TYPEDEF_xref2osjob_t   typedef       osjob_t* xref2osjob_t

#define SIZEOFEXPR(x) sizeof(x)

#define DECL_ON_LMIC_EVENT LMIC_DECLARE_FUNCTION_WEAK(void, onEvent, (ev_t e))

extern u4_t AESAUX[];
extern u4_t AESKEY[];
#define AESkey ((u1_t*)AESKEY)
#define AESaux ((u1_t*)AESAUX)
#define FUNC_ADDR(func) (&(func))

u1_t radio_rand1 (void);
#define os_getRndU1() radio_rand1()

#define DEFINE_LMIC  struct lmic_t LMIC
#define DECLARE_LMIC extern struct lmic_t LMIC

typedef struct oslmic_radio_rssi_s oslmic_radio_rssi_t;

struct oslmic_radio_rssi_s {
        s2_t    min_rssi;
        s2_t    max_rssi;
        s2_t    mean_rssi;
        u2_t    n_rssi;
};

int radio_init (void);
void radio_irq_handler (u1_t dio);
void radio_irq_handler_v2 (u1_t dio, ostime_t tref);
void os_init (void);
int os_init_ex (const void *pPinMap);
void os_runloop (void);
void os_runloop_once (void);
u1_t radio_rssi (void);
void radio_monitor_rssi(ostime_t n, oslmic_radio_rssi_t *pRssi);

//================================================================================

#ifndef RX_RAMPUP_DEFAULT
//! \brief RX_RAMPUP_DEFAULT specifies the extra time we must allow to set up an RX event due
//! to platform issues. It's specified in units of ostime_t. It must reflect
//! platform jitter and latency, as well as the speed of the LMIC when running
//! on this plaform. It's not used directly; clients call os_getRadioRxRampup(),
//! which might adaptively vary this based on observed timeouts.
#define RX_RAMPUP_DEFAULT  (us2osticks(10000))
#endif

#ifndef TX_RAMPUP
// TX_RAMPUP specifies the extra time we must allow to set up a TX event) due
// to platform issues. It's specified in units of ostime_t. It must reflect
// platform jitter and latency, as well as the speed of the LMIC when running
// on this plaform.
#define TX_RAMPUP  (us2osticks(10000))
#endif

#ifndef OSTICKS_PER_SEC
#define OSTICKS_PER_SEC 32768
#elif OSTICKS_PER_SEC < 10000 || OSTICKS_PER_SEC > 64516
#error Illegal OSTICKS_PER_SEC - must be in range [10000:64516]. One tick must be 15.5us .. 100us long.
#endif

#if !HAS_ostick_conv
#define us2osticks(us)   ((ostime_t)( ((int64_t)(us) * OSTICKS_PER_SEC) / 1000000))
#define ms2osticks(ms)   ((ostime_t)( ((int64_t)(ms) * OSTICKS_PER_SEC)    / 1000))
#define sec2osticks(sec) ((ostime_t)( (int64_t)(sec) * OSTICKS_PER_SEC))
#define osticks2ms(os)   ((s4_t)(((os)*(int64_t)1000    ) / OSTICKS_PER_SEC))
#define osticks2us(os)   ((s4_t)(((os)*(int64_t)1000000 ) / OSTICKS_PER_SEC))
// Special versions
#define us2osticksCeil(us)  ((ostime_t)( ((int64_t)(us) * OSTICKS_PER_SEC + 999999) / 1000000))
#define us2osticksRound(us) ((ostime_t)( ((int64_t)(us) * OSTICKS_PER_SEC + 500000) / 1000000))
#define ms2osticksCeil(ms)  ((ostime_t)( ((int64_t)(ms) * OSTICKS_PER_SEC + 999) / 1000))
#define ms2osticksRound(ms) ((ostime_t)( ((int64_t)(ms) * OSTICKS_PER_SEC + 500) / 1000))
#endif


struct osjob_t;  // fwd decl.

//! the function type for osjob_t callbacks
typedef void (osjobcbfn_t)(struct osjob_t*);

//! the pointer-to-function for osjob_t callbacks
typedef osjobcbfn_t *osjobcb_t;

struct osjob_t {
    struct osjob_t* next;
    ostime_t deadline;
    osjobcb_t  func;
};
TYPEDEF_xref2osjob_t;

//! determine whether a job is timed or immediate. os_setTimedCallback()
// must treat incoming == 0 as being 1 instead.
static inline int os_jobIsTimed(xref2osjob_t job) {
    return (job->deadline != 0);
}

#ifndef HAS_os_calls

#ifndef os_getDevKey
void os_getDevKey (xref2u1_t buf);
#endif
#ifndef os_getArtEui
void os_getArtEui (xref2u1_t buf);
#endif
#ifndef os_getDevEui
void os_getDevEui (xref2u1_t buf);
#endif
#ifndef os_setCallback
void os_setCallback (xref2osjob_t job, osjobcb_t cb);
#endif
#ifndef os_setTimedCallback
void os_setTimedCallback (xref2osjob_t job, ostime_t time, osjobcb_t cb);
#endif
#ifndef os_clearCallback
void os_clearCallback (xref2osjob_t job);
#endif
#ifndef os_getRadioRxRampup
ostime_t os_getRadioRxRampup (void);
#endif
#ifndef os_getTime
ostime_t os_getTime (void);
#endif
#ifndef os_getTimeSecs
uint os_getTimeSecs (void);
#endif
#ifndef os_radio
void os_radio (u1_t mode);
#endif
#ifndef os_getBattLevel
u1_t os_getBattLevel (void);
#endif
#ifndef os_queryTimeCriticalJobs
//! Return non-zero if any jobs are scheduled between now and now+time.
bit_t os_queryTimeCriticalJobs(ostime_t time);
#endif

#ifndef os_rlsbf4
//! Read 32-bit quantity from given pointer in little endian byte order.
u4_t os_rlsbf4 (xref2cu1_t buf);
#endif
#ifndef os_wlsbf4
//! Write 32-bit quntity into buffer in little endian byte order.
void os_wlsbf4 (xref2u1_t buf, u4_t value);
#endif
#ifndef os_rmsbf4
//! Read 32-bit quantity from given pointer in big endian byte order.
u4_t os_rmsbf4 (xref2cu1_t buf);
#endif
#ifndef os_wmsbf4
//! Write 32-bit quntity into buffer in big endian byte order.
void os_wmsbf4 (xref2u1_t buf, u4_t value);
#endif
#ifndef os_rlsbf2
//! Read 16-bit quantity from given pointer in little endian byte order.
u2_t os_rlsbf2 (xref2cu1_t buf);
#endif
#ifndef os_wlsbf2
//! Write 16-bit quantity into buffer in little endian byte order.
void os_wlsbf2 (xref2u1_t buf, u2_t value);
#endif

//! Get random number (default impl for u2_t).
#ifndef os_getRndU2
#define os_getRndU2() ((u2_t)((os_getRndU1()<<8)|os_getRndU1()))
#endif
#ifndef os_crc16
u2_t os_crc16 (xref2cu1_t d, uint len);
#endif

#endif // !HAS_os_calls

// ======================================================================
// Table support
// These macros for defining a table of constants and retrieving values
// from it makes it easier for other platforms (like AVR) to optimize
// table accesses.
// Use CONST_TABLE() whenever declaring or defining a table, and
// TABLE_GET_xx whenever accessing its values. The actual name of the
// declared variable will be modified to prevent accidental direct
// access. The accessor macros forward to an inline function to allow
// proper type checking of the array element type.

// Helper to add a prefix to the table name
#define RESOLVE_TABLE(table) constant_table_ ## table

// get number of entries in table
#define LENOF_TABLE(table) (sizeof(RESOLVE_TABLE(table)) / sizeof(RESOLVE_TABLE(table)[0]))

// Accessors for table elements
#define TABLE_GET_U1(table, index) table_get_u1(RESOLVE_TABLE(table), index)
#define TABLE_GET_S1(table, index) table_get_s1(RESOLVE_TABLE(table), index)
#define TABLE_GET_U2(table, index) table_get_u2(RESOLVE_TABLE(table), index)
#define TABLE_GET_S2(table, index) table_get_s2(RESOLVE_TABLE(table), index)
#define TABLE_GET_U4(table, index) table_get_u4(RESOLVE_TABLE(table), index)
#define TABLE_GET_S4(table, index) table_get_s4(RESOLVE_TABLE(table), index)
#define TABLE_GET_OSTIME(table, index) table_get_ostime(RESOLVE_TABLE(table), index)
#define TABLE_GET_U1_TWODIM(table, index1, index2) table_get_u1(RESOLVE_TABLE(table)[index1], index2)

#if defined(__AVR__)
    #include <avr/pgmspace.h>
    // Macro to define the getter functions. This loads data from
    // progmem using pgm_read_xx, or accesses memory directly when the
    // index is a constant so gcc can optimize it away;
    #define TABLE_GETTER(postfix, type, pgm_type) \
        static inline type table_get ## postfix(const type *table, size_t index) { \
            if (__builtin_constant_p(table[index])) \
                return table[index]; \
            return pgm_read_ ## pgm_type(&table[index]); \
        }

    TABLE_GETTER(_u1, u1_t, byte);
    TABLE_GETTER(_s1, s1_t, byte);
    TABLE_GETTER(_u2, u2_t, word);
    TABLE_GETTER(_s2, s2_t, word);
    TABLE_GETTER(_u4, u4_t, dword);
    TABLE_GETTER(_s4, s4_t, dword);

    // This assumes ostime_t is 4 bytes, so error out if it is not
    typedef int check_sizeof_ostime_t[(sizeof(ostime_t) == 4) ? 0 : -1];
    TABLE_GETTER(_ostime, ostime_t, dword);

    // For AVR, store constants in PROGMEM, saving on RAM usage
    #define CONST_TABLE(type, name) const type PROGMEM RESOLVE_TABLE(name)
#else
    static inline u1_t table_get_u1(const u1_t *table, size_t index) { return table[index]; }
    static inline s1_t table_get_s1(const s1_t *table, size_t index) { return table[index]; }
    static inline u2_t table_get_u2(const u2_t *table, size_t index) { return table[index]; }
    static inline s2_t table_get_s2(const s2_t *table, size_t index) { return table[index]; }
    static inline u4_t table_get_u4(const u4_t *table, size_t index) { return table[index]; }
    static inline s4_t table_get_s4(const s4_t *table, size_t index) { return table[index]; }
    static inline ostime_t table_get_ostime(const ostime_t *table, size_t index) { return table[index]; }

    // Declare a table
    #define CONST_TABLE(type, name) const type RESOLVE_TABLE(name)
#endif

// ======================================================================
// AES support
// !!Keep in sync with lorabase.hpp!!

#ifndef AES_ENC  // if AES_ENC is defined as macro all other values must be too
#define AES_ENC       0x00
#define AES_DEC       0x01
#define AES_MIC       0x02
#define AES_CTR       0x04
#define AES_MICNOAUX  0x08
#endif
#ifndef AESkey  // if AESkey is defined as macro all other values must be too
extern xref2u1_t AESkey;
extern xref2u1_t AESaux;
#endif
#ifndef os_aes
u4_t os_aes (u1_t mode, xref2u1_t buf, u2_t len);
#endif

// ======================================================================
// Simple logging support. Vanishes unless enabled.

#if LMIC_ENABLE_event_logging
extern void LMICOS_logEvent(const char *pMessage);
extern void LMICOS_logEventUint32(const char *pMessage, uint32_t datum);
#else // ! LMIC_ENABLE_event_logging
# define LMICOS_logEvent(m)     do { ; } while (0)
# define LMICOS_logEventUint32(m, d) do { ; } while (0)
#endif // ! LMIC_ENABLE_event_logging


LMIC_END_DECLS

#endif // _oslmic_h_
