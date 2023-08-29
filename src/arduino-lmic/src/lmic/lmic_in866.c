/*
* Copyright (c) 2014-2016 IBM Corporation.
* Copyright (c) 2017, 2019-2021 MCCI Corporation.
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

#define LMIC_DR_LEGACY 0

#include "lmic_bandplan.h"

#if defined(CFG_in866)
// ================================================================================
//
// BEG: IN866 related stuff
//

CONST_TABLE(u1_t, _DR2RPS_CRC)[] = {
        ILLEGAL_RPS,
        (u1_t)MAKERPS(SF12, BW125, CR_4_5, 0, 0),       // [0]
        (u1_t)MAKERPS(SF11, BW125, CR_4_5, 0, 0),       // [1]
        (u1_t)MAKERPS(SF10, BW125, CR_4_5, 0, 0),       // [2]
        (u1_t)MAKERPS(SF9,  BW125, CR_4_5, 0, 0),       // [3]
        (u1_t)MAKERPS(SF8,  BW125, CR_4_5, 0, 0),       // [4]
        (u1_t)MAKERPS(SF7,  BW125, CR_4_5, 0, 0),       // [5]
        ILLEGAL_RPS,                                    // [6]
        (u1_t)MAKERPS(FSK,  BW125, CR_4_5, 0, 0),       // [7]
        ILLEGAL_RPS
};

bit_t
LMICin866_validDR(dr_t dr) {
        // use subtract here to avoid overflow
        if (dr >= LENOF_TABLE(_DR2RPS_CRC) - 2)
                return 0;
        return TABLE_GET_U1(_DR2RPS_CRC, dr+1)!=ILLEGAL_RPS;
}

static CONST_TABLE(u1_t, maxFrameLens)[] = {
        59+5, 59+5, 59+5, 123+5, 250+5, 250+5, 0, 250+5
};

uint8_t LMICin866_maxFrameLen(uint8_t dr) {
        if (dr < LENOF_TABLE(maxFrameLens))
                return TABLE_GET_U1(maxFrameLens, dr);
        else
                return 0;
}

static CONST_TABLE(s1_t, TXPOWLEVELS)[] = {
        30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10
};

int8_t LMICin866_pow2dBm(uint8_t mcmd_ladr_p1) {
        uint8_t const pindex = (mcmd_ladr_p1&MCMD_LinkADRReq_POW_MASK)>>MCMD_LinkADRReq_POW_SHIFT;
        if (pindex < LENOF_TABLE(TXPOWLEVELS)) {
                return TABLE_GET_S1(TXPOWLEVELS, pindex);
        } else {
                return -128;
        }
}

// only used in this module, but used by variant macro dr2hsym().
static CONST_TABLE(ostime_t, DR2HSYM_osticks)[] = {
        us2osticksRound(128 << 7),  // DR_SF12
        us2osticksRound(128 << 6),  // DR_SF11
        us2osticksRound(128 << 5),  // DR_SF10
        us2osticksRound(128 << 4),  // DR_SF9
        us2osticksRound(128 << 3),  // DR_SF8
        us2osticksRound(128 << 2),  // DR_SF7
        us2osticksRound(128 << 1),  // --
        us2osticksRound(80)       // FSK -- not used (time for 1/2 byte)
};

ostime_t LMICin866_dr2hsym(uint8_t dr) {
        return TABLE_GET_OSTIME(DR2HSYM_osticks, dr);
}


// All frequencies are marked as BAND_MILLI, and we don't do duty-cycle. But this lets
// us reuse code.
enum { NUM_DEFAULT_CHANNELS = 3 };
static CONST_TABLE(u4_t, iniChannelFreq)[NUM_DEFAULT_CHANNELS] = {
        // Default operational frequencies
        IN866_F1 | BAND_MILLI,
        IN866_F2 | BAND_MILLI,
        IN866_F3 | BAND_MILLI,
};

// india ignores join, becuase the channel setup is the same either way.
void LMICin866_initDefaultChannels(bit_t join) {
        LMIC_API_PARAMETER(join);

        os_clearMem(&LMIC.channelFreq, sizeof(LMIC.channelFreq));
#if !defined(DISABLE_MCMD_DlChannelReq)
        os_clearMem(&LMIC.channelDlFreq, sizeof(LMIC.channelDlFreq));
#endif // !DISABLE_MCMD_DlChannelReq
        os_clearMem(&LMIC.channelDrMap, sizeof(LMIC.channelDrMap));
        os_clearMem(&LMIC.bands, sizeof(LMIC.bands));

        LMIC.channelMap = (1 << NUM_DEFAULT_CHANNELS) - 1;
        for (u1_t fu = 0; fu<NUM_DEFAULT_CHANNELS; fu++) {
                LMIC.channelFreq[fu] = TABLE_GET_U4(iniChannelFreq, fu);
                LMIC.channelDrMap[fu] = DR_RANGE_MAP(IN866_DR_SF12, IN866_DR_SF7);
        }

        LMIC.bands[BAND_MILLI].txcap = 1;  // no limit, in effect.
        LMIC.bands[BAND_MILLI].txpow = IN866_TX_EIRP_MAX_DBM;
        LMIC.bands[BAND_MILLI].lastchnl = os_getRndU1() % MAX_CHANNELS;
        LMIC.bands[BAND_MILLI].avail = os_getTime();
}

bit_t LMIC_setupBand(u1_t bandidx, s1_t txpow, u2_t txcap) {
        if (bandidx > BAND_MILLI) return 0;
        //band_t* b = &LMIC.bands[bandidx];
        xref2band_t b = &LMIC.bands[bandidx];
        b->txpow = txpow;
        b->txcap = txcap;
        b->avail = os_getTime();
        b->lastchnl = os_getRndU1() % MAX_CHANNELS;
        return 1;
}

///
/// \brief query number of default channels.
///
u1_t LMIC_queryNumDefaultChannels() {
        return NUM_DEFAULT_CHANNELS;
}

///
/// \brief LMIC_setupChannel for IN region
///
/// \note according to LoRaWAN 1.3 section 5.6, "the acceptable range
///     for **ChIndex** is N to 16", where N is our \c NUM_DEFAULT_CHANNELS.
///     This routine is used internally for MAC commands, so we enforce
///     this for the extenal API as well.
///
bit_t LMIC_setupChannel(u1_t chidx, u4_t freq, u2_t drmap, s1_t band) {
        // zero the band bits in freq, just in case.
        freq &= ~3;

        if (chidx < NUM_DEFAULT_CHANNELS) {
                return 0;
        }
        bit_t fEnable = (freq != 0);
        if (chidx >= MAX_CHANNELS)
                return 0;
        if (band == -1) {
                freq = (freq&~3) | BAND_MILLI;
        } else {
                if (band > BAND_MILLI) return 0;
                freq = (freq&~3) | band;
        }
        LMIC.channelFreq[chidx] = freq;
        LMIC.channelDrMap[chidx] = drmap == 0 ? DR_RANGE_MAP(IN866_DR_SF12, IN866_DR_SF7) : drmap;
        if (fEnable)
                LMIC.channelMap |= 1 << chidx;  // enabled right away
        else
                LMIC.channelMap &= ~(1 << chidx);
        return 1;
}



u4_t LMICin866_convFreq(xref2cu1_t ptr) {
        u4_t freq = (os_rlsbf4(ptr - 1) >> 8) * 100;
        if (freq < IN866_FREQ_MIN || freq > IN866_FREQ_MAX)
                freq = 0;
        return freq;
}

///
/// \brief change the TX channel given the desired tx time.
///
/// \param [in] now is the time at which we want to transmit. In fact, it's always
///     the current time.
///
/// \returns the actual time at which we can transmit. \c LMIC.txChnl is set to the
///     selected channel.
///
/// \details
///     We scan all the bands, creating a mask of all enabled channels that are
///     feasible at the earliest possible time. We then randomly choose one from
///     that, updating the shuffle mask.
///
///     Since there's no duty cycle limitation, and no dwell limitation,
///     we just choose a channel from the shuffle and return the current time.
///
ostime_t LMICin866_nextTx(ostime_t now) {
        uint16_t availmask;

        // scan all the enabled channels and make a mask of candidates
        availmask = 0;
        for (u1_t chnl = 0; chnl < MAX_CHANNELS; ++chnl) {
                // not enabled?
                if ((LMIC.channelMap & (1 << chnl)) == 0)
                        continue;
                // not feasible?
                if ((LMIC.channelDrMap[chnl] & (1 << (LMIC.datarate & 0xF))) == 0)
                        continue;
                availmask |= 1 << chnl;
        }

        // now: calculate the mask
        int candidateCh = LMIC_findNextChannel(&LMIC.channelShuffleMap, &availmask, 1, LMIC.txChnl == 0xFF ? -1 : LMIC.txChnl);
        if (candidateCh >= 0) {
                // update the channel.
                LMIC.txChnl = candidateCh;
        }
        return now;
}

#if !defined(DISABLE_BEACONS)
void LMICin866_setBcnRxParams(void) {
        LMIC.dataLen = 0;
        LMIC.freq = LMIC.channelFreq[LMIC.bcnChnl] & ~(u4_t)3;
        LMIC.rps = setIh(setNocrc(dndr2rps((dr_t)DR_BCN), 1), LEN_BCN);
}
#endif // !DISABLE_BEACONS

#if !defined(DISABLE_JOIN)
ostime_t LMICin866_nextJoinState(void) {
        return LMICeulike_nextJoinState(NUM_DEFAULT_CHANNELS);
}
#endif // !DISABLE_JOIN

// set the Rx1 dndr, rps.
void LMICin866_setRx1Params(void) {
    u1_t const txdr = LMIC.dndr;
    s1_t drOffset;
    s1_t candidateDr;

    LMICeulike_setRx1Freq();

    if ( LMIC.rx1DrOffset <= 5)
        drOffset = (s1_t) LMIC.rx1DrOffset;
    else
        drOffset = 5 - (s1_t) LMIC.rx1DrOffset;

    candidateDr = (s1_t) txdr - drOffset;
    if (candidateDr < LORAWAN_DR0)
            candidateDr = 0;
    else if (candidateDr > LORAWAN_DR5)
            candidateDr = LORAWAN_DR5;

    LMIC.dndr = (u1_t) candidateDr;
    LMIC.rps = dndr2rps(LMIC.dndr);
}

void
LMICin866_initJoinLoop(void) {
        LMICeulike_initJoinLoop(NUM_DEFAULT_CHANNELS, /* adr dBm */ IN866_TX_EIRP_MAX_DBM);
}

//
// END: IN866 related stuff
//
// ================================================================================
#endif