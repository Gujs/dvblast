/*****************************************************************************
 * biss.c: BISS-1 descrambling support
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

#ifdef HAVE_DVBCSA

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <dvbcsa/dvbcsa.h>
#include <bitstream/mpeg/ts.h>

#include "biss.h"

/*****************************************************************************
 * biss_ParseKey : parse 12 hex digit BISS-1 key into 8-byte control word
 *
 * BISS-1 key is 6 bytes (12 hex digits). Expanded to 8 bytes:
 *   CW[0..2] = key[0..2]
 *   CW[3]    = (key[0] + key[1] + key[2]) mod 256  (checksum)
 *   CW[4..6] = key[3..5]
 *   CW[7]    = (key[3] + key[4] + key[5]) mod 256  (checksum)
 *****************************************************************************/
bool biss_ParseKey( const char *psz_hex, uint8_t pi_cw[8] )
{
    uint8_t pi_key[6];
    int i;

    if ( strlen(psz_hex) != 12 )
        return false;

    for ( i = 0; i < 12; i++ )
    {
        if ( !isxdigit( (unsigned char)psz_hex[i] ) )
            return false;
    }

    for ( i = 0; i < 6; i++ )
    {
        char hex[3] = { psz_hex[i*2], psz_hex[i*2+1], '\0' };
        pi_key[i] = (uint8_t)strtoul( hex, NULL, 16 );
    }

    pi_cw[0] = pi_key[0];
    pi_cw[1] = pi_key[1];
    pi_cw[2] = pi_key[2];
    pi_cw[3] = (pi_key[0] + pi_key[1] + pi_key[2]) & 0xFF;
    pi_cw[4] = pi_key[3];
    pi_cw[5] = pi_key[4];
    pi_cw[6] = pi_key[5];
    pi_cw[7] = (pi_key[3] + pi_key[4] + pi_key[5]) & 0xFF;

    return true;
}

/*****************************************************************************
 * biss_Init : allocate and set DVB-CSA keys (BISS-1 uses same key for
 *             both even and odd, no key rotation)
 *****************************************************************************/
void biss_Init( biss_key_t *p_key, const uint8_t pi_cw[8] )
{
    p_key->p_even_key = dvbcsa_key_alloc();
    p_key->p_odd_key = dvbcsa_key_alloc();

    if ( p_key->p_even_key == NULL || p_key->p_odd_key == NULL )
    {
        if ( p_key->p_even_key ) dvbcsa_key_free( p_key->p_even_key );
        if ( p_key->p_odd_key ) dvbcsa_key_free( p_key->p_odd_key );
        p_key->p_even_key = NULL;
        p_key->p_odd_key = NULL;
        p_key->b_valid = false;
        return;
    }

    dvbcsa_key_set( pi_cw, p_key->p_even_key );
    dvbcsa_key_set( pi_cw, p_key->p_odd_key );
    p_key->b_valid = true;
}

/*****************************************************************************
 * biss_Destroy : free DVB-CSA keys
 *****************************************************************************/
void biss_Destroy( biss_key_t *p_key )
{
    if ( p_key->p_even_key )
    {
        dvbcsa_key_free( p_key->p_even_key );
        p_key->p_even_key = NULL;
    }
    if ( p_key->p_odd_key )
    {
        dvbcsa_key_free( p_key->p_odd_key );
        p_key->p_odd_key = NULL;
    }
    p_key->b_valid = false;
}

/*****************************************************************************
 * biss_Descramble : descramble a single TS packet in-place
 *
 * Checks scrambling control bits, decrypts payload using the appropriate
 * key (even/odd), then clears the scrambling bits.
 *****************************************************************************/
void biss_Descramble( biss_key_t *p_key, uint8_t *p_ts )
{
    uint8_t i_scrambling;
    struct dvbcsa_key_s *p_dvbcsa_key;
    uint8_t *p_payload;
    int i_payload_len;

    if ( !p_key->b_valid )
        return;

    i_scrambling = ts_get_scrambling( p_ts );
    if ( i_scrambling == 0 )
        return; /* not scrambled */

    if ( i_scrambling == 2 )
        p_dvbcsa_key = p_key->p_even_key;
    else if ( i_scrambling == 3 )
        p_dvbcsa_key = p_key->p_odd_key;
    else
        return; /* reserved value */

    p_payload = ts_payload( p_ts );
    if ( p_payload >= p_ts + TS_SIZE )
        return; /* no payload */

    i_payload_len = p_ts + TS_SIZE - p_payload;
    if ( i_payload_len < 8 )
        return; /* DVB-CSA needs at least 8 bytes */

    dvbcsa_decrypt( p_dvbcsa_key, p_payload, i_payload_len );

    /* Clear scrambling bits - packet is now in the clear */
    ts_set_scrambling( p_ts, 0 );
}

#endif /* HAVE_DVBCSA */
