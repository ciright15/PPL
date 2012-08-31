/*
 * Copyright (c) 2010-2012 Dominik Oepen and Frank Morgner
 *
 * This file is part of OpenPACE.
 *
 * OpenPACE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * OpenPACE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file pace.c
 * @brief OpenPACE implementation
 *
 * @author Frank Morgner <morgner@informatik.hu-berlin.de>
 * @author Dominik Oepen <oepen@informatik.hu-berlin.de>
 */

#include "eac_err.h"
#include "eac_kdf.h"
#include "eac_lib.h"
#include "eac_util.h"
#include "misc.h"
#include "pace_mappings.h"
#include <eac/eac.h>
#include <eac/pace.h>
#include <openssl/crypto.h>
#include <openssl/ecdh.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <string.h>

int
PACE_STEP1_enc_nonce(const EAC_CTX * ctx, const PACE_SEC * pi,
                   BUF_MEM ** enc_nonce)
{
    BUF_MEM * key = NULL;
    int r = 0;

    check((ctx && ctx->pace_ctx && ctx->pace_ctx->ka_ctx &&
                ctx->pace_ctx->ka_ctx->cipher && enc_nonce),
            "Invalid arguments");

    key = kdf_pi(pi, NULL, ctx->pace_ctx->ka_ctx, ctx->md_ctx);
    if (!key)
        goto err;

    BUF_MEM_clear_free(ctx->pace_ctx->nonce);
    ctx->pace_ctx->nonce = randb(EVP_CIPHER_block_size(ctx->pace_ctx->ka_ctx->cipher));
    if (!ctx->pace_ctx->nonce)
        goto err;

    *enc_nonce = cipher_no_pad(ctx->pace_ctx->ka_ctx, ctx->cipher_ctx, key, ctx->pace_ctx->nonce, 1);
    if (!*enc_nonce)
        goto err;

    r = 1;

err:
    BUF_MEM_clear_free(key);

    return r;
}

int
PACE_STEP2_dec_nonce(const EAC_CTX * ctx, const PACE_SEC * pi,
        const BUF_MEM * enc_nonce)
{
    BUF_MEM *key = NULL;
    int r = 0;

    check((ctx && ctx->pace_ctx && ctx->pace_ctx->ka_ctx && ctx->pace_ctx->ka_ctx->cipher),
        "Invalid arguments");

    key = kdf_pi(pi, NULL, ctx->pace_ctx->ka_ctx, ctx->md_ctx);
    if (!key)
        goto err;

    BUF_MEM_clear_free(ctx->pace_ctx->nonce);
    ctx->pace_ctx->nonce = cipher_no_pad(ctx->pace_ctx->ka_ctx, ctx->cipher_ctx, key, enc_nonce, 0);
    if (!ctx->pace_ctx->nonce)
        goto err;

    r = 1;

err:
    BUF_MEM_clear_free(key);

    return r;
}

BUF_MEM *
PACE_STEP3A_generate_mapping_data(const EAC_CTX * ctx)
{
    check_return((ctx && ctx->pace_ctx && ctx->pace_ctx->map_generate_key), "Invalid arguments");

    return ctx->pace_ctx->map_generate_key(ctx->pace_ctx, ctx->bn_ctx);
}

int
PACE_STEP3A_map_generator(const EAC_CTX * ctx, const BUF_MEM * in)
{
    if(!ctx || !ctx->pace_ctx && !ctx->pace_ctx->map_compute_key) {
        log_err("Invalid arguments");
        return 0;
    }

    return ctx->pace_ctx->map_compute_key(ctx->pace_ctx, ctx->pace_ctx->nonce, in, ctx->bn_ctx);
}

BUF_MEM *
PACE_STEP3B_generate_ephemeral_key(EAC_CTX * ctx)
{
    check_return((ctx && ctx->pace_ctx), "Invalid arguments");

    ctx->pace_ctx->my_eph_pubkey = KA_CTX_generate_key(ctx->pace_ctx->ka_ctx,
            ctx->bn_ctx);

    if (!ctx->pace_ctx->my_eph_pubkey)
        return NULL;

    return BUF_MEM_create_init(ctx->pace_ctx->my_eph_pubkey->data,
            ctx->pace_ctx->my_eph_pubkey->length);
}

int
PACE_STEP3B_compute_shared_secret(const EAC_CTX * ctx, const BUF_MEM * in)
{
    BUF_MEM *my_pub = NULL;
    int r = 0;

    check((ctx  && ctx->pace_ctx  && ctx->pace_ctx->ka_ctx
            && ctx->pace_ctx->ka_ctx->compute_key && in), "Invalid arguments");

    /* Check if the new public key is different from the other party's public
     * key.  Note that this check is only required since TR-03110 v2.02, but it
     * makes sense to always check this... */
    if (!ctx->pace_ctx->my_eph_pubkey
               || (in->length == ctx->pace_ctx->my_eph_pubkey->length
                && memcmp(in->data, ctx->pace_ctx->my_eph_pubkey, in->length) == 0)) {
        log_err("Bad DH or ECKEY object");
        goto err;
    }


    if (!KA_CTX_compute_key(ctx->pace_ctx->ka_ctx, in, ctx->bn_ctx))
        goto err;

    r = 1;

err:
    if (my_pub)
        BUF_MEM_free(my_pub);

    return r;
}

int
PACE_STEP3C_derive_keys(const EAC_CTX *ctx)
{
    if (!ctx || !ctx->pace_ctx) {
        log_err("Invalid arguments");
        return 0;
    }

    return KA_CTX_derive_keys(ctx->pace_ctx->ka_ctx, NULL, ctx->md_ctx);
}

BUF_MEM *
PACE_STEP3D_compute_authentication_token(const EAC_CTX *ctx, const BUF_MEM *pub)
{
    if (!ctx || !ctx->pace_ctx) {
        log_err("Invalid arguments");
        return NULL;
    }

    return get_authentication_token(ctx->pace_ctx->protocol,
            ctx->pace_ctx->ka_ctx, ctx->bn_ctx,
            ctx->tr_version, pub);
}

int
PACE_STEP3D_verify_authentication_token(const EAC_CTX *ctx, const BUF_MEM *token)
{
    if (!ctx || !token|| !ctx->pace_ctx) {
        log_err("Invalid arguments");
        return -1;
    }

    return verify_authentication_token(ctx->pace_ctx->protocol,
            ctx->pace_ctx->ka_ctx, ctx->bn_ctx, ctx->tr_version,
            token);
}