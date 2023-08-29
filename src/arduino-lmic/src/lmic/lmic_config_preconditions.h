/* lmic_config_preconditions.h	Fri May 19 2017 23:58:34 tmm */

/*

Module:  lmic_config_preconditions.h

Function:
	Preconditions for LMIC configuration.

Version:
	V2.0.0	Sun Aug 06 2017 17:40:44 tmm	Edit level 1

Copyright notice:
	This file copyright (C) 2017 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

        MIT License

        Permission is hereby granted, free of charge, to any person obtaining a copy
        of this software and associated documentation files (the "Software"), to deal
        in the Software without restriction, including without limitation the rights
        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
        furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included in all
        copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
        OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
        SOFTWARE.

Author:
	Terry Moore, MCCI Corporation	July 2017

Revision history:
   2.0.0  Sun Aug 06 2017 17:40:44  tmm
	Module created.

*/

#ifndef _LMIC_CONFIG_PRECONDITIONS_H_
# define _LMIC_CONFIG_PRECONDITIONS_H_

// We need to be able to compile with different options without editing source.
// When building with a more advanced environment, set the following variable:
// ARDUINO_LMIC_PROJECT_CONFIG_H=my_project_config.h
//
// otherwise the lmic_project_config.h from the ../../project_config directory will be used.
#ifndef ARDUINO_LMIC_PROJECT_CONFIG_H
# define ARDUINO_LMIC_PROJECT_CONFIG_H ../../project_config/lmic_project_config.h
#endif

#define CFG_TEXT_1(x)	CFG_TEXT_2(x)
#define CFG_TEXT_2(x)	#x

// constants for comparison
#define LMIC_REGION_eu868    1
#define LMIC_REGION_us915    2
#define LMIC_REGION_cn783    3
#define LMIC_REGION_eu433    4
#define LMIC_REGION_au915    5
#define LMIC_REGION_cn490    6
#define LMIC_REGION_as923    7
#define LMIC_REGION_kr920    8
#define LMIC_REGION_in866    9

// Some regions have country-specific overrides. For generality, we specify
// country codes using the LMIC_COUNTY_CODE_C() macro These values are chosen
// from the 2-letter domain suffixes standardized by ISO-3166-1 alpha2 (see
// https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). They are therefore
// 16-bit constants. By convention, we use UPPER-CASE letters, thus
// LMIC_COUNTRY_CODE('J', 'P'), not ('j', 'p').
#define LMIC_COUNTRY_CODE_C(c1, c2)     ((c1) * 256 + (c2))

// this special code means "no country code defined"
#define LMIC_COUNTRY_CODE_NONE  0

// specific countries. Only the ones that are needed by the code are defined.
#define LMIC_COUNTRY_CODE_JP    LMIC_COUNTRY_CODE_C('J', 'P')

// include the file that the user is really supposed to edit. But for really strange
// ports, this can be suppressed
#ifndef ARDUINO_LMIC_PROJECT_CONFIG_H_SUPPRESS
# include CFG_TEXT_1(ARDUINO_LMIC_PROJECT_CONFIG_H)
#endif /* ARDUINO_LMIC_PROJECT_CONFIG_H_SUPPRESS */

#if defined(CFG_au921) && !defined(CFG_au915)
#   warning "CFG_au921 was deprecated in favour of CFG_au915. Support for CFG_au921 might be removed in the future."
#   define CFG_au915
#endif

// for backwards compatibility to legacy code, define CFG_au921 if we see CFG_au915.
#if defined(CFG_au915) && !defined(CFG_au921)
#   define CFG_au921
#endif

// a mask of the supported regions
// TODO(tmm@mcci.com) consider moving this block to a central file as it's not
// user-editable.
#define LMIC_REGIONS_SUPPORTED  (                               \
                                (1 << LMIC_REGION_eu868) |      \
                                (1 << LMIC_REGION_us915) |      \
                             /* (1 << LMIC_REGION_cn783) | */   \
                             /* (1 << LMIC_REGION_eu433) | */   \
                                (1 << LMIC_REGION_au915) |      \
                             /* (1 << LMIC_REGION_cn490) | */   \
                                (1 << LMIC_REGION_as923) |      \
                                (1 << LMIC_REGION_kr920) |      \
                                (1 << LMIC_REGION_in866) |      \
                                0)

// the selected region.
// TODO(tmm@mcci.com) consider moving this block to a central file as it's not
// user-editable.
#if defined(CFG_eu868)
# define CFG_region     LMIC_REGION_eu868
#elif defined(CFG_us915)
# define CFG_region     LMIC_REGION_us915
#elif defined(CFG_cn783)
# define CFG_region     LMIC_REGION_cn783
#elif defined(CFG_eu433)
# define CFG_region     LMIC_REGION_eu433
#elif defined(CFG_au915)
# define CFG_region     LMIC_REGION_au915
#elif defined(CFG_cn490)
# define CFG_region     LMIC_REGION_cn490
#elif defined(CFG_as923jp)
# define CFG_as923	1			/* CFG_as923jp implies CFG_as923 */
# define CFG_region     LMIC_REGION_as923
# define LMIC_COUNTRY_CODE  LMIC_COUNTRY_CODE_JP
#elif defined(CFG_as923)
# define CFG_region     LMIC_REGION_as923
#elif defined(CFG_kr920)
# define CFG_region     LMIC_REGION_kr920
#elif defined(CFG_in866)
# define CFG_region     LMIC_REGION_in866
#else
# define CFG_region     0
#endif

// LMIC_CFG_*_ENA -- either 1 or 0 based on whether that region
// is enabled.  Note: these must be after the code that special-cases
// CFG_as923jp.
#if defined(CFG_eu868)
# define LMIC_CFG_eu868_ENA  1
#else
# define LMIC_CFG_eu868_ENA  0
#endif

#if defined(CFG_us915)
# define LMIC_CFG_us915_ENA  1
#else
# define LMIC_CFG_us915_ENA  0
#endif

#if defined(CFG_cn783)
# define LMIC_CFG_cn783_ENA  1
#else
# define LMIC_CFG_cn783_ENA  0
#endif

#if defined(CFG_eu433)
# define LMIC_CFG_eu433_ENA  1
#else
# define LMIC_CFG_eu433_ENA  0
#endif

#if defined(CFG_au915)
# define LMIC_CFG_au915_ENA  1
#else
# define LMIC_CFG_au915_ENA  0
#endif

#if defined(CFG_cn490)
# define LMIC_CFG_cn490_ENA  1
#else
# define LMIC_CFG_cn490_ENA  0
#endif

#if defined(CFG_as923)
# define LMIC_CFG_as923_ENA  1
#else
# define LMIC_CFG_as923_ENA  0
#endif

#if defined(CFG_kr920)
# define LMIC_CFG_kr920_ENA  1
#else
# define LMIC_CFG_kr920_ENA  0
#endif

#if defined(CFG_in866)
# define LMIC_CFG_in866_ENA  1
#else
# define LMIC_CFG_in866_ENA  0
#endif

/// \brief Bitmask of configured regions
///
/// Our input is a -D of one of CFG_eu868, CFG_us915, CFG_as923, CFG_au915, CFG_in866
/// More will be added in the the future. So at this point we create CFG_region with
/// following values. These are in order of the sections in the manual. Not all of the
/// below are supported yet.
///
/// CFG_as923jp is treated as a special case of CFG_as923, so it's not included in
/// the below.
///
/// TODO(tmm@mcci.com) consider moving this block to a central file as it's not
/// user-editable.
///
# define CFG_LMIC_REGION_MASK                                         \
                        ((LMIC_CFG_eu868_ENA << LMIC_REGION_eu868) | \
                         (LMIC_CFG_us915_ENA << LMIC_REGION_us915) | \
                         (LMIC_CFG_cn783_ENA << LMIC_REGION_cn783) | \
                         (LMIC_CFG_eu433_ENA << LMIC_REGION_eu433) | \
                         (LMIC_CFG_au915_ENA << LMIC_REGION_au915) | \
                         (LMIC_CFG_cn490_ENA << LMIC_REGION_cn490) | \
                         (LMIC_CFG_as923_ENA << LMIC_REGION_as923) | \
                         (LMIC_CFG_kr920_ENA << LMIC_REGION_kr920) | \
                         (LMIC_CFG_in866_ENA << LMIC_REGION_in866) | \
                         0)

/// \brief a bitmask of EU-like regions
///
/// EU-like regions have up to 16 channels individually programmable via downlink.
///
/// TODO(tmm@mcci.com) consider moving this block to a central file as it's not
/// user-editable.
#define CFG_LMIC_EU_like_MASK   (                               \
                                (1 << LMIC_REGION_eu868) |      \
                             /* (1 << LMIC_REGION_us915) | */   \
                                (1 << LMIC_REGION_cn783) |      \
                                (1 << LMIC_REGION_eu433) |      \
                             /* (1 << LMIC_REGION_au915) | */   \
                             /* (1 << LMIC_REGION_cn490) | */   \
                                (1 << LMIC_REGION_as923) |      \
                                (1 << LMIC_REGION_kr920) |      \
                                (1 << LMIC_REGION_in866) |      \
                                0)

/// \brief bitmask of` US-like regions
///
/// US-like regions have 64 fixed 125-kHz channels overlaid by 8 500-kHz
/// channels. The channel frequencies can't be changed, but
/// subsets of channels can be selected via masks.
///
/// TODO(tmm@mcci.com) consider moving this block to a central file as it's not
/// user-editable.
#define CFG_LMIC_US_like_MASK   (                               \
                             /* (1 << LMIC_REGION_eu868) | */   \
                                (1 << LMIC_REGION_us915) |      \
                             /* (1 << LMIC_REGION_cn783) | */   \
                             /* (1 << LMIC_REGION_eu433) | */   \
                                (1 << LMIC_REGION_au915) |      \
                             /* (1 << LMIC_REGION_cn490) | */   \
                             /* (1 << LMIC_REGION_as923) | */   \
                             /* (1 << LMIC_REGION_kr920) | */   \
                             /* (1 << LMIC_REGION_in866) | */   \
                                0)

//
// booleans that are true if the configured region is EU-like or US-like.
// TODO(tmm@mcci.com) consider moving this block to a central file as it's not
// user-editable.
//

/// \brief true if configured region is EU-like, false otherwise.
#define CFG_LMIC_EU_like        (!!(CFG_LMIC_REGION_MASK & CFG_LMIC_EU_like_MASK))
/// \brief true if configured region is US-like, false otherwise.
#define CFG_LMIC_US_like        (!!(CFG_LMIC_REGION_MASK & CFG_LMIC_US_like_MASK))

//
// The supported LMIC LoRaWAN spec versions. These need to be numerically ordered,
// so that we can (for example) compare
//
//    LMIC_LORAWAN_SPEC_VERSION < LMIC_LORAWAN_SPEC_VERSION_1_0_3.
//
#define LMIC_LORAWAN_SPEC_VERSION_1_0_2  0x01000200u  /// Refers to LoRaWAN spec 1.0.2
#define LMIC_LORAWAN_SPEC_VERSION_1_0_3  0x01000300u  /// Refers to LoRaWAN spec 1.0.3

#endif /* _LMIC_CONFIG_PRECONDITIONS_H_ */
