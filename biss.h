/*****************************************************************************
 * biss.h: BISS-1 descrambling support
 *****************************************************************************
 * Copyright (C) 2025 VideoLAN
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef _DVBLAST_BISS_H_
#define _DVBLAST_BISS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef HAVE_DVBCSA

#include <dvbcsa/dvbcsa.h>

typedef struct biss_key_t
{
    struct dvbcsa_key_s *p_even_key;
    struct dvbcsa_key_s *p_odd_key;
    bool b_valid;
} biss_key_t;

bool biss_ParseKey( const char *psz_hex, uint8_t pi_cw[8] );
void biss_Init( biss_key_t *p_key, const uint8_t pi_cw[8] );
void biss_Destroy( biss_key_t *p_key );
void biss_Descramble( biss_key_t *p_key, uint8_t *p_ts );

#else /* !HAVE_DVBCSA */

typedef struct biss_key_t
{
    bool b_valid;
} biss_key_t;

static inline bool biss_ParseKey( const char *psz_hex, uint8_t pi_cw[8] )
{
    (void)psz_hex; (void)pi_cw;
    return false;
}
static inline void biss_Init( biss_key_t *p_key, const uint8_t pi_cw[8] )
{
    (void)p_key; (void)pi_cw;
    p_key->b_valid = false;
}
static inline void biss_Destroy( biss_key_t *p_key )
{
    (void)p_key;
    p_key->b_valid = false;
}
static inline void biss_Descramble( biss_key_t *p_key, uint8_t *p_ts )
{
    (void)p_key; (void)p_ts;
}

#endif /* HAVE_DVBCSA */

#endif /* _DVBLAST_BISS_H_ */
