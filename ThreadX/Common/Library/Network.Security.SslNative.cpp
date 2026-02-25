//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Network.h"
#include <nx_secure_tls.h>
#include <nx_secure_tls_api.h>
#include <nx_secure_x509.h>

NX_IP ServerIP;
NX_SECURE_TLS_SESSION TLSServerSession;
NX_SECURE_TLS_SESSION TLSClientSession;
NX_SECURE_TLS_CRYPTO cipher_table;
NX_PACKET *send_packet;
NX_PACKET *receive_packet;
NX_SECURE_X509_CERT SecureX509CertificateLocal;
NX_SECURE_X509_CERT SecureX509CertificateAuthority;
NX_SECURE_X509_CERT SecureX509CertificateRemoteServer;
NX_SECURE_X509_CERT SecureX509CertificateRemoteServerAuthority;

const UCHAR *private_key;
USHORT priv_len;
UINT private_key_type = NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER; // Default ??????
ULONG type_of_service;
ULONG fragment;
UINT time_to_live;
ULONG window_size;

unsigned char remote_certificate_buffer[10];
unsigned char remote_certificate_authority_buffer[10];
uint8_t working_buffer[2048];
uint8_t tls_packet_buffer[2048];

#pragma region Callbacks
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
#pragma endregion

// NetX Secure expects the private key in plain DER format, not PEM headers or footers
// If you need password protection, implement a decryption routine after reading with
// FileX or internal storage then pass the decrypted buffer to a routine to extract the
// private key.

// NOTE:
// First implementation, no password support
//
// SslProtocols ( one, Tls, Tls11,Tls12,Tls13)
// SslVerification ( noVerification,VerifyPeer,certRequired,verifyClientOnce)
HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureServerInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *hbaPrivateKey;
        CLR_RT_HeapBlock *hbPassword;
        uint8_t *PrivateKey;
        CLR_UINT32 PrivateKeyLength;
        const char *Password;
        int PasswordLength = 0;
        ULONG actual_status;
        UINT status;
        bool IsX509Certificate2;
        bool useDeviceCertificate;
        CLR_RT_HeapBlock *hbCertificate;
        CLR_RT_HeapBlock *hbCertificateAuthority;
        CLR_RT_HeapBlock_Array *hbaCertificate;
        CLR_RT_HeapBlock_Array *hbaCertificateAuthority;
        CLR_RT_TypeDef_Index tiX509Certificate2;
        CLR_UINT8 *sslLocalCertificate;
        CLR_UINT8 *sslCertificateAuthority;
        CLR_INT32 sslMode;
        CLR_INT32 sslVerification;

        NX_SECURE_X509_CERT *remote_certificate;
        unsigned char remote_cert_buffer[10];
        NX_SECURE_X509_CERT *remote_issuer;
        unsigned char remote_issuer_buffer[10];

        FAULT_ON_NULL((hbCertificate = stack.Arg2().Dereference()));
        FAULT_ON_NULL_ARG(hbaCertificate = hbCertificate[X509Certificate::FIELD___certificate].DereferenceArray());

        hbCertificate->Pin();
        g_CLR_RT_TypeSystem.FindTypeDef(
            "X509Certificate2",
            "System.Security.Cryptography.X509Certificates",
            tiX509Certificate2);
        IsX509Certificate2 = (hbCertificate->ObjectCls().Type() == tiX509Certificate2.Type());

        if (IsX509Certificate2)
        {
            FAULT_ON_NULL_ARG(hbaPrivateKey = hbCertificate[X509Certificate2::FIELD___privateKey].DereferenceArray());
            FAULT_ON_NULL_ARG(hbPassword = hbCertificate[X509Certificate2::FIELD___password].Dereference());
            PrivateKey = hbaPrivateKey->GetFirstElement();
            Password = (const char *)hbPassword->StringText();
            PasswordLength = hal_strlen_s(Password);
        }
        sslLocalCertificate = hbaCertificate->GetFirstElement();

        sslMode = stack.Arg0().NumericByRef().s4;
        sslVerification = stack.Arg1().NumericByRef().s4;
        useDeviceCertificate = (bool)stack.Arg4().NumericByRef().u1;

        status = nx_secure_tls_session_create(&TLSServerSession, &cipher_table, working_buffer, sizeof(working_buffer));
        FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);

        status =
            nx_secure_tls_session_packet_buffer_set(&TLSServerSession, tls_packet_buffer, sizeof(tls_packet_buffer));
        FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);

        status = nx_secure_tls_remote_certificate_allocate(
            &TLSServerSession,
            remote_certificate,
            remote_cert_buffer,
            sizeof(remote_cert_buffer));
        FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);

        status = nx_secure_tls_remote_certificate_allocate(
            &TLSServerSession,
            remote_issuer,
            remote_issuer_buffer,
            sizeof(remote_issuer_buffer));
        FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);

        status = nx_secure_x509_certificate_initialize(
            &SecureX509CertificateLocal,
            sslCertificateAuthority,
            sizeof(sslCertificateAuthority),
            working_buffer,
            sizeof(working_buffer),
            private_key,
            priv_len,
            private_key_type);
        FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);

        status = nx_secure_tls_local_certificate_add(&TLSServerSession, &SecureX509CertificateLocal);
        FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);

        FAULT_ON_NULL(hbCertificateAuthority);
        hbCertificateAuthority = stack.Arg3().Dereference();
        sslCertificateAuthority = hbaCertificate->GetFirstElement();

        status = nx_secure_x509_certificate_initialize(
            &SecureX509CertificateAuthority,
            sslLocalCertificate,
            sizeof(sslLocalCertificate),
            working_buffer,
            sizeof(working_buffer),
            private_key,
            priv_len,
            private_key_type);
        FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);

        status = nx_secure_tls_trusted_certificate_add(&TLSServerSession, &SecureX509CertificateAuthority);
        FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureClientInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // SslProtocols ( one, Tls, Tls11,Tls12,Tls13)
        // SslVerification ( noVerification,VerifyPeer,certRequired,verifyClientOnce)
        ULONG actual_status;
        UINT status;
        CLR_INT32 sslProtocol = stack.Arg0().NumericByRef().s4;
        CLR_INT32 sslVerification = stack.Arg1().NumericByRef().s4;
        uint8_t *PrivateKey;
        CLR_UINT32 PrivateKeyLength;
        const char *Password = NULL;
        int PasswordLength = 0;
        CLR_RT_HeapBlock *hbCertificate;
        CLR_RT_HeapBlock *hbCertificateAuthority;
        CLR_RT_HeapBlock_Array *hbaPrivateKey;
        CLR_RT_HeapBlock *hbPassword;
        CLR_RT_HeapBlock_Array *Certificate = NULL;
        CLR_RT_HeapBlock_Array *CertificateAuthority = NULL;
        CLR_RT_TypeDef_Index x509Certificate2TypeDef;
        CLR_UINT8 *sslCertificateAuthority;
        CLR_UINT8 *sslLocalCertificate;

        bool useDeviceCertificate = (bool)stack.Arg4().NumericByRef().u1;
        bool typeFound = g_CLR_RT_TypeSystem.FindTypeDef(
            "X509Certificate2",
            "System.Security.Cryptography.X509Certificates",
            x509Certificate2TypeDef);

        hbCertificate = stack.Arg2().Dereference();
        FAULT_ON_NULL(hbCertificate);
        {
            hbCertificateAuthority = stack.Arg3().Dereference();
            FAULT_ON_NULL(hbCertificateAuthority);
            {
                CertificateAuthority = hbCertificateAuthority[X509Certificate::FIELD___certificate].DereferenceArray();
                FAULT_ON_NULL_ARG(CertificateAuthority);
                {
                    sslCertificateAuthority = CertificateAuthority->GetFirstElement();
                    Certificate = hbCertificate[X509Certificate::FIELD___certificate].DereferenceArray();
                    FAULT_ON_NULL_ARG(Certificate);
                    {
                        Certificate->Pin();
                        sslLocalCertificate = Certificate->GetFirstElement();

                        bool IsX509Certificate2 = (hbCertificate->ObjectCls().Type() == x509Certificate2TypeDef.Type());
                        if (IsX509Certificate2)
                        {
                            if (hbaPrivateKey = Certificate[X509Certificate2::FIELD___privateKey].DereferenceArray())
                                PrivateKey = hbaPrivateKey->GetFirstElement();
                            hbPassword = hbCertificate[X509Certificate2::FIELD___password].Dereference();
                            if (hbPassword)
                            {
                                Password = (const char *)hbPassword->StringText();
                                PasswordLength = hal_strlen_s(Password);
                            }
                        }

                        // Ensure the IP instance has been initialized.
                        status =
                            nx_ip_status_check(&ServerIP, NX_IP_INITIALIZE_DONE, &actual_status, NX_IP_PERIODIC_RATE);
                        FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);

                        status = nx_secure_tls_session_create(
                            &TLSClientSession,
                            &cipher_table,
                            working_buffer,
                            sizeof(working_buffer));
                        FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);

                        status = nx_secure_tls_session_packet_buffer_set(
                            &TLSClientSession,
                            tls_packet_buffer,
                            sizeof(tls_packet_buffer));
                        FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);

                        status = nx_secure_tls_remote_certificate_allocate(
                            &TLSClientSession,
                            &SecureX509CertificateRemoteServer,
                            remote_certificate_buffer,
                            sizeof(remote_certificate_buffer));
                        FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);

                        // Need to allocate space for the certificate coming in from the remote host
                        status = nx_secure_tls_remote_certificate_allocate(
                            &TLSClientSession,
                            &SecureX509CertificateRemoteServerAuthority,
                            remote_certificate_authority_buffer,
                            sizeof(remote_certificate_authority_buffer));
                        FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);

                        // Add our trusted certificate
                        status = nx_secure_x509_certificate_initialize(
                            &SecureX509CertificateLocal,
                            sslCertificateAuthority,
                            sizeof(sslCertificateAuthority),
                            working_buffer,
                            sizeof(working_buffer),
                            private_key,
                            priv_len,
                            private_key_type);
                        FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);

                        status = nx_secure_tls_trusted_certificate_add(&TLSClientSession, &SecureX509CertificateLocal);
                        FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);

                        status = nx_secure_x509_certificate_initialize(
                            &SecureX509CertificateAuthority,
                            sslLocalCertificate,
                            sizeof(sslLocalCertificate),
                            working_buffer,
                            sizeof(working_buffer),
                            private_key,
                            priv_len,
                            private_key_type);
                        FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);

                        status =
                            nx_secure_tls_trusted_certificate_add(&TLSClientSession, &SecureX509CertificateAuthority);
                        FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);
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
        UINT status;
        CLR_INT32 timeout_ms = -1;
        CLR_RT_HeapBlock *pbSocket;
        socket_entry_t *socket_entry;
        CLR_INT32 sslContext;

        FAULT_ON_NULL(pbSocket = stack.Arg1().Dereference());
        socket_entry =
            (socket_entry_t *)pbSocket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                .NumericByRef()
                .s4;
        // If we are accepting on the socket, assume its for the server
        socket_entry->TLSSession = TLSServerSession;

        sslContext = stack.Arg0().NumericByRef().s4;

        // Because we could have been a rescheduled call due to a prior call that would have
        // blocked, we need to see if our handle has been shutdown before continuing.
        if ((int)socket_entry == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }
        hbTimeout.SetInteger((CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS);
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

        while (true)
        {
            // Setup this thread to bind to a port.
            status = nx_tcp_client_socket_bind(socket_entry->tcp_socket, 0, NX_WAIT_FOREVER);
            FAULT_IF_NX_CALL_UNSUCCESSFUL(TranslateNXStatusToBSDStatus(status), stack);

            status = nx_secure_tls_session_start(&TLSClientSession, socket_entry->tcp_socket, NX_WAIT_FOREVER);
            FAULT_IF_NX_CALL_UNSUCCESSFUL(SOCK_EINVAL, stack);

            status = nx_tcp_server_socket_accept(socket_entry->tcp_socket, NX_WAIT_FOREVER);
            FAULT_IF_NX_CALL_UNSUCCESSFUL(TranslateNXStatusToBSDStatus(status), stack);

            if (status == NX_IN_PROGRESS)
            {
                // non-blocking - allow other threads to run while we wait for socket activity
                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));
            }
            else if (status != NX_SUCCESS)
            {
                status = TranslateNXStatusToBSDStatus(status);
                SetReturnStatus(stack, status, g_CLR_RT_WellKnownTypes.m_SocketException);
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                break;
            }
        }
        stack.PopValue(); // Timeout
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
        UINT status;
        bool fRes = true;
        CLR_INT64 *timeout;
        CLR_INT32 sslContext;
        CLR_RT_HeapBlock *hb;
        CLR_RT_HeapBlock hbTimeout;
        CLR_RT_HeapBlock *pbSocket;
        socket_entry_t *socket_entry;
        const char *pName;

        FAULT_ON_NULL(pbSocket = stack.Arg0().Dereference());
        sslContext = stack.Arg0().NumericByRef().s4;
        socket_entry =
            (socket_entry_t *)pbSocket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                .NumericByRef()
                .s4;
        if ((int)socket_entry == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }
        // If we are connecting on the socket, assume its for the client
        socket_entry->TLSSession = TLSClientSession;

        FAULT_ON_NULL_ARG(hb = stack.Arg1().DereferenceString());
        pName = hb->StringText();
        hbTimeout.SetInteger(NETWORK_INFINITE_TIMEOUT);
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

        ULONG TOFIX_serverIP;
        UINT server_port = 9999;
        while (true)
        {
            status = nx_tcp_client_socket_connect(
                socket_entry->tcp_socket,
                TOFIX_serverIP,
                server_port,
                5 * NX_IP_PERIODIC_RATE);
            if (status == NX_IN_PROGRESS)
            {
                fRes = true;
                while (fRes)
                {
                    // Connection is being established asynchronously
                    // Return to CLR and wait for the connection to complete
                    NANOCLR_CHECK_HRESULT(
                        g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));
                    if (!fRes)
                    {
                        SetReturnStatus(stack, SOCK_SOCKET_ERROR, g_CLR_RT_WellKnownTypes.m_NetworkInterface);
                        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                    }
                }
            }
            else if (status != NX_SUCCESS)
            {
                status = TranslateNXStatusToBSDStatus(status);
                SetReturnStatus(stack, status, g_CLR_RT_WellKnownTypes.m_SocketException);
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }
        stack.PopValue(); // Timeout
        status = TranslateNXStatusToBSDStatus(status);
        SetReturnStatus(stack, status, g_CLR_RT_WellKnownTypes.m_SocketException);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureRead___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 handle;
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT32 totalRead;
        bool fRes = true;
        CLR_INT64 *timeout;
        int result = 0;
        UINT status;
        socket_entry_t *socket_entry;
        CLR_RT_HeapBlock *pbSocket;
        CLR_RT_HeapBlock_Array *pbArrData;
        CLR_INT32 offset;
        ULONG count;
        CLR_INT32 timeout_ms;
        NX_PACKET *receive_buffer;

        FAULT_ON_NULL(pbSocket = stack.Arg0().Dereference());
        FAULT_ON_NULL(pbArrData = stack.Arg1().DereferenceArray());

        if ((count = stack.Arg3().NumericByRef().s4) == 0)
        {
            stack.SetResult_I4(0);
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
        socket_entry =
            (socket_entry_t *)pbSocket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                .NumericByRef()
                .s4;
        if ((int)socket_entry == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }
        offset = stack.Arg2().NumericByRef().s4;
        timeout_ms = stack.Arg4().NumericByRef().s4;
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
        receive_buffer->nx_packet_data_start = pbArrData->GetElement(offset + totalRead);
        count -= totalRead;
        if ((offset + count + totalRead) > (int)pbArrData->m_numOfElements)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }
        while (count > 0)
        {
            // first make sure we have data to read or ability to write
            while (fRes)
            {
                status = nx_secure_tls_session_receive(&socket_entry->TLSSession, &receive_buffer, NX_WAIT_FOREVER);
                FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);

                /* Extract data received from server. */
                status = nx_packet_data_extract_offset(receive_packet, 0, receive_buffer, 100, &count);
                FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack);

                // non-blocking - allow other threads to run while we wait for socket
                // activity
                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));

                // timeout expired
                if (!fRes)
                {
                    SetReturnStatus(stack, SOCK_SOCKET_ERROR, g_CLR_RT_WellKnownTypes.m_NetworkInterface);
                    NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                }
            }
            receive_buffer += result;
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
            // Need to replace with the proper code that should be returned below
            SetReturnStatus(stack, SOCK_SOCKET_ERROR, g_CLR_RT_WellKnownTypes.m_SocketException);
        }

        stack.SetResult_I4(totalRead);
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
        NX_PACKET *pReceivePacket;
        CLR_INT32 offset;
        CLR_INT32 count;
        CLR_INT32 timeout_ms;
        CLR_INT32 status;
        socket_entry_t *socket_entry;
        CLR_RT_HeapBlock *pbSocket;
        NX_PACKET *write_buffer;
        CLR_RT_HeapBlock *socket;
        CLR_RT_HeapBlock_Array *arrData;
        offset = stack.Arg2().NumericByRef().s4;
        timeout_ms = stack.Arg4().NumericByRef().s4;

        FAULT_ON_NULL(socket = stack.Arg0().Dereference());
        FAULT_ON_NULL(arrData = stack.Arg1().DereferenceArray());

        if ((count = stack.Arg3().NumericByRef().s4) == 0)
        {
            stack.SetResult_I4(0);
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
        socket_entry =
            (socket_entry_t *)pbSocket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                .NumericByRef()
                .s4;
        if ((CLR_INT32)socket_entry == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
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

                write_buffer->nx_packet_data_start = arrData->GetElement(offset + totalWritten);
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
                        status = nx_secure_tls_session_send(&socket_entry->TLSSession, send_packet, NX_NO_WAIT);
                        // non-blocking - allow other threads to run while we wait for socket
                        // activity

                        if (status == NX_SUCCESS)
                        {
                            // All data sent in one go
                            break;
                        }
                        else if (status == NX_NO_PACKET)
                        {
                            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine
                                                      .WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));
                            // timeout expired
                            if (!fRes)
                            {
                                SetReturnStatus(stack, SOCK_SOCKET_ERROR, g_CLR_RT_WellKnownTypes.m_NetworkInterface);
                                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                            }
                        }
                        else if (status == NX_NOT_CONNECTED)
                        {
                            // TLS session closed or not connected
                            break;
                        }
                    }
                    write_buffer->nx_packet_data_start += result;
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
        socket_entry_t *socket_entry;
        CLR_RT_HeapBlock *pbSocket;

        CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
        FAULT_ON_NULL(pbSocket = stack.Arg1().Dereference());
        {
            socket_entry =
                (socket_entry_t *)pbSocket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                    .NumericByRef()
                    .s4;
            // End the TLS session
            status = nx_secure_tls_session_end(&socket_entry->TLSSession, NX_WAIT_FOREVER);
            if (status == NX_SUCCESS)
            {
            }

            // Disconnect the TCP socket
            status = nx_tcp_socket_disconnect(socket_entry->tcp_socket, NX_WAIT_FOREVER);
            nx_secure_tls_session_end(&socket_entry->TLSSession, NX_WAIT_FOREVER);
            nx_secure_tls_session_delete(&socket_entry->TLSSession);
            stack.SetResult_I4(status);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::ExitSecureContext___STATIC__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
        socket_entry_t *socket_entry;
        CLR_RT_HeapBlock *pbSocket;

        FAULT_ON_NULL(sslContext);
        {
            socket_entry =
                (socket_entry_t *)pbSocket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                    .NumericByRef()
                    .s4;
            status = nx_secure_tls_session_end(&socket_entry->TLSSession, NX_WAIT_FOREVER);
            if (status == NX_SUCCESS)
            {
            }
            nx_secure_tls_session_delete(&socket_entry->TLSSession);
            stack.SetResult_I4(status);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::DataAvailable___STATIC__I4__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NX_SECURE_TLS_SESSION tls_session;
        UINT status;
        NX_PACKET *packet;
        NX_TCP_SOCKET tcp_socket_ptr;
        bool dataAvailable = false;
        socket_entry_t *socket_entry;
        CLR_RT_HeapBlock *pbSocket;

        FAULT_ON_NULL(pbSocket = stack.Arg0().Dereference());
        {
            socket_entry =
                (socket_entry_t *)pbSocket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                    .NumericByRef()
                    .s4;
            status = nx_tcp_socket_receive(&tcp_socket_ptr, &packet, NX_NO_WAIT);
            if (status == NX_SUCCESS)
            {
                dataAvailable = true;
            }
            stack.SetResult_I4(dataAvailable);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate::
    ParseCertificate___STATIC__VOID__SZARRAY_U1__BYREF_STRING__BYREF_STRING__BYREF_SystemDateTime__BYREF_SystemDateTime(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // Supply a certificate with DER data and decode and return
        // Issuer,Subject,Effective Date,Expiration Date
        CLR_RT_HeapBlock_Array *hbArrayData;
        CLR_RT_HeapBlock hbReturnIssuer;
        CLR_RT_HeapBlock hbReturnSubject;
        CLR_INT64 *returnEffectiveDate;
        CLR_INT64 *returnExpirationDate;

        NX_SECURE_X509_CERT certificate = {};
        CLR_UINT8 *pCertificateData;
        CLR_UINT8 certificateLength;

        X509CertData clr_cert = {};

        CLR_INT64 *val;
        SYSTEMTIME st;
        UINT status;

        hbArrayData = stack.Arg0().DereferenceArray();
        FAULT_ON_NULL(hbArrayData);
        {
            pCertificateData = hbArrayData->GetFirstElement();
            FAULT_ON_NULL_ARG(pCertificateData)
            {
                certificateLength = hbArrayData->m_numOfElements;
                status = nx_secure_x509_certificate_initialize(
                    &certificate,
                    pCertificateData,
                    certificateLength,
                    NX_NULL,
                    0,
                    NX_NULL,
                    0,
                    NX_SECURE_X509_KEY_TYPE_NONE);

                if (status != NX_SUCCESS)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                }

                memcpy(
                    clr_cert.Issuer,
                    (void *)certificate.nx_secure_x509_issuer.nx_secure_x509_common_name,
                    certificate.nx_secure_x509_issuer.nx_secure_x509_common_name_length);
                clr_cert.Issuer[certificate.nx_secure_x509_issuer.nx_secure_x509_common_name_length + 1] = 0;
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(hbReturnIssuer, clr_cert.Issuer));
                NANOCLR_CHECK_HRESULT(hbReturnIssuer.StoreToReference(stack.Arg1(), 0));

                memcpy(
                    clr_cert.Subject,
                    (void *)certificate.nx_secure_x509_subject_identifier,
                    certificate.nx_secure_x509_subject_identifier_length);
                clr_cert.Subject[certificate.nx_secure_x509_subject_identifier_length + 1] = 0;
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(hbReturnSubject, clr_cert.Subject));
                NANOCLR_CHECK_HRESULT(hbReturnSubject.StoreToReference(stack.Arg2(), 0));

                const UCHAR *StartDate = certificate.nx_secure_x509_not_before;
                st.wYear = (StartDate[0] - '0') * 1000 + (StartDate[1] - '0') * 100 + (StartDate[2] - '0') * 10 +
                           (StartDate[3] - '0');
                st.wMonth = (StartDate[4] - '0') * 10 + (StartDate[5] - '0');
                st.wDay = (StartDate[6] - '0') * 10 + (StartDate[7] - '0');
                st.wHour = (StartDate[8] - '0') * 10 + (StartDate[9] - '0');
                st.wMinute = (StartDate[10] - '0') * 10 + (StartDate[11] - '0');
                st.wSecond = (StartDate[12] - '0') * 10 + (StartDate[13] - '0');
                val = Library_corlib_native_System_DateTime::GetValuePtr(stack.Arg3());
                *val = HAL_Time_ConvertFromSystemTime(&st);

                const UCHAR *EndDate = certificate.nx_secure_x509_not_after;
                st.wYear =
                    (EndDate[0] - '0') * 1000 + (EndDate[1] - '0') * 100 + (EndDate[2] - '0') * 10 + (EndDate[3] - '0');
                st.wMonth = (EndDate[4] - '0') * 10 + (EndDate[5] - '0');
                st.wDay = (EndDate[6] - '0') * 10 + (EndDate[7] - '0');
                st.wHour = (EndDate[8] - '0') * 10 + (EndDate[9] - '0');
                st.wMinute = (EndDate[10] - '0') * 10 + (EndDate[11] - '0');
                st.wSecond = (EndDate[12] - '0') * 10 + (EndDate[13] - '0');
                val = Library_corlib_native_System_DateTime::GetValuePtr(stack.Arg4());
                *val = HAL_Time_ConvertFromSystemTime(&st);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2::
    DecodePrivateKeyNative___STATIC__VOID__SZARRAY_U1__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        (void)stack;
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }
    NANOCLR_NOCLEANUP();
}
