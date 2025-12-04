//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "System.Device.Network.h"
#include <nx_secure_tls.h>
#include <nx_secure_tls_api.h>
#include <nx_secure_x509.h>

NX_IP ServerIP;
NX_TCP_SOCKET ServerSocket;
NX_SECURE_TLS_SESSION TLSSession;
NX_SECURE_TLS_CRYPTO cipher_table;
NX_PACKET *send_packet;
NX_PACKET *receive_packet;
NX_SECURE_X509_CERT SecureX509CertificateLocal;
NX_SECURE_X509_CERT SecureX509CertificateAuthority;

ULONG type_of_service;
ULONG fragment;
UINT time_to_live;
ULONG window_size;
void tcp_urgent_callback(NX_TCP_SOCKET *server_socket);
void tcp_disconnect_callback(NX_TCP_SOCKET *server_socket);

uint8_t working_buffer[2048];
uint8_t tls_packet_buffer[2048];

const UCHAR *private_key;
USHORT priv_len;
UINT private_key_type = NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER; // Default ??????

void tcp_urgent_callback(NX_TCP_SOCKET *server_socket)
{
    // Placeholder for urgent data handling
}
void tcp_disconnect_callback(NX_TCP_SOCKET *server_socket)
{
    // Placeholder for disconnect handling
}
void thread_connect_received(NX_TCP_SOCKET *server_socket, UINT port)
{
    // Placeholder for connection received handling
}

// NetX Secure expects the private key in plain DER/PEM format
// If you need password protection, implement a decryption routine after reading with
// FileX or internal storage then pass the decrypted buffer to a routine to extract the
// private key.

// NOTE:
// First implementation, no password support
HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureServerInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // SslProtocols ( one, Tls, Tls11,Tls12,Tls13)
        // SslVerification ( noVerification,VerifyPeer,certRequired,verifyClientOnce)
        CLR_INT32 sslProtocol = stack.Arg0().NumericByRef().s4;
        CLR_INT32 sslVerification = stack.Arg1().NumericByRef().s4;
        uint8_t *PrivateKey;
        CLR_UINT32 PrivateKeyLength;
        const char *Password = NULL;
        int PasswordLength = 0;
        ULONG actual_status;
        UINT status;
        CLR_RT_HeapBlock *hbCertificate = stack.Arg2().Dereference();
        CLR_RT_HeapBlock *hbCertificateAuthority = stack.Arg3().Dereference();
        CLR_RT_HeapBlock_Array *Certificate = NULL;
        CLR_RT_HeapBlock_Array *CertificateAuthority = NULL;
        CLR_RT_TypeDef_Index x509Certificate2TypeDef;
        CLR_UINT8 *sslCertificateAuthority;
        CLR_UINT8 *sslLocalCertificate;
        CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

        NX_SECURE_X509_CERT *remote_certificate;
        unsigned char remote_cert_buffer[10];
        NX_SECURE_X509_CERT *remote_issuer;
        unsigned char remote_issuer_buffer[10];

        bool useDeviceCertificate = (bool)stack.Arg4().NumericByRef().u1;
        bool typeFound = g_CLR_RT_TypeSystem.FindTypeDef(
            "X509Certificate2",
            "System.Security.Cryptography.X509Certificates",
            x509Certificate2TypeDef);

        // Create SocketException in case of failure
        Library_corlib_native_System_Exception::CreateInstance(
            res,
            g_CLR_RT_WellKnownTypes.m_SocketException,
            CLR_E_FAIL,
            &stack);

        FAULT_ON_NULL(hbCertificate);
        {
            FAULT_ON_NULL(hbCertificateAuthority);
            {
                CertificateAuthority = hbCertificateAuthority[X509Certificate::FIELD___certificate].DereferenceArray();
                Certificate = hbCertificate[X509Certificate::FIELD___certificate].DereferenceArray();

                FAULT_ON_NULL_ARG(CertificateAuthority);
                {
                    sslCertificateAuthority = CertificateAuthority->GetFirstElement();
                    FAULT_ON_NULL_ARG(Certificate);
                    {
                        Certificate->Pin();
                        sslLocalCertificate = Certificate->GetFirstElement();

                        bool IsX509Certificate2 = (hbCertificate->ObjectCls().Type() == x509Certificate2TypeDef.Type());
                        if (IsX509Certificate2)
                        {
                            CLR_RT_HeapBlock_Array *hbaPrivateKey;
                            if (hbaPrivateKey = Certificate[X509Certificate2::FIELD___privateKey].DereferenceArray())
                                PrivateKey = hbaPrivateKey->GetFirstElement();
                            CLR_RT_HeapBlock *hbPassword =
                                hbCertificate[X509Certificate2::FIELD___password].Dereference();
                            if (hbPassword)
                            {
                                Password = (const char *)hbPassword->StringText();
                                PasswordLength = hal_strlen_s(Password);
                            }
                        }

                        // Ensure the IP instance has been initialized.
                        status =
                            nx_ip_status_check(&ServerIP, NX_IP_INITIALIZE_DONE, &actual_status, NX_IP_PERIODIC_RATE);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_tcp_socket_create(
                            &ServerIP,
                            &ServerSocket,
                            (char *)"Server Socket",
                            NX_IP_NORMAL,
                            NX_FRAGMENT_OKAY /*NX_DONT_FRAGMENT*/,
                            NX_IP_TIME_TO_LIVE,
                            8192,
                            NX_NULL,
                            NX_NULL);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_session_create(
                            &TLSSession,
                            &cipher_table,
                            working_buffer,
                            sizeof(working_buffer));
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_session_packet_buffer_set(
                            &TLSSession,
                            tls_packet_buffer,
                            sizeof(tls_packet_buffer));
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_remote_certificate_allocate(
                            &TLSSession,
                            remote_certificate,
                            remote_cert_buffer,
                            sizeof(remote_cert_buffer));
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_remote_certificate_allocate(
                            &TLSSession,
                            remote_issuer,
                            remote_issuer_buffer,
                            sizeof(remote_issuer_buffer));
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_x509_certificate_initialize(
                            &SecureX509CertificateLocal,
                            sslCertificateAuthority,
                            sizeof(sslCertificateAuthority),
                            working_buffer,
                            sizeof(working_buffer),
                            private_key,
                            priv_len,
                            private_key_type);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_local_certificate_add(&TLSSession, &SecureX509CertificateLocal);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_x509_certificate_initialize(
                            &SecureX509CertificateAuthority,
                            sslLocalCertificate,
                            sizeof(sslLocalCertificate),
                            working_buffer,
                            sizeof(working_buffer),
                            private_key,
                            priv_len,
                            private_key_type);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_trusted_certificate_add(&TLSSession, &SecureX509CertificateAuthority);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status =
                            nx_tcp_server_socket_listen(&ServerIP, 12, &ServerSocket, 5, thread_connect_received);
                        if (status != NX_SUCCESS)
                            goto ReturnError;
                    }
                ReturnError:
                    if (status != NX_SUCCESS)
                    {
                        int sslContext = -1; // Dummy context for now
                        CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

                        int translated_socket_error = TranslateNXErrorToSocketError(status);
                        stack.SetResult_I4(sslContext);

                        if ((Library_corlib_native_System_Exception::CreateInstance(
                                res,
                                g_CLR_RT_WellKnownTypes.m_SocketException,
                                CLR_E_FAIL,
                                &stack)) == S_OK)
                        {
                            res.Dereference()
                                [Library_sys_net_native_System_Net_Sockets_SocketException::FIELD___errorCode]
                                    .SetInteger(translated_socket_error);
                            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
                        }
                    }
                }
            }
        }
        NANOCLR_NOCLEANUP();
    }
}

// internal static extern int SecureClientInit(
// int sslProtocols,                             -- SslProtocols
// int sslCertVerify,                            -- int
// X509Certificate certificate,                  -- X509Certificate
// X509Certificate ca,                           -- X509Certificate
// bool useDeviceCertificate                     -- bool  false
// private int _sslContext;
// private bool _isServer;

HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureClientInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // SslProtocols ( one, Tls, Tls11,Tls12,Tls13)
        // SslVerification ( noVerification,VerifyPeer,certRequired,verifyClientOnce)
        CLR_INT32 sslProtocol = stack.Arg0().NumericByRef().s4;
        CLR_INT32 sslVerification = stack.Arg1().NumericByRef().s4;
        uint8_t *PrivateKey;
        CLR_UINT32 PrivateKeyLength;
        const char *Password = NULL;
        int PasswordLength = 0;
        ULONG actual_status;
        UINT status;
        CLR_RT_HeapBlock *hbCertificate = stack.Arg2().Dereference();
        CLR_RT_HeapBlock *hbCertificateAuthority = stack.Arg3().Dereference();
        CLR_RT_HeapBlock_Array *Certificate = NULL;
        CLR_RT_HeapBlock_Array *CertificateAuthority = NULL;
        CLR_RT_TypeDef_Index x509Certificate2TypeDef;
        CLR_UINT8 *sslCertificateAuthority;
        CLR_UINT8 *sslLocalCertificate;
        CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

        NX_SECURE_X509_CERT *remote_certificate;
        unsigned char remote_cert_buffer[10];
        NX_SECURE_X509_CERT *remote_issuer;
        unsigned char remote_issuer_buffer[10];

        bool useDeviceCertificate = (bool)stack.Arg4().NumericByRef().u1;
        bool typeFound = g_CLR_RT_TypeSystem.FindTypeDef(
            "X509Certificate2",
            "System.Security.Cryptography.X509Certificates",
            x509Certificate2TypeDef);

        // Create SocketException in case of failure
        Library_corlib_native_System_Exception::CreateInstance(
            res,
            g_CLR_RT_WellKnownTypes.m_SocketException,
            CLR_E_FAIL,
            &stack);

        FAULT_ON_NULL(hbCertificate);
        {
            FAULT_ON_NULL(hbCertificateAuthority);
            {
                CertificateAuthority = hbCertificateAuthority[X509Certificate::FIELD___certificate].DereferenceArray();
                Certificate = hbCertificate[X509Certificate::FIELD___certificate].DereferenceArray();

                FAULT_ON_NULL_ARG(CertificateAuthority);
                {
                    sslCertificateAuthority = CertificateAuthority->GetFirstElement();
                    FAULT_ON_NULL_ARG(Certificate);
                    {
                        Certificate->Pin();
                        sslLocalCertificate = Certificate->GetFirstElement();

                        bool IsX509Certificate2 = (hbCertificate->ObjectCls().Type() == x509Certificate2TypeDef.Type());
                        if (IsX509Certificate2)
                        {
                            CLR_RT_HeapBlock_Array *hbaPrivateKey;
                            if (hbaPrivateKey = Certificate[X509Certificate2::FIELD___privateKey].DereferenceArray())
                                PrivateKey = hbaPrivateKey->GetFirstElement();
                            CLR_RT_HeapBlock *hbPassword =
                                hbCertificate[X509Certificate2::FIELD___password].Dereference();
                            if (hbPassword)
                            {
                                Password = (const char *)hbPassword->StringText();
                                PasswordLength = hal_strlen_s(Password);
                            }
                        }

                        // Ensure the IP instance has been initialized.
                        status =
                            nx_ip_status_check(&ServerIP, NX_IP_INITIALIZE_DONE, &actual_status, NX_IP_PERIODIC_RATE);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_tcp_socket_create(
                            &ServerIP,
                            &ServerSocket,
                            (char *)"Server Socket",
                            NX_IP_NORMAL,
                            NX_FRAGMENT_OKAY /*NX_DONT_FRAGMENT*/,
                            NX_IP_TIME_TO_LIVE,
                            8192,
                            NX_NULL,
                            NX_NULL);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_session_create(
                            &TLSSession,
                            &cipher_table,
                            working_buffer,
                            sizeof(working_buffer));
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_session_packet_buffer_set(
                            &TLSSession,
                            tls_packet_buffer,
                            sizeof(tls_packet_buffer));
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_remote_certificate_allocate(
                            &TLSSession,
                            remote_certificate,
                            remote_cert_buffer,
                            sizeof(remote_cert_buffer));
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_remote_certificate_allocate(
                            &TLSSession,
                            remote_issuer,
                            remote_issuer_buffer,
                            sizeof(remote_issuer_buffer));
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_x509_certificate_initialize(
                            &SecureX509CertificateLocal,
                            sslCertificateAuthority,
                            sizeof(sslCertificateAuthority),
                            working_buffer,
                            sizeof(working_buffer),
                            private_key,
                            priv_len,
                            private_key_type);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_local_certificate_add(&TLSSession, &SecureX509CertificateLocal);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_x509_certificate_initialize(
                            &SecureX509CertificateAuthority,
                            sslLocalCertificate,
                            sizeof(sslLocalCertificate),
                            working_buffer,
                            sizeof(working_buffer),
                            private_key,
                            priv_len,
                            private_key_type);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status = nx_secure_tls_trusted_certificate_add(&TLSSession, &SecureX509CertificateAuthority);
                        if (status != NX_SUCCESS)
                            goto ReturnError;

                        status =
                            nx_tcp_server_socket_listen(&ServerIP, 12, &ServerSocket, 5, thread_connect_received);
                        if (status != NX_SUCCESS)
                            goto ReturnError;
                    }
                ReturnError:
                    if (status != NX_SUCCESS)
                    {
                        int sslContext = -1; // Dummy context for now
                        CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

                        int translated_socket_error = TranslateNXErrorToSocketError(status);
                        stack.SetResult_I4(sslContext);

                        if ((Library_corlib_native_System_Exception::CreateInstance(
                                res,
                                g_CLR_RT_WellKnownTypes.m_SocketException,
                                CLR_E_FAIL,
                                &stack)) == S_OK)
                        {
                            res.Dereference()
                                [Library_sys_net_native_System_Net_Sockets_SocketException::FIELD___errorCode]
                                    .SetInteger(translated_socket_error);
                            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
                        }
                    }
                }
            }
        }
        NANOCLR_NOCLEANUP();
    }
}

HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureAccept___STATIC__VOID__I4__OBJECT(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int result = 0;
        bool fRes = true;
        CLR_INT64 *timeout;
        CLR_RT_HeapBlock hbTimeout;

        CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
        CLR_INT32 timeout_ms = -1;

        CLR_RT_HeapBlock *socket_info = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket_info);
        {
            socket_entry_t *socket_entry =
                (socket_entry_t *)socket_info[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                    .NumericByRef()
                    .s4;
            FAULT_ON_NULL_ARG(socket_entry);

            // Because we could have been a rescheduled call due to a prior call that would have
            // blocked, we need to see if our handle has been shutdown before continuing.
            if ((int)socket_entry == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
            {
                ThrowError(stack, CLR_E_OBJECT_DISPOSED);
                NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            }

            // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
            hbTimeout.SetInteger((CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS);

            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

            // first make sure we have data to read or ability to write
            while (true)
            {

                nx_tcp_server_socket_accept(socket_ptr, NX_WAIT_FOREVER);
                nx_secure_tls_session_start(&tls_session, &server_socket, NX_WAIT_FOREVER);
                if (result == SOCK_EWOULDBLOCK || result == SOCK_TRY_AGAIN)
                {
                    // non-blocking - allow other threads to run while we wait for socket activity
                    NANOCLR_CHECK_HRESULT(
                        g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));
                }
                else
                {
                    break;
                }
            }

            stack.PopValue(); // Timeout

            NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureConnect___STATIC__VOID__I4__STRING__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int result;

        bool fRes = true;
        CLR_INT64 *timeout;
        CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket);
        {
            CLR_INT32 handle =
                socket[Library_sys_net_native_System_Net_Sockets_Socket::FIELD__m_Handle].NumericByRef().s4;

            CLR_RT_HeapBlock *hb = stack.Arg1().DereferenceString();
            FAULT_ON_NULL_ARG(hb);
            {

                CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
                CLR_RT_HeapBlock hbTimeout;

                const char *szName = hb->StringText();

                // Infinite Timeout
                hbTimeout.SetInteger((CLR_INT64)-1);

                NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

                while (true)
                {
                    // result = SSL_Connect(handle, szName, sslContext);
                    //  Connect to the server
                    nx_tcp_client_socket_connect(
                        socket_ptr,
                        IP_ADDRESS(192, 168, 1, 100),
                        SERVER_PORT,
                        NX_WAIT_FOREVER);

                    if (result == SOCK_EWOULDBLOCK || result == SOCK_TRY_AGAIN)
                    {
                        // non-blocking - allow other threads to run while we wait for socket
                        // activity
                        NANOCLR_CHECK_HRESULT(
                            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));
                        if (result < 0)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                stack.PopValue(); // Timeout

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureRead___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{

    CLR_INT32 handle;

    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT32 totalRead;
        bool fRes = true;
        CLR_INT64 *timeout;
        int result = 0;
        CLR_UINT8 *buffer;
        CLR_RT_HeapBlock hbTimeout;

        CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        CLR_INT32 offset = stack.Arg2().NumericByRef().s4;
        CLR_INT32 count = stack.Arg3().NumericByRef().s4;
        CLR_INT32 timeout_ms = stack.Arg4().NumericByRef().s4;
        if (count == 0)
        {
            stack.SetResult_I4(0);
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
        CLR_INT32 handle = socket[Library_sys_net_native_System_Net_Sockets_Socket::FIELD__m_Handle].NumericByRef().s4;
        if (handle == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
        {
            ThrowError(stack, CLR_E_OBJECT_DISPOSED);
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }

        FAULT_ON_NULL(arrData);
        {
            FAULT_ON_NULL(socket);
            {
                if (count == 0)
                {
                    stack.SetResult_I4(0);
                    NANOCLR_SET_AND_LEAVE(S_OK);
                }
                hbTimeout.SetInteger((CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS);
                NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));
                //
                // Push "totReadWrite" onto the eval stack.
                //
                if (stack.m_customState == 1)
                {
                    stack.PushValueI4(0);

                    stack.m_customState = 2;
                }
                totalRead = stack.m_evalStack[1].NumericByRef().s4;
                buffer = arrData->GetElement(offset + totalRead);
                count -= totalRead;
                if ((offset + count + totalRead) > (int)arrData->m_numOfElements)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
                }
                while (count > 0)
                {
                    // first make sure we have data to read or ability to write
                    while (fRes)
                    {
                        nx_secure_tls_session_receive(&tls_session, buffer, buffer_size, &actual_size, NX_WAIT_FOREVER);

                        // check SSL_DataAvailable() in case SSL has already read and buffered
                        // socket data
                        result = SSL_DataAvailable(handle);

                        if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                        {
                            break;
                        }

                        result =
                            Library_sys_net_native_System_Net_Sockets_NativeSocket::Helper__SelectSocket(handle, 0);

                        if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                        {
                            break;
                        }

                        // non-blocking - allow other threads to run while we wait for socket
                        // activity
                        NANOCLR_CHECK_HRESULT(
                            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));

                        // timeout expired
                        if (!fRes)
                        {
                            result = SOCK_SOCKET_ERROR;
                            ThrowError(stack, SOCK_ETIMEDOUT);
                            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
                        }
                    }

                    // socket is in the excepted state, so let's bail out
                    if (SOCK_SOCKET_ERROR == result)
                    {
                        break;
                    }

                    result = SSL_Read(handle, (char *)buffer, count);

                    if (result == SSL_RESULT__WOULD_BLOCK)
                    {
                        continue;
                    }

                    // ThrowOnError expects anything other than 0 to be a failure - so return 0 if
                    // we don't have an error
                    if (result <= 0)
                    {
                        break;
                    }
                    buffer += result;
                    totalRead += result;
                    count -= result;

                    // read is non-blocking if we have any data
                    if (totalRead > 0)
                    {
                        break;
                    }

                    stack.m_evalStack[1].NumericByRef().s4 = totalRead;
                }

                stack.PopValue(); // totReadWrite
                stack.PopValue(); // Timeout

                if (result < 0)
                {
                    NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
                }

                stack.SetResult_I4(totalRead);
            }
        }
    }
    NANOCLR_NOCLEANUP();

}
HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureWrite___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{

    CLR_INT32 handle;

    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT32 totalWritten;
        bool fRes = true;
        CLR_INT64 *timeout;
        int result = 0;
        CLR_UINT8 *buffer;
        CLR_RT_HeapBlock hbTimeout;

        CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        CLR_INT32 offset = stack.Arg2().NumericByRef().s4;
        CLR_INT32 count = stack.Arg3().NumericByRef().s4;
        CLR_INT32 timeout_ms = stack.Arg4().NumericByRef().s4;
        if (count == 0)
        {
            stack.SetResult_I4(0);
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
        CLR_INT32 handle = socket[Library_sys_net_native_System_Net_Sockets_Socket::FIELD__m_Handle].NumericByRef().s4;
        if (handle == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
        {
            ThrowError(stack, CLR_E_OBJECT_DISPOSED);
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }

        FAULT_ON_NULL(arrData);
        {
            FAULT_ON_NULL(socket);
            {
                if (count == 0)
                {
                    stack.SetResult_I4(0);
                    NANOCLR_SET_AND_LEAVE(S_OK);
                }
                hbTimeout.SetInteger((CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS);
                NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));
                //
                // Push "totReadWrite" onto the eval stack.
                //
                if (stack.m_customState == 1)
                {
                    stack.PushValueI4(0);

                    stack.m_customState = 2;
                }
                totalWritten = stack.m_evalStack[1].NumericByRef().s4;
                buffer = arrData->GetElement(offset + totalWritten);
                count -= totalWritten;
                if ((offset + count + totalWritten) > (int)arrData->m_numOfElements)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
                }
                while (count > 0)
                {
                    // first make sure we have data to read or ability to write
                    while (fRes)
                    {

                        nx_secure_tls_session_send(&tls_session, data, length, NX_WAIT_FOREVER);
                        result =
                            Library_sys_net_native_System_Net_Sockets_NativeSocket::Helper__SelectSocket(handle, 0);

                        if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                        {
                            break;
                        }

                        // non-blocking - allow other threads to run while we wait for socket
                        // activity
                        NANOCLR_CHECK_HRESULT(
                            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));

                        // timeout expired
                        if (!fRes)
                        {
                            result = SOCK_SOCKET_ERROR;
                            ThrowError(stack, SOCK_ETIMEDOUT);
                            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
                        }
                    }

                    // socket is in the excepted state, so let's bail out
                    if (SOCK_SOCKET_ERROR == result)
                    {
                        break;
                    }
                    result = SSL_Write(handle, (const char *)buffer, count);

                    // ThrowOnError expects anything other than 0 to be a failure - so return 0 if
                    // we don't have an error
                    if (result <= 0)
                    {
                        break;
                    }
                    buffer += result;
                    totalWritten += result;
                    count -= result;

                    stack.m_evalStack[1].NumericByRef().s4 = totalWritten;
                }

                stack.PopValue(); // totReadWrite
                stack.PopValue(); // Timeout

                if (result < 0)
                {
                    NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
                }

                stack.SetResult_I4(totalWritten);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureCloseSocket___STATIC__I4__OBJECT(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

        UINT status;
        CLR_INT32 handle;

        CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket);

        handle = socket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle].NumericByRef().s4;
        FAULT_ON_NULL(handle);

        // End the TLS session
        status = nx_secure_tls_session_end(&tls_session, NX_WAIT_FOREVER);
        if (status == NX_SUCCESS)
        {
        }

        try
        {
        }
        catch ()
        {
        }
        // Disconnect the TCP socket
        status = nx_tcp_socket_disconnect(socket_ptr, NX_WAIT_FOREVER);
        if (status == NX_SUCCESS)
        {

            // Delete the TLS session
            nx_secure_tls_session_end(&tls_session, NX_WAIT_FOREVER);
            nx_secure_tls_session_delete(&tls_session);

            stack.SetResult_I4(status);
        }
        NANOCLR_NOCLEANUP();
    }
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::ExitSecureContext___STATIC__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
        FAULT_ON_NULL(sslContext);

        status = nx_secure_tls_session_end(&tls_session, NX_WAIT_FOREVER);
        if (status == NX_SUCCESS)
        {
        }
        nx_secure_tls_session_delete(&tls_session);
        stack.SetResult_I4(status);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::DataAvailable___STATIC__I4__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NX_SECURE_TLS_SESSION tls_session;
        UCHAR buffer[1024];
        UINT bytes_received;
        UINT status;
        NX_PACKET *packet;
        NX_TCP_SOCKET tcp_socket_ptr;
        int result;

        CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket);

        CLR_INT32 handle =
            socket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle].NumericByRef().s4;

        status = nx_tcp_socket_receive(&tcp_socket_ptr, &packet, NX_NO_WAIT);
        if (status == NX_SUCCESS)
        {
            // Data is available in 'packet'
            // Remember to release the packet when done: nx_packet_release(packet);
        }
        else if (status == NX_NO_PACKET)
        {
            // No data available
        }

        stack.SetResult_I4(result);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate::
    ParseCertificate___STATIC__VOID__SZARRAY_U1__BYREF_STRING__BYREF_STRING__BYREF_SystemDateTime__BYREF_SystemDateTime(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        X509Certificate x;
        CLR_INT64 *val;
        SYSTEMTIME st;
        NX_SECURE_X509_CERT certificate;
        UINT status;

        CLR_RT_HeapBlock_Array *arrData = stack.Arg0().DereferenceArray();
        FAULT_ON_NULL(arrData);
        certificate.nx_secure_x509_certificate_data = arrData->GetFirstElement();

        CLR_RT_HeapBlock hbIssuer;
        CLR_RT_ProtectFromGC gc1(hbIssuer);

        CLR_RT_HeapBlock hbSubject;
        CLR_RT_ProtectFromGC gc2(hbSubject);

        // Load the certificate
        status = nx_secure_x509_certificate_initialize(
            &certificate,
            test_device_cert_der,
            test_device_cert_der_len,
            NX_NULL,
            0,
            test_device_cert_key_der,
            test_device_cert_key_der_len,
            NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER);

        if (status == NX_SUCCESS)
        {
        }
        //// from test netxduo code
        status = _nx_secure_tls_local_certificate_add(&tls_session, &certificate);
        //// ??????????

        ////// fill in the various fields of the certificate class
        ////NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(
        ////    hbIssuer.StoreToReference(stack.Arg1(),
        ////    certificate.nx_secure_x509_issuer.nx_secure_x509_common_name));
        ////NANOCLR_CHECK_HRESULT(hbIssuer.StoreToReference(stack.Arg1(), 0));

        ////NANOCLR_CHECK_HRESULT(
        ////    CLR_RT_HeapBlock_String::CreateInstance(hbSubject,
        /// certificate.nx_secure_x509_subject_identifier));
        ////NANOCLR_CHECK_HRESULT(hbSubject.StoreToReference(stack.Arg2(), 0));

        const UCHAR *StartDate = certificate.nx_secure_x509_not_before;

        st.wYear =
            (StartDate[0] - '0') * 1000 + (StartDate[1] - '0') * 100 + (StartDate[2] - '0') * 10 + (StartDate[3] - '0');
        st.wMonth = (StartDate[4] - '0') * 10 + (StartDate[5] - '0');
        st.wDay = (StartDate[6] - '0') * 10 + (StartDate[7] - '0');
        st.wHour = (StartDate[8] - '0') * 10 + (StartDate[9] - '0');
        st.wMinute = (StartDate[10] - '0') * 10 + (StartDate[11] - '0');
        st.wSecond = (StartDate[12] - '0') * 10 + (StartDate[13] - '0');

        val = Library_corlib_native_System_DateTime::GetValuePtr(stack.Arg3());
        *val = HAL_Time_ConvertFromSystemTime(&st);

        const UCHAR *EndDate = certificate.nx_secure_x509_not_after;

        st.wYear = (EndDate[0] - '0') * 1000 + (EndDate[1] - '0') * 100 + (EndDate[2] - '0') * 10 + (EndDate[3] - '0');
        st.wMonth = (EndDate[4] - '0') * 10 + (EndDate[5] - '0');
        st.wDay = (EndDate[6] - '0') * 10 + (EndDate[7] - '0');
        st.wHour = (EndDate[8] - '0') * 10 + (EndDate[9] - '0');
        st.wMinute = (EndDate[10] - '0') * 10 + (EndDate[11] - '0');
        st.wSecond = (EndDate[12] - '0') * 10 + (EndDate[13] - '0');

        val = Library_corlib_native_System_DateTime::GetValuePtr(stack.Arg4());
        *val = HAL_Time_ConvertFromSystemTime(&st);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2::
    DecodePrivateKeyNative___STATIC__VOID__SZARRAY_U1__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *keyData = stack.Arg0().DereferenceArray();
        CLR_UINT8 *keyBuffer = keyData->GetFirstElement();

        const char *password = NULL;
        CLR_UINT32 passwordLength = 0;

        CLR_RT_HeapBlock *passwordHb = stack.Arg1().DereferenceString();
        if (passwordHb != NULL)
        {
            password = passwordHb->StringText();
            passwordLength = hal_strlen_s(password);
        }

        UINT status;
        NX_SECURE_TLS_SESSION tls_session;
        UCHAR private_key[] = {/* DER-encoded private key bytes */};
        UINT private_key_length = sizeof(private_key);

        NX_CRYPTO_EC *curve;
        UCHAR *output;
        ULONG output_length_in_byte;
        ULONG *actual_output_length;
        HN_UBASE *scratch;

        status =
            _nx_crypto_ec_key_pair_stream_generate(curve, output, output_length_in_byte, actual_output_length, scratch);

        if (SSL_DecodePrivateKey(
                (const unsigned char *)keyBuffer,
                keyData->m_numOfElements,
                (const unsigned char *)password,
                passwordLength) < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
