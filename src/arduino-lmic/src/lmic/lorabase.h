/*
 * Copyright (c) 2014-2016 IBM Corporation.
 * Copyright (c) 2017-2021 MCCI Corporation.
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

#ifndef _lorabase_h_
#define _lorabase_h_

#ifdef __cplusplus
extern "C"{
#endif

// ================================================================================
// BEG: Keep in sync with lorabase.hpp
//

enum _cr_t { CR_4_5=0, CR_4_6, CR_4_7, CR_4_8 };
enum _sf_t { FSK=0, SF7, SF8, SF9, SF10, SF11, SF12, SFrfu };
enum _bw_t { BW125=0, BW250, BW500, BWrfu };
typedef u1_t cr_t;
typedef u1_t sf_t;
typedef u1_t bw_t;
typedef u1_t dr_t;
typedef u2_t rxsyms_t;

// Radio parameter set (encodes SF/BW/CR/IH/NOCRC)
// 2..0:    Spreading factor
// 4..3:    bandwidth: 0 == 125kHz, 1 == 250 kHz, 2 == 500 kHz. 3 == reserved.
// 6..5:    coding rate: 0 == 4/5, 1 == 4/6, 2 == 4/7, 3 == 4/8
// 7:       nocrc: 0 == with crc, 1 == without crc
// 15..8:   Implicit header control: 0 ==> none, 1..0xFF ==> length in bytes.

typedef u2_t rps_t;
TYPEDEF_xref2rps_t;

enum { ILLEGAL_RPS = 0xFF };

// Global maximum frame length
enum { STD_PREAMBLE_LEN  =  8 };
enum { MAX_LEN_FRAME     =  LMIC_MAX_FRAME_LENGTH };
enum { LEN_DEVNONCE      =  2 };
enum { LEN_ARTNONCE      =  3 };
enum { LEN_NETID         =  3 };
enum { DELAY_JACC1       =  5 }; // in secs
enum { DELAY_DNW1        =  1 }; // in secs down window #1
enum { DELAY_EXTDNW2     =  1 }; // in secs
enum { DELAY_JACC2       =  DELAY_JACC1+(int)DELAY_EXTDNW2 }; // in secs
enum { DELAY_DNW2        =  DELAY_DNW1 +(int)DELAY_EXTDNW2 }; // in secs down window #1
enum { BCN_INTV_exp      = 7 };
enum { BCN_INTV_sec      = 1<<BCN_INTV_exp };
enum { BCN_INTV_ms       = BCN_INTV_sec*1000L };
enum { BCN_INTV_us       = BCN_INTV_ms*1000L };
enum { BCN_RESERVE_ms    = 2120 };   // space reserved for beacon and NWK management
enum { BCN_GUARD_ms      = 3000 };   // end of beacon period to prevent interference with beacon
enum { BCN_SLOT_SPAN_ms  =   30 };   // 2^12 reception slots a this span
enum { BCN_WINDOW_ms     = BCN_INTV_ms-(int)BCN_GUARD_ms-(int)BCN_RESERVE_ms };
enum { BCN_RESERVE_us    = 2120000 };
enum { BCN_GUARD_us      = 3000000 };
enum { BCN_SLOT_SPAN_us  =   30000 };

// there are exactly 16 datarates
enum _dr_code_t {
        LORAWAN_DR0 = 0,
        LORAWAN_DR1,
        LORAWAN_DR2,
        LORAWAN_DR3,
        LORAWAN_DR4,
        LORAWAN_DR5,
        LORAWAN_DR6,
        LORAWAN_DR7,
        LORAWAN_DR8,
        LORAWAN_DR9,
        LORAWAN_DR10,
        LORAWAN_DR11,
        LORAWAN_DR12,
        LORAWAN_DR13,
        LORAWAN_DR14,
        LORAWAN_DR15,
        LORAWAN_DR_LENGTH       // 16, for sizing arrays.
};

// post conditions from this block: symbols used by general code that is not
// ostensiblly region-specific.
// DR_DFLTMIN must be defined as a suitable substititute value if we get a bogus DR
//  It is misnamed, it should be the maximum DR (which is the minimum SF) for
//  125 kHz.
// DR_PAGE is used only for a non-supported debug system, but should be defined.
// CHNL_DNW2 is the channel to be used for RX2
// FREQ_DNW2 is the frequency to be used for RX2
// DR_DNW2 is the data-rate to be used for RX2
//
// The Class B stuff is untested and definitely wrong in parts for LoRaWAN 1.02
// CHNL_PING is the channel to be used for pinging.
// FREQ_PING is the default ping channel frequency
// DR_PING is the data-rate to be used for pings.
// CHNL_BCN is the channel to be used for the beacon (or perhaps the start chan)
// FREQ_BCN is the frequency to be used for the beacon
// DR_BCN is the datarate to be used for the beacon
// AIRTIME_BCN is the airtime for the beacon



#if defined(CFG_eu868) // ==============================================

#include "lorabase_eu868.h"

// per 2.1.3: not implemented
#define LMIC_ENABLE_TxParamSetupReq	0

enum { DR_DFLTMIN = EU868_DR_SF7 };   // DR5
                                      // DR_PAGE is a debugging parameter
enum { DR_PAGE = DR_PAGE_EU868 };

//enum { CHNL_PING         = 5 };
enum { FREQ_PING = EU868_F6 };  // default ping freq
enum { DR_PING = EU868_DR_SF9 };       // default ping DR
                                       //enum { CHNL_DNW2         = 5 };
enum { FREQ_DNW2 = EU868_F6 };
enum { DR_DNW2 = EU868_DR_SF12 };
enum { CHNL_BCN = 5 };
enum { FREQ_BCN = EU868_F6 };
enum { DR_BCN = EU868_DR_SF9 };
enum { AIRTIME_BCN = 144384 };  // micros
enum { LMIC_REGION_EIRP = EU868_LMIC_REGION_EIRP };         // region uses EIRP

enum {
        // Beacon frame format EU SF9
        OFF_BCN_NETID = 0,
        OFF_BCN_TIME = 3,
        OFF_BCN_CRC1 = 7,
        OFF_BCN_INFO = 8,
        OFF_BCN_LAT = 9,
        OFF_BCN_LON = 12,
        OFF_BCN_CRC2 = 15,
        LEN_BCN = 17
};

// for backwards compatibility. This must match _dr_eu868_t
# if LMIC_DR_LEGACY
enum _dr_configured_t {
	DR_SF12 = EU868_DR_SF12,
	DR_SF11 = EU868_DR_SF11,
	DR_SF10 = EU868_DR_SF10,
	DR_SF9  = EU868_DR_SF9,
	DR_SF8  = EU868_DR_SF8,
	DR_SF7  = EU868_DR_SF7,
	DR_SF7B = EU868_DR_SF7B,
	DR_FSK  = EU868_DR_FSK,
	DR_NONE = EU868_DR_NONE
};
# endif // LMIC_DR_LEGACY

#elif defined(CFG_us915)  // =========================================

#include "lorabase_us915.h"

// per 2.2.3: not implemented
#define LMIC_ENABLE_TxParamSetupReq	0

enum { DR_DFLTMIN = US915_DR_SF7 };  // DR5

// DR_PAGE is a debugging parameter; it must be defined but it has no use in arduino-lmic
enum { DR_PAGE = DR_PAGE_US915 };

//enum { CHNL_PING         = 0 }; // used only for default init of state (follows beacon - rotating)
enum { FREQ_PING         = US915_500kHz_DNFBASE + 0*US915_500kHz_DNFSTEP };  // default ping freq
enum { DR_PING           = US915_DR_SF10CR };       // default ping DR
//enum { CHNL_DNW2         = 0 };
enum { FREQ_DNW2         = US915_500kHz_DNFBASE + 0*US915_500kHz_DNFSTEP };
enum { DR_DNW2           = US915_DR_SF12CR };
enum { CHNL_BCN          = 0 }; // used only for default init of state (rotating beacon scheme)
enum { DR_BCN            = US915_DR_SF12CR };
// TODO(tmm@mcci.com): check this, as beacon DR was SF10 in IBM code.
enum { AIRTIME_BCN       = 72192 };  // micros
enum { LMIC_REGION_EIRP = US915_LMIC_REGION_EIRP };         // region uses EIRP

enum {
    // Beacon frame format US SF10
    OFF_BCN_NETID    = 0,
    OFF_BCN_TIME     = 3,
    OFF_BCN_CRC1     = 7,
    OFF_BCN_INFO     = 9,
    OFF_BCN_LAT      = 10,
    OFF_BCN_LON      = 13,
    OFF_BCN_RFU1     = 16,
    OFF_BCN_CRC2     = 17,
    LEN_BCN          = 19
};

# if LMIC_DR_LEGACY
enum _dr_configured_t {
        DR_SF10   = US915_DR_SF10,
        DR_SF9    = US915_DR_SF9,
        DR_SF8    = US915_DR_SF8,
        DR_SF7    = US915_DR_SF7,
        DR_SF8C   = US915_DR_SF8C,
        DR_NONE   = US915_DR_NONE,
        DR_SF12CR = US915_DR_SF12CR,
        DR_SF11CR = US915_DR_SF11CR,
        DR_SF10CR = US915_DR_SF10CR,
        DR_SF9CR  = US915_DR_SF9CR,
        DR_SF8CR  = US915_DR_SF8CR,
        DR_SF7CR  = US915_DR_SF7CR
};
# endif // LMIC_DR_LEGACY

#elif defined(CFG_au915)  // =========================================

#include "lorabase_au915.h"

// per 2.5.3: must be implemented
#define LMIC_ENABLE_TxParamSetupReq	1

enum { DR_DFLTMIN       = AU915_DR_SF7 };  // DR5

                                // DR_PAGE is a debugging parameter; it must be defined but it has no use in arduino-lmic
enum { DR_PAGE          = DR_PAGE_AU915 };

//enum { CHNL_PING        = 0 }; // used only for default init of state (follows beacon - rotating)
enum { FREQ_PING        = AU915_500kHz_DNFBASE + 0*AU915_500kHz_DNFSTEP };  // default ping freq
enum { DR_PING          = AU915_DR_SF10CR };       // default ping DR
//enum { CHNL_DNW2        = 0 };
enum { FREQ_DNW2        = AU915_500kHz_DNFBASE + 0*AU915_500kHz_DNFSTEP };
enum { DR_DNW2          = AU915_DR_SF12CR };                  // DR8
enum { CHNL_BCN         = 0 }; // used only for default init of state (rotating beacon scheme)
enum { DR_BCN           = AU915_DR_SF10CR };
enum { AIRTIME_BCN      = 72192 };  // micros ... TODO(tmm@mcci.com) check.
enum { LMIC_REGION_EIRP = AU915_LMIC_REGION_EIRP };         // region uses EIRP

enum {
        // Beacon frame format AU DR10/SF10 500kHz
        OFF_BCN_NETID = 0,
        OFF_BCN_TIME = 3,
        OFF_BCN_CRC1 = 7,
        OFF_BCN_INFO = 9,
        OFF_BCN_LAT = 10,
        OFF_BCN_LON = 13,
        OFF_BCN_RFU1 = 16,
        OFF_BCN_CRC2 = 17,
        LEN_BCN = 19
};

# if LMIC_DR_LEGACY
enum _dr_configured_t {
        DR_SF12    = AU915_DR_SF12,
        DR_SF11    = AU915_DR_SF11,
        DR_SF10    = AU915_DR_SF10,
        DR_SF9     = AU915_DR_SF9,
        DR_SF8     = AU915_DR_SF8,
        DR_SF7     = AU915_DR_SF7,
        DR_SF8C    = AU915_DR_SF8C,
        DR_NONE    = AU915_DR_NONE,
        DR_SF12CR  = AU915_DR_SF12CR,
        DR_SF11CR  = AU915_DR_SF11CR,
        DR_SF10CR  = AU915_DR_SF10CR,
        DR_SF9CR   = AU915_DR_SF9CR,
        DR_SF8CR   = AU915_DR_SF8CR,
        DR_SF7CR   = AU915_DR_SF7CR
};
# endif // LMIC_DR_LEGACY

#elif defined(CFG_as923) // ==============================================

#include "lorabase_as923.h"

// per 2.7.3: must be implemented
#define LMIC_ENABLE_TxParamSetupReq	1

enum { DR_DFLTMIN = AS923_DR_SF10 };  // DR2
                                      // DR_PAGE is a debugging parameter
enum { DR_PAGE = DR_PAGE_AS923 };

enum { FREQ_PING = AS923_F2 };         // default ping freq
enum { DR_PING = AS923_DR_SF9 };       // default ping DR: DR3
enum { FREQ_DNW2 = AS923_FDOWN };
enum { DR_DNW2 = AS923_DR_SF10 };
enum { CHNL_BCN = 5 };
enum { FREQ_BCN = AS923_FBCN };
enum { DR_BCN = AS923_DR_SF9 };
enum { AIRTIME_BCN = 144384 };  // micros
enum { LMIC_REGION_EIRP = AS923_LMIC_REGION_EIRP };         // region uses EIRP

enum {
        // Beacon frame format AS SF9
        OFF_BCN_NETID = 0,
        OFF_BCN_TIME = 2,
        OFF_BCN_CRC1 = 6,
        OFF_BCN_INFO = 8,
        OFF_BCN_LAT = 9,
        OFF_BCN_LON = 12,
        OFF_BCN_CRC2 = 15,
        LEN_BCN = 17
};

# if LMIC_DR_LEGACY
enum _dr_configured_t {
        DR_SF12 = AS923_DR_SF12,
        DR_SF11 = AS923_DR_SF11,
        DR_SF10 = AS923_DR_SF10,
        DR_SF9  = AS923_DR_SF9,
        DR_SF8  = AS923_DR_SF8,
        DR_SF7  = AS923_DR_SF7,
        DR_SF7B = AS923_DR_SF7B,
        DR_FSK  = AS923_DR_FSK,
        DR_NONE = AS923_DR_NONE
};
# endif // LMIC_DR_LEGACY

#elif defined(CFG_kr920) // ==============================================

#include "lorabase_kr920.h"

// per 2.8.3 (1.0.3 2.9.3): is not implemented
#define LMIC_ENABLE_TxParamSetupReq	0

enum { DR_DFLTMIN = KR920_DR_SF12 };  // DR2
                                      // DR_PAGE is a debugging parameter
enum { DR_PAGE = DR_PAGE_KR920 };

enum { FREQ_PING = KR920_FBCN };       // default ping freq
enum { DR_PING = KR920_DR_SF9 };       // default ping DR: DR3
enum { FREQ_DNW2 = KR920_FDOWN };
enum { DR_DNW2 = KR920_DR_SF12 };
enum { CHNL_BCN = 11 };
enum { FREQ_BCN = KR920_FBCN };
enum { DR_BCN = KR920_DR_SF9 };
enum { AIRTIME_BCN = 144384 };  // micros
enum { LMIC_REGION_EIRP = KR920_LMIC_REGION_EIRP };         // region uses EIRP

enum {
        // Beacon frame format KR SF9
        OFF_BCN_NETID = 0,
        OFF_BCN_TIME = 2,
        OFF_BCN_CRC1 = 6,
        OFF_BCN_INFO = 8,
        OFF_BCN_LAT = 9,
        OFF_BCN_LON = 12,
        OFF_BCN_CRC2 = 15,
        LEN_BCN = 17
};

# if LMIC_DR_LEGACY
enum _dr_configured_t {
        DR_SF12 = KR920_DR_SF12,
        DR_SF11 = KR920_DR_SF11,
        DR_SF10 = KR920_DR_SF10,
        DR_SF9  = KR920_DR_SF9,
        DR_SF8  = KR920_DR_SF8,
        DR_SF7  = KR920_DR_SF7,
        DR_NONE = KR920_DR_NONE
};
# endif // LMIC_DR_LEGACY

#elif defined(CFG_in866) // ==============================================

#include "lorabase_in866.h"

// per 2.9.3: not implemented
#define LMIC_ENABLE_TxParamSetupReq	0

enum { DR_DFLTMIN = IN866_DR_SF7 };     // DR5
enum { DR_PAGE = DR_PAGE_IN866 };       // DR_PAGE is a debugging parameter

enum { FREQ_PING = IN866_FB };          // default ping freq
enum { DR_PING = IN866_DR_SF8 };        // default ping DR
enum { FREQ_DNW2 = IN866_FB };
enum { DR_DNW2 = IN866_DR_SF10 };
enum { CHNL_BCN = 5 };
enum { FREQ_BCN = IN866_FB };
enum { DR_BCN = IN866_DR_SF8 };
enum { AIRTIME_BCN = 144384 };  // micros
enum { LMIC_REGION_EIRP = IN866_LMIC_REGION_EIRP };         // region uses EIRP

enum {
        // Beacon frame format IN SF9
        OFF_BCN_NETID = 0,
        OFF_BCN_TIME = 1,
        OFF_BCN_CRC1 = 5,
        OFF_BCN_INFO = 7,
        OFF_BCN_LAT = 8,
        OFF_BCN_LON = 11,
        OFF_BCN_CRC2 = 17,
        LEN_BCN = 19
};

# if LMIC_DR_LEGACY
enum _dr_configured_t {
        DR_SF12 = IN866_DR_SF12,          // DR0
        DR_SF11 = IN866_DR_SF11,          // DR1
        DR_SF10 = IN866_DR_SF10,          // DR2
        DR_SF9  = IN866_DR_SF9,           // DR3
        DR_SF8  = IN866_DR_SF8,           // DR4
        DR_SF7  = IN866_DR_SF7,           // DR5
        DR_FSK  = IN866_DR_FSK,           // DR7
        DR_NONE = IN866_DR_NONE
};
# endif // LMIC_DR_LEGACY

#else
# error Unsupported configuration setting
#endif // ===================================================

enum {
    // Join Request frame format
    OFF_JR_HDR      = 0,
    OFF_JR_ARTEUI   = 1,
    OFF_JR_DEVEUI   = 9,
    OFF_JR_DEVNONCE = 17,
    OFF_JR_MIC      = 19,
    LEN_JR          = 23
};
enum {
    // Join Accept frame format
    OFF_JA_HDR      = 0,
    OFF_JA_ARTNONCE = 1,
    OFF_JA_NETID    = 4,
    OFF_JA_DEVADDR  = 7,
    OFF_JA_RFU      = 11,
    OFF_JA_DLSET    = 11,
    OFF_JA_RXDLY    = 12,
    OFF_CFLIST      = 13,
    LEN_JA          = 17,
    LEN_JAEXT       = 17+16
};

enum {
    // JoinAccept CFList types
    LORAWAN_JoinAccept_CFListType_FREQUENCIES = 0,  ///< the CFList contains 5 frequencies
    LORAWAN_JoinAccept_CFListType_MASK = 1,         ///< the CFList contains channel-mask data
};

enum {
    // Data frame format
    OFF_DAT_HDR      = 0,
    OFF_DAT_ADDR     = 1,
    OFF_DAT_FCT      = 5,
    OFF_DAT_SEQNO    = 6,
    OFF_DAT_OPTS     = 8,
};
enum { MAX_LEN_PAYLOAD = MAX_LEN_FRAME-(int)OFF_DAT_OPTS-4 };
enum {
    // Bitfields in frame format octet
    HDR_FTYPE   = 0xE0,
    HDR_RFU     = 0x1C,
    HDR_MAJOR   = 0x03
};
enum { HDR_FTYPE_DNFLAG = 0x20 };  // flags DN frame except for HDR_FTYPE_PROP
enum {
    // Values of frame type bit field
    HDR_FTYPE_JREQ   = 0x00,
    HDR_FTYPE_JACC   = 0x20,
    HDR_FTYPE_DAUP   = 0x40,  // data (unconfirmed) up
    HDR_FTYPE_DADN   = 0x60,  // data (unconfirmed) dn
    HDR_FTYPE_DCUP   = 0x80,  // data confirmed up
    HDR_FTYPE_DCDN   = 0xA0,  // data confirmed dn
    HDR_FTYPE_PROP   = 0xE0
};
enum {
    HDR_MAJOR_V1 = 0x00,
};
enum {
    // Bitfields in frame control octet
    FCT_ADREN       = 0x80,
    FCT_ADRACKReq   = 0x40,
    FCT_ACK         = 0x20,
    FCT_MORE        = 0x10,   // also in DN direction: Class B indicator
    FCT_OPTLEN      = 0x0F,
};
enum {
    // In UP direction: signals class B enabled
    FCT_CLASSB = FCT_MORE
};

enum {
    LWAN_FCtrl_FOptsLen_MAX = 0x0Fu,  // maximum size of embedded MAC commands
};

enum {
    NWKID_MASK = (int)0xFE000000,
    NWKID_BITS = 7
};

// MAC uplink commands   downwlink too
enum {
    // Class A
    MCMD_LinkCheckReq = 0x02,       // -
    MCMD_LinkADRAns = 0x03,         // u1:7-3:RFU, 3/2/1: pow/DR/Ch ACK
    MCMD_DutyCycleAns = 0x04,       // -
    MCMD_RXParamSetupAns = 0x05,    // u1:7-2:RFU  1/0:datarate/channel ack
    MCMD_DevStatusAns = 0x06,       // u1:battery 0,1-254,255=?, u1:7-6:RFU,5-0:margin(-32..31)
    MCMD_NewChannelAns = 0x07,      // u1: 7-2=RFU, 1/0:DR/freq ACK
    MCMD_RXTimingSetupAns = 0x08,   // -
    MCMD_TxParamSetupAns = 0x09,    // -
    MCMD_DlChannelAns = 0x0A,       // u1: [7-2]:RFU 1:exists 0:OK
    MCMD_DeviceTimeReq = 0x0D,      // -

    // Class B
    MCMD_PingSlotInfoReq = 0x10,    // u1: 7=RFU, 6-4:interval, 3-0:datarate
    MCMD_PingSlotChannelAns = 0x11, // u1: 7-1:RFU, 0:freq ok
    MCMD_BeaconInfoReq = 0x12,      // - (DEPRECATED)
    MCMD_BeaconFreqAns = 0x13,      // u1: 7-1:RFU, 0:freq ok
};

// MAC downlink commands
enum {
    // Class A
    MCMD_LinkCheckAns = 0x02,       // u1:margin 0-254,255=unknown margin / u1:gwcnt         LinkCheckReq
    MCMD_LinkADRReq = 0x03,         // u1:DR/TXPow, u2:chmask, u1:chpage/repeat
    MCMD_DutyCycleReq = 0x04,       // u1:255 dead [7-4]:RFU, [3-0]:cap 2^-k
    MCMD_RXParamSetupReq = 0x05,    // u1:7-4:RFU/3-0:datarate, u3:freq
    MCMD_DevStatusReq = 0x06,       // -
    MCMD_NewChannelReq = 0x07,      // u1:chidx, u3:freq, u1:DRrange
    MCMD_RXTimingSetupReq = 0x08,   // u1: [7-4]:RFU [3-0]: Delay 1-15s (0 => 1)
    MCMD_TxParamSetupReq = 0x09,    // u1: [7-6]:RFU [5:4]: dl dwell/ul dwell [3:0] max EIRP
    MCMD_DlChannelReq = 0x0A,       // u1: channel, u3: frequency
    MCMD_DeviceTimeAns = 0x0D,      // u4: seconds since epoch, u1: fractional second

    // Class B
    MCMD_PingSlotInfoAns = 0x10,    // -
    MCMD_PingSlotChannelReq = 0x11, // u3: freq, u1:dr [7-4]:RFU [3:0]:datarate
    MCMD_BeaconTimingAns = 0x12,    // u2: delay(in TUNIT millis), u1:channel (DEPRECATED)
    MCMD_BeaconFreqReq = 0x13,      // u3: freq
};

enum {
    MCMD_BeaconTimingAns_TUNIT = 30  // time unit of delay value in millis
};
enum {
    MCMD_LinkADRAns_RFU    = 0xF8,      // RFU bits
    MCMD_LinkADRAns_PowerACK = 0x04,    // 0=not supported power level
    MCMD_LinkADRAns_DataRateACK  = 0x02, // 0=unknown data rate
    MCMD_LinkADRAns_ChannelACK  = 0x01, // 0=unknown channel enabled
};
enum {
    MCMD_RXParamSetupAns_RFU    = 0xF8, // RFU bits
    MCMD_RXParamSetupAns_RX1DrOffsetAck = 0x04, // 0=dr2 not allowed
    MCMD_RXParamSetupAns_RX2DataRateACK  = 0x02, // 0=unknown data rate
    MCMD_RXParamSetupAns_ChannelACK  = 0x01, // 0=unknown channel enabled
};
enum {
    MCMD_NewChannelAns_RFU    = 0xFC, // RFU bits
    MCMD_NewChannelAns_DataRateACK  = 0x02, // 0=unknown data rate
    MCMD_NewChannelAns_ChannelACK  = 0x01, // 0=rejected channel frequency
};
enum {
    MCMD_RXTimingSetupReq_RFU   = 0xF0,     // RFU bits
    MCMD_RXTimingSetupReq_Delay = 0x0F,     // delay in secs, 1..15; 0 is mapped to 1.
};
enum {
    MCMD_DlChannelAns_RFU       = 0xFC,     // RFU bits
    MCMD_DlChannelAns_FreqACK   = 0x02,     // 0 = uplink frequency not defined for this channel
    MCMD_DlChannelAns_ChannelACK = 0x01,    // 0 = rejected channel freq
};
enum {
    MCMD_PingSlotFreqAns_RFU   = 0xFC,
    MCMD_PingSlotFreqAns_DataRateACK = 0x02,
    MCMD_PingSlotFreqAns_ChannelACK = 0x01,
};

enum {
    MCMD_DEVS_EXT_POWER   = 0x00, // external power supply
    MCMD_DEVS_BATT_MIN    = 0x01, // min battery value
    MCMD_DEVS_BATT_MAX    = 0xFE, // max battery value
    MCMD_DEVS_BATT_NOINFO = 0xFF, // unknown battery level
};

// Bit fields byte#3 of MCMD_LinkADRReq payload
enum {
    MCMD_LinkADRReq_Redundancy_RFU            = 0x80,   ///< mask for RFU bit
    MCMD_LinkADRReq_Redundancy_ChMaskCntl_MASK= 0x70,   ///< mask for the channel-mask control field.
    MCMD_LinkADRReq_Redundancy_NbTrans_MASK   = 0x0F,   ///< mask for the `NbTrans` (repetition) field.

    MCMD_LinkADRReq_ChMaskCntl_EULIKE_DIRECT  = 0x00,    ///< EU-like: direct masking for EU
    MCMD_LinkADRReq_ChMaskCntl_EULIKE_ALL_ON  = 0x60,    ///< EU-like: enable everything.

    MCMD_LinkADRReq_ChMaskCntl_USLIKE_500K    = 0x40,    ///< US-like: mask is for the 8 us-like 500 kHz channels
    MCMD_LinkADRReq_ChMaskCntl_USLIKE_SPECIAL = 0x50,    ///< US-like: first special for us-like
    MCMD_LinkADRReq_ChMaskCntl_USLIKE_BANK    = 0x50,    ///< US-like: special: bits are banks.
    MCMD_LinkADRReq_ChMaskCntl_USLIKE_125ON   = 0x60,    ///< US-like: special channel page enable, bits applied to 64..71
    MCMD_LinkADRReq_ChMaskCntl_USLIKE_125OFF  = 0x70,    ///< US-like: special channel page: disable 125K, bits apply to 64..71

    MCMD_LinkADRReq_ChMaskCntl_CN470_ALL_ON   = 0x60,    ///< CN-470: turn all on for China.
};

// Bit fields byte#0 of MCMD_LinkADRReq payload
enum {
    MCMD_LinkADRReq_DR_MASK    = 0xF0,
    MCMD_LinkADRReq_POW_MASK   = 0x0F,
    MCMD_LinkADRReq_DR_SHIFT   = 4,
    MCMD_LinkADRReq_POW_SHIFT  = 0,
};

// bit fields of the TxParam request
enum {
    MCMD_TxParam_RxDWELL_SHIFT   = 5,
    MCMD_TxParam_RxDWELL_MASK    = 1 << MCMD_TxParam_RxDWELL_SHIFT,
    MCMD_TxParam_TxDWELL_SHIFT   = 4,
    MCMD_TxParam_TxDWELL_MASK    = 1 << MCMD_TxParam_TxDWELL_SHIFT,
    MCMD_TxParam_MaxEIRP_SHIFT   = 0,
    MCMD_TxParam_MaxEIRP_MASK    = 0xF << MCMD_TxParam_MaxEIRP_SHIFT,
};

// Device address
typedef u4_t devaddr_t;

// RX quality (device)
enum { RSSI_OFF=64, SNR_SCALEUP=4 };

static inline sf_t  getSf   (rps_t params)            { return   (sf_t)(params &  0x7); }
static inline rps_t setSf   (rps_t params, sf_t sf)   { return (rps_t)((params & ~0x7) | sf); }
static inline bw_t  getBw   (rps_t params)            { return  (bw_t)((params >> 3) & 0x3); }
static inline rps_t setBw   (rps_t params, bw_t cr)   { return (rps_t)((params & ~0x18) | (cr<<3)); }
static inline cr_t  getCr   (rps_t params)            { return  (cr_t)((params >> 5) & 0x3); }
static inline rps_t setCr   (rps_t params, cr_t cr)   { return (rps_t)((params & ~0x60) | (cr<<5)); }
static inline int   getNocrc(rps_t params)            { return        ((params >> 7) & 0x1); }
static inline rps_t setNocrc(rps_t params, int nocrc) { return (rps_t)((params & ~0x80) | (nocrc<<7)); }
static inline int   getIh   (rps_t params)            { return        ((params >> 8) & 0xFF); }
static inline rps_t setIh   (rps_t params, int ih)    { return (rps_t)((params & ~0xFF00) | (ih<<8)); }
static inline rps_t makeRps (sf_t sf, bw_t bw, cr_t cr, int ih, int nocrc) {
    return sf | (bw<<3) | (cr<<5) | (nocrc?(1<<7):0) | ((ih&0xFF)<<8);
}
#define MAKERPS(sf,bw,cr,ih,nocrc) ((rps_t)((sf) | ((bw)<<3) | ((cr)<<5) | ((nocrc)?(1<<7):0) | ((ih&0xFF)<<8)))
// Two frames with params r1/r2 would interfere on air: same SFx + BWx
static inline int sameSfBw(rps_t r1, rps_t r2) { return ((r1^r2)&0x1F) == 0; }
static inline int isFasterDR (dr_t dr1, dr_t dr2) { return dr1 > dr2; }
static inline int isSlowerDR (dr_t dr1, dr_t dr2) { return dr1 < dr2; }

//
// BEG: Keep in sync with lorabase.hpp
// ================================================================================


// Calculate airtime
ostime_t calcAirTime (rps_t rps, u1_t plen);
// Sensitivity at given SF/BW
int getSensitivity (rps_t rps);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _lorabase_h_
