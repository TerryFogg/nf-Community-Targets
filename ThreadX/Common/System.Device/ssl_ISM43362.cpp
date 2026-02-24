//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHal.h>
#include <ssl.h>

bool ssl_parse_certificate_internal(void *buf, size_t size, void *pwd, void *x509)
{
    (void)buf;
    (void)size;
    (void)pwd;
    (void)x509;
    return true;
}
int ssl_decode_private_key_internal(
    const unsigned char *key,
    size_t keyLength,
    const unsigned char *password,
    size_t passwordLength)
{
    (void)key;
    (void)keyLength;
    (void)password;
    (void)passwordLength;
    return 0;
}
int ssl_accept_internal(int socket, int contextHandle)
{
    (void)socket;
    (void)contextHandle;
    return 0;
}
bool ssl_add_cert_auth_internal(int contextHandle, const char *certificate, int certLength)
{
    (void)contextHandle;
    (void)certificate;
    (void)certLength;
    return true;
}
bool ssl_initialize_internal()
{
    return true;
}

bool ssl_generic_init_internal(
    int sslMode,
    int sslVerify,
    const char *certificate,
    int certLength,
    const uint8_t *privateKey,
    int privateKeyLength,
    const char *password,
    int passwordLength,
    int &contextHandle,
    bool useDeviceCertificate,
    bool isServer)
{
    (void)password;
    (void)passwordLength;
    (void)privateKey;
    (void)privateKeyLength;
    (void)certificate;
    (void)certLength;
    (void)useDeviceCertificate;
    (void)sslMode;
    (void)sslVerify;
    (void)isServer;
    return true;
}
bool ssl_exit_context_internal(int contextHandle)
{
    return true;
}
int ssl_connect_internal(int sd, const char *szTargetHost, int contextHandle)
{
    (void)sd;
    (void)szTargetHost;
    (void)contextHandle;
    return 0;
}
int ssl_available_internal(int sd)
{
    (void)sd;
    return 0;
}
int ssl_write_internal(int sd, const char *data, size_t req_len)
{
    (void)sd;
    (void)data;
    (void)req_len;
    return 0;
}
int ssl_read_internal(int sd, char *data, size_t size)
{
    (void)sd;
    (void)data;
    (void)size; // SSL_RESULT__WOULD_BLOCK
    return 0;
}
int ssl_close_socket_internal(int sd)
{
    return 0;
}
