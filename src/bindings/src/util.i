/**
 * @date 2012-02-08
 * @version 0.2
 * @author Dominik Oepen <oepen@informatik.hu-berlin.de>
 */

%{
#include <eac/eac.h>
#include <openssl/err.h>
#include <openssl/objects.h>
#include <stdlib.h>
#include <string.h>

/* included in OpenPACE, but not propagated */
extern BUF_MEM *BUF_MEM_create_init(const void *buf, size_t len);
%}

int
OBJ_txt2nid(char *in);

%inline %{
    /*Dumps a BUF_MEM structure to stdout for debugging purposes */
    void hexdump(const char *title, const BUF_MEM *s) {
        unsigned int n=0;

        if (!s) return;

        fprintf(stdout,"%s",title);
        for(; n < s->length; ++n) {
            if((n%16) == 0)
            fprintf(stdout,"\n    ");
            fprintf(stdout,"%02x:",(unsigned char) s->data[n]);
        }
        fprintf(stdout,"\n");
    }
%}

%inline %{
    void set_tr_version(EAC_CTX *ctx, int version) {
        ctx->tr_version = version;
        return;
    }
%}

%inline %{
    /* Converts a binary string and a length into a BUF_MEM structure */
    BUF_MEM * get_buf(char *in, int in_len) {
        BUF_MEM *buf = NULL;

        if (in_len <= 0)
            return NULL;

        buf = BUF_MEM_create_init(in, (size_t) in_len);

        return buf;
    }
%}

%inline %{
    /* Print the OpenSSL error stack to stdout */
    void print_ossl_err() {
        /* Might be better to load the strings once on program startup */
        ERR_load_crypto_strings();
        ERR_print_errors_fp(stdout);
        ERR_free_strings();
        return;
    }
%}

#ifdef SWIGPYTHON

%inline %{
    void buf2string(BUF_MEM *buf, char **out, int *out_len) {
        if (!buf) { /* Return a NULL pointer on error */
            *out = NULL;
            out_len = 0;
            return;
        } else {
            *out_len = buf->length;
            *out = malloc(*out_len);

            if (!out) {
                *out_len = 0;
                return;
            }

            memcpy(*out, buf->data, *out_len);
            return;
        }
    }
%}

#endif