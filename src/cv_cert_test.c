/*
 * Copyright (c) 2010-2012 Dominik Oepen and Frank Morgner
 *
 * This file is part of OpenPACE.
 *
 * OpenPACE is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * OpenPACE is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * OpenPACE.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file cv_cert_test.c
 * @brief Tests for the Card Verifiable Certificate functions and data
 *        structures
 *
 * @author Dominik Oepen <oepen@informatik.hu-berlin.de>
 * @author Frank Morgner <morgner@informatik.hu-berlin.de>
 */

#include <eac/cv_cert.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

#define err(s) { printf(s "\n"); ERR_print_errors_fp(stdout); goto err; }
#define verb(s) { if (verbose) printf(s "\n"); else printf("."); }

static char verbose=0;
static char debug=0;

/* http://www.internet-sicherheit.de/forschung/aktuelle-forschungsprojekte/elektronischer-personalausweis/berechtigungszertifikat/ */
/*static unsigned const char CVCert[] = {
    0x7f, 0x21, 0x82, 0x01, 0x89, 0x7f, 0x4e, 0x82, 0x01, 0x49, 0x5f,
    0x29, 0x01, 0x00, 0x42, 0x0e, 0x44, 0x45, 0x43, 0x56, 0x43, 0x41,
    0x45, 0x50, 0x41, 0x53, 0x53, 0x30, 0x30, 0x31, 0x7f, 0x49, 0x81,
    0xfd, 0x06, 0x0a, 0x04, 0x00, 0x7f, 0x00, 0x07, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x81, 0x1c, 0xd7, 0xc1, 0x34, 0xaa, 0x26, 0x43, 0x66,
    0x86, 0x2a, 0x18, 0x30, 0x25, 0x75, 0xd1, 0xd7, 0x87, 0xb0, 0x9f,
    0x07, 0x57, 0x97, 0xda, 0x89, 0xf5, 0x7e, 0xc8, 0xc0, 0xff, 0x82,
    0x1c, 0x68, 0xa5, 0xe6, 0x2c, 0xa9, 0xce, 0x6c, 0x1c, 0x29, 0x98,
    0x03, 0xa6, 0xc1, 0x53, 0x0b, 0x51, 0x4e, 0x18, 0x2a, 0xd8, 0xb0,
    0x04, 0x2a, 0x59, 0xca, 0xd2, 0x9f, 0x43, 0x83, 0x1c, 0x25, 0x80,
    0xf6, 0x3c, 0xcf, 0xe4, 0x41, 0x38, 0x87, 0x07, 0x13, 0xb1, 0xa9,
    0x23, 0x69, 0xe3, 0x3e, 0x21, 0x35, 0xd2, 0x66, 0xdb, 0xb3, 0x72,
    0x38, 0x6c, 0x40, 0x0b, 0x84, 0x39, 0x04, 0x0d, 0x90, 0x29, 0xad,
    0x2c, 0x7e, 0x5c, 0xf4, 0x34, 0x08, 0x23, 0xb2, 0xa8, 0x7d, 0xc6,
    0x8c, 0x9e, 0x4c, 0xe3, 0x17, 0x4c, 0x1e, 0x6e, 0xfd, 0xee, 0x12,
    0xc0, 0x7d, 0x58, 0xaa, 0x56, 0xf7, 0x72, 0xc0, 0x72, 0x6f, 0x24,
    0xc6, 0xb8, 0x9e, 0x4e, 0xcd, 0xac, 0x24, 0x35, 0x4b, 0x9e, 0x99,
    0xca, 0xa3, 0xf6, 0xd3, 0x76, 0x14, 0x02, 0xcd, 0x85, 0x1c, 0xd7,
    0xc1, 0x34, 0xaa, 0x26, 0x43, 0x66, 0x86, 0x2a, 0x18, 0x30, 0x25,
    0x75, 0xd0, 0xfb, 0x98, 0xd1, 0x16, 0xbc, 0x4b, 0x6d, 0xde, 0xbc,
    0xa3, 0xa5, 0xa7, 0x93, 0x9f, 0x86, 0x39, 0x04, 0x39, 0x3e, 0xe8,
    0xe0, 0x6d, 0xb6, 0xc7, 0xf5, 0x28, 0xf8, 0xb4, 0x26, 0x0b, 0x49,
    0xaa, 0x93, 0x30, 0x98, 0x24, 0xd9, 0x2c, 0xdb, 0x18, 0x07, 0xe5,
    0x43, 0x7e, 0xe2, 0xe2, 0x6e, 0x29, 0xb7, 0x3a, 0x71, 0x11, 0x53,
    0x0f, 0xa8, 0x6b, 0x35, 0x00, 0x37, 0xcb, 0x94, 0x15, 0xe1, 0x53,
    0x70, 0x43, 0x94, 0x46, 0x37, 0x97, 0x13, 0x9e, 0x14, 0x87, 0x01,
    0x01, 0x5f, 0x20, 0x0e, 0x44, 0x45, 0x43, 0x56, 0x43, 0x41, 0x45,
    0x50, 0x41, 0x53, 0x53, 0x30, 0x30, 0x31, 0x7f, 0x4c, 0x0e, 0x06,
    0x09, 0x04, 0x00, 0x7f, 0x00, 0x07, 0x03, 0x01, 0x02, 0x01, 0x53,
    0x01, 0xc1, 0x5f, 0x25, 0x06, 0x00, 0x07, 0x00, 0x04, 0x00, 0x01,
    0x5f, 0x24, 0x06, 0x00, 0x09, 0x00, 0x04, 0x00, 0x01, 0x5f, 0x37,
    0x38, 0x4c, 0xcf, 0x25, 0xc5, 0x9f, 0x36, 0x12, 0xee, 0xe1, 0x88,
    0x75, 0xf6, 0xc5, 0xf2, 0xe2, 0xd2, 0x1f, 0x03, 0x95, 0x68, 0x3b,
    0x53, 0x2a, 0x26, 0xe4, 0xc1, 0x89, 0xb7, 0x1e, 0xfe, 0x65, 0x9c,
    0x3f, 0x26, 0xe0, 0xeb, 0x9a, 0xea, 0xe9, 0x98, 0x63, 0x10, 0x7f,
    0x9b, 0x0d, 0xad, 0xa1, 0x64, 0x14, 0xff, 0xa2, 0x04, 0x51, 0x6a,
    0xee, 0x2b
};*/

static const unsigned char CVCert[] = {
    0x7F, 0x21, 0x82, 0x01, 0x41, 0x7F, 0x4E, 0x81, 0xFA, 0x5F, 0x29,
    0x01, 0x00, 0x42, 0x0D, 0x5A, 0x5A, 0x44, 0x56, 0x43, 0x41, 0x41,
    0x54, 0x41, 0x30, 0x30, 0x30, 0x33, 0x7F, 0x49, 0x4F, 0x06, 0x0A,
    0x04, 0x00, 0x7F, 0x00, 0x07, 0x02, 0x02, 0x02, 0x02, 0x03, 0x86,
    0x41, 0x04, 0x19, 0xD1, 0x75, 0x45, 0xD3, 0xFE, 0x0B, 0x34, 0x3E,
    0x7E, 0xE2, 0xAE, 0x4E, 0x2B, 0xC9, 0x2D, 0x51, 0x35, 0x1C, 0xC1,
    0x17, 0xA4, 0x7F, 0xA9, 0x51, 0x9A, 0xDB, 0x1E, 0x40, 0x5E, 0xE6,
    0xB8, 0x12, 0x12, 0x80, 0xBC, 0xC2, 0xFF, 0xF0, 0x35, 0x7A, 0x19,
    0x7D, 0xE7, 0x39, 0xA7, 0xFD, 0x2E, 0xF0, 0x22, 0x10, 0xEF, 0x34,
    0x3C, 0xDB, 0xE7, 0x9E, 0xF9, 0x4B, 0x8E, 0x28, 0x59, 0x1B, 0xB9,
    0x5F, 0x20, 0x0B, 0x5A, 0x5A, 0x44, 0x4B, 0x42, 0x32, 0x30, 0x30,
    0x30, 0x30, 0x52, 0x7F, 0x4C, 0x12, 0x06, 0x09, 0x04, 0x00, 0x7F,
    0x00, 0x07, 0x03, 0x01, 0x02, 0x02, 0x53, 0x05, 0x00, 0x03, 0x01,
    0xDF, 0x04, 0x5F, 0x25, 0x06, 0x01, 0x00, 0x00, 0x02, 0x01, 0x07,
    0x5F, 0x24, 0x06, 0x01, 0x00, 0x00, 0x03, 0x03, 0x01, 0x65, 0x5E,
    0x73, 0x2D, 0x06, 0x09, 0x04, 0x00, 0x7F, 0x00, 0x07, 0x03, 0x01,
    0x03, 0x01, 0x80, 0x20, 0x75, 0xE0, 0xC4, 0xAC, 0x36, 0xC2, 0x5A,
    0x33, 0xAC, 0x0E, 0x9A, 0x75, 0xEB, 0x79, 0x2A, 0x72, 0xF3, 0x31,
    0xA5, 0x1E, 0x28, 0x63, 0x4E, 0xCC, 0x2E, 0xD6, 0x2E, 0x54, 0xF3,
    0xC6, 0x93, 0xDA, 0x73, 0x2D, 0x06, 0x09, 0x04, 0x00, 0x7F, 0x00,
    0x07, 0x03, 0x01, 0x03, 0x02, 0x80, 0x20, 0x18, 0x12, 0x65, 0x74,
    0x49, 0xFC, 0xF1, 0xD3, 0xDA, 0xD8, 0x3D, 0x13, 0x14, 0x29, 0x17,
    0x5C, 0x61, 0x8B, 0x21, 0xBA, 0xF0, 0xAF, 0x44, 0xAC, 0xE3, 0x8C,
    0xB2, 0xC1, 0x2C, 0xEB, 0x2A, 0x56, 0x5F, 0x37, 0x40, 0x54, 0x0F,
    0x85, 0x09, 0x12, 0xAB, 0xD3, 0x51, 0xF8, 0xF5, 0x56, 0x9B, 0x53,
    0x4A, 0x5C, 0x8F, 0x64, 0x54, 0x5B, 0x51, 0xA7, 0x34, 0x70, 0xBE,
    0x5A, 0xD2, 0x89, 0xC1, 0x9A, 0x5E, 0x13, 0x52, 0x53, 0xD3, 0xBB,
    0x15, 0x52, 0x26, 0x21, 0x7B, 0x41, 0xE7, 0xF0, 0x68, 0xB3, 0x52,
    0x3F, 0x3A, 0x63, 0x92, 0x22, 0xAF, 0x2B, 0x62, 0x8C, 0x39, 0x7D,
    0x4F, 0xD4, 0x02, 0x1E, 0xDE, 0x00, 0xDC
};

/*static const unsigned int CVCert_len = 398;*/
static const unsigned int CVCert_len = 326;

static const unsigned char certDesc[] = {
    0x30, 0x82, 0x01, 0x90, 0x06, 0x0A, 0x04, 0x00, 0x7F, 0x00, 0x07, 0x03,
    0x01, 0x03, 0x01, 0x01, 0xA1, 0x16, 0x0C, 0x14, 0x42, 0x75, 0x6E, 0x64,
    0x65, 0x73, 0x64, 0x72, 0x75, 0x63, 0x6B, 0x65, 0x72, 0x65, 0x69, 0x20,
    0x47, 0x6D, 0x62, 0x48, 0xA2, 0x24, 0x13, 0x22, 0x68, 0x74, 0x74, 0x70,
    0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x62, 0x75, 0x6E, 0x64, 0x65,
    0x73, 0x64, 0x72, 0x75, 0x63, 0x6B, 0x65, 0x72, 0x65, 0x69, 0x2E, 0x64,
    0x65, 0x2F, 0x64, 0x76, 0x63, 0x61, 0xA3, 0x18, 0x0C, 0x16, 0x44, 0x65,
    0x75, 0x74, 0x73, 0x63, 0x68, 0x65, 0x20, 0x4B, 0x72, 0x65, 0x64, 0x69,
    0x74, 0x62, 0x61, 0x6E, 0x6B, 0x20, 0x41, 0x47, 0xA4, 0x13, 0x13, 0x11,
    0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x64,
    0x6B, 0x62, 0x2E, 0x64, 0x65, 0xA5, 0x82, 0x01, 0x13, 0x0C, 0x82, 0x01,
    0x0F, 0x54, 0x61, 0x75, 0x62, 0x65, 0x6E, 0x73, 0x74, 0x72, 0x2E, 0x20,
    0x37, 0x2D, 0x39, 0x0D, 0x0A, 0x31, 0x30, 0x31, 0x31, 0x37, 0x20, 0x42,
    0x65, 0x72, 0x6C, 0x69, 0x6E, 0x0D, 0x0A, 0x69, 0x6E, 0x66, 0x6F, 0x40,
    0x64, 0x6B, 0x62, 0x2E, 0x64, 0x65, 0x0D, 0x0A, 0x45, 0x72, 0xC3, 0xB6,
    0x66, 0x66, 0x6E, 0x75, 0x6E, 0x67, 0x20, 0x65, 0x69, 0x6E, 0x65, 0x73,
    0x20, 0x4B, 0x6F, 0x6E, 0x74, 0x6F, 0x73, 0x0D, 0x0A, 0x42, 0x65, 0x72,
    0x6C, 0x69, 0x6E, 0x65, 0x72, 0x20, 0x42, 0x65, 0x61, 0x75, 0x66, 0x74,
    0x72, 0x61, 0x67, 0x74, 0x65, 0x72, 0x20, 0x66, 0xC3, 0xBC, 0x72, 0x20,
    0x44, 0x61, 0x74, 0x65, 0x6E, 0x73, 0x63, 0x68, 0x75, 0x74, 0x7A, 0x20,
    0x75, 0x6E, 0x64, 0x20, 0x49, 0x6E, 0x66, 0x6F, 0x72, 0x6D, 0x61, 0x74,
    0x69, 0x6F, 0x6E, 0x73, 0x66, 0x72, 0x65, 0x69, 0x68, 0x65, 0x69, 0x74,
    0x2C, 0x20, 0x41, 0x6E, 0x20, 0x64, 0x65, 0x72, 0x20, 0x55, 0x72, 0x61,
    0x6E, 0x69, 0x61, 0x20, 0x34, 0x2D, 0x31, 0x30, 0x2C, 0x20, 0x31, 0x30,
    0x37, 0x38, 0x37, 0x20, 0x42, 0x65, 0x72, 0x6C, 0x69, 0x6E, 0x2C, 0x20,
    0x30, 0x33, 0x30, 0x2F, 0x31, 0x33, 0x20, 0x38, 0x38, 0x39, 0x2D, 0x30,
    0x2C, 0x20, 0x6D, 0x61, 0x69, 0x6C, 0x62, 0x6F, 0x78, 0x40, 0x64, 0x61,
    0x74, 0x65, 0x6E, 0x73, 0x63, 0x68, 0x75, 0x74, 0x7A, 0x2D, 0x62, 0x65,
    0x72, 0x6C, 0x69, 0x6E, 0x2E, 0x64, 0x65, 0x2C, 0x20, 0x68, 0x74, 0x74,
    0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x64, 0x61, 0x74, 0x65,
    0x6E, 0x73, 0x63, 0x68, 0x75, 0x74, 0x7A, 0x2D, 0x62, 0x65, 0x72, 0x6C,
    0x69, 0x6E, 0x2E, 0x64, 0x65, 0x0D, 0x0A, 0x45, 0x72, 0xC3, 0xB6, 0x66,
    0x66, 0x6E, 0x75, 0x6E, 0x67, 0x20, 0x65, 0x69, 0x6E, 0x65, 0x73, 0x20,
    0x4B, 0x6F, 0x6E, 0x74, 0x6F, 0x73, 0x0D, 0x0A
};

static const unsigned int certDesc_len = 404;

static int parse_cv_cert(const unsigned char *cert, const unsigned int cert_len,
        const unsigned char *cert_desc, const unsigned int cert_desc_len) {
    BIO *bio_stdout = NULL;
    CVC_CERT *cvc_cert = NULL;
    CVC_CERTIFICATE_DESCRIPTION *desc = NULL;
    const unsigned char *p;
    int fail = 1;

    printf("Test CV certificate parsing ");
    if (verbose)
        printf("\n");

    bio_stdout = BIO_new_fp(stdout, BIO_NOCLOSE);
    if (!bio_stdout)
        err("Could not get output buffer");

    p = cert;
    cvc_cert = CVC_d2i_CVC_CERT(NULL, &p, cert_len);
    if (!cvc_cert)
        err("Could not parse CV Cert");
    verb("Parsed CV Cert");
    if (debug) {
        if (!CVC_print(bio_stdout, cvc_cert, 4))
            err("Could not print CV certificate");
    }

    /* FIXME: CVC_CERT_print_ctx -> segfault */
    /* CVC_CERT_print_ctx(bio_stdout, cvc_cert, 1, NULL); */

    if (cert_desc) {
        p = cert_desc;
        desc = d2i_CVC_CERTIFICATE_DESCRIPTION(NULL, &p, cert_desc_len);
        if (!desc)
            err("Could not parse Certificate Description");
        verb("\nParsed Certificate Description");
        if (debug)
            if (!certificate_description_print(bio_stdout, desc, 4))
                err("Could not print Certificate Description");

        if (!CVC_check_description(cvc_cert, cert_desc, cert_desc_len)) {
            err("Certificate Description doesn't match Certificate\n");
        } else {
            verb("Certificate Description matches Certificate");
        }
    }

    fail = 0;

err:
    if (desc)
        CVC_CERTIFICATE_DESCRIPTION_free(desc);
    if (cvc_cert)
        CVC_CERT_free(cvc_cert);
    if (bio_stdout)
        BIO_free_all(bio_stdout);
    return fail;
}

static int parse_cv_file(char *filename) {

    FILE *fp = NULL;
    int fail = 1;
    size_t filesize = 0;
    unsigned char *cv_data = NULL;

    fp = fopen(filename, "rb");
    if (!fp)
        err("Could not open file\n");

    fseek(fp, 0L, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    cv_data = (unsigned char*) malloc(filesize);
    if (!cv_data)
        err("Failed to allocate memory\n");

    if (filesize != fread(cv_data, sizeof(unsigned char), filesize, fp))
        err("Failed to read file\n");

    fail = parse_cv_cert(cv_data, filesize, NULL, 0);

    if (verbose)
        printf("Parsed %s\n", filename);
    else
        printf(".");


err:
    if (fp)
        fclose(fp);
    if(cv_data)
        free(cv_data);

    return fail;
}

int main(int argc, char *argv[])
{
    BIO *bio_file = NULL;
    int fail = 1, i = 1;
        char *filename = NULL;


    for (i=1; i<argc; i++)
    {
        if ((strcmp(argv[i], "--verbose") == 0)
                || (strcmp(argv[i], "-v") == 0)) {
            verbose++;
            continue;
        }
        if ((strcmp(argv[i], "--debug") == 0)
                || (strcmp(argv[i], "-d") == 0)) {
            verbose++;
            debug++;
            continue;
        }
        if ((strcmp(argv[i], "--file") == 0)
                || (strcmp(argv[i], "-f") == 0)) {
            if (i++>=argc) {
                fprintf(stderr, "%d %d-f,--file requires an argument\n", i, argc);
                return fail;
            }
            filename = argv[i];
            continue;
        }
        if ((strcmp(argv[i], "--help") == 0)
                || (strcmp(argv[i], "-h") == 0)) {
            fprintf(stderr,
                    "%s tests the OpenSSL parser for card verifiable certificates\n"
                    "\n"
                    "Usage: %s [Options]\n"
                    "\n"
                    "Options:\n"
                    "  -f,--file         file with cv certificate to parse\n"
                    "  -h,--help         show this help message and exit\n"
                    "  -v,--verbose      more verbosity\n"
                    "     --version      print version information and exit\n"
                    , argv[0], argv[0]
            );
            return fail;
        }
        if (strcmp(argv[i], "--version") == 0) {
            fprintf(stderr,
                    "%s 0.1\n"
                    "\n"
                    "Written by Frank Morgner and Dominik Oepen.\n"
                    , argv[0]
            );
            return fail;
        }

        fprintf(stderr, "unrecognized option \"%s\"\n", argv[i]);
        return fail;
    }


    if (filename)
        fail = parse_cv_file(filename);
    else
        fail = parse_cv_cert(CVCert, CVCert_len, certDesc, certDesc_len);

    fail = 0;

    if (bio_file)
        BIO_free_all(bio_file);

    if (verbose) {
        if (fail)
            printf("Some error occurred.\n");
        else
            printf("Everything works as expected.\n");
    } else {
        if (!fail)
            printf(" ok\n");
        else
            printf(" error\n");
    }

    return fail;
}

