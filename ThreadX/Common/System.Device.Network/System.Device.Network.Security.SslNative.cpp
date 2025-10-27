//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "System.Device.Network.h"
#include "System.Device.Wifi.h"

NX_TCP_SOCKET *socket_ptr;
NX_SECURE_TLS_SESSION tls_session;
NX_PACKET *send_packet;
NX_PACKET *receive_packet;

static NX_SECURE_X509_CERT certificate;


HRESULT Library_sys_net_native_System_Net_Security_CertificateManager::
    AddCaCertificateBundle___STATIC__BOOLEAN__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint32_t certificateSize;
        uint32_t allocationSize;
        HAL_Configuration_X509CaRootBundle *caBundle = NULL;

        CLR_RT_HeapBlock_Array *arrayCA = stack.Arg0().DereferenceArray();

        // check for empty array
        FAULT_ON_NULL(arrayCA);

        certificateSize = (int)arrayCA->m_numOfElements;

        // build a HAL_Configuration_X509CaRootBundle
        // because certificate length is variable need to compute required memory
        // header
        allocationSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        // certificate
        allocationSize += certificateSize;

        // allocate memory
        caBundle = (HAL_Configuration_X509CaRootBundle *)platform_malloc(allocationSize);
        // sanity check
        FAULT_ON_NULL(caBundle);

        // fill in struct
        caBundle->CertificateSize = certificateSize;

        // copy from get a pointer to the the binary data for the certificate
        memcpy(caBundle->Certificate, arrayCA->GetFirstElement(), certificateSize);

        if (g_TargetConfiguration.CertificateStore->Count == 0)
        {
            // not found, add the certificate bundle
            // we only support one CA root bundle, so this is fixed to 0
            // block size doesn't matter
            // offset is 0 and done flag is true because this is being stored in a single chunk
            if (ConfigurationManager_StoreConfigurationBlock(
                    caBundle,
                    DeviceConfigurationOption_X509CaRootBundle,
                    0,
                    certificateSize,
                    0,
                    true) != TRUE)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }
        else
        {
            // update the configuration block
            // we only support one CA root bundle, so this is fixed to 0
            if (ConfigurationManager_UpdateConfigurationBlock(
                    caBundle,
                    DeviceConfigurationOption_X509CaRootBundle,
                    0) == UpdateConfigurationResult_Failed)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }

        // reach here, we should be OK
        stack.SetResult_Boolean(TRUE);

        if (caBundle != NULL)
        {
            platform_free(caBundle);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureServerInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

        CLR_RT_TypeDef_Index x509Certificate2TypeDef;

        CLR_INT32 sslContext = -1;
        CLR_INT32 sslMode = stack.Arg0().NumericByRef().s4;
        CLR_INT32 sslVerify = stack.Arg1().NumericByRef().s4;
        CLR_RT_HeapBlock *hbCert = stack.Arg2().Dereference();
        CLR_RT_HeapBlock *caCert = stack.Arg3().Dereference();
        bool useDeviceCertificate = (bool)stack.Arg4().NumericByRef().u1;
        CLR_RT_HeapBlock_Array *arrCert = NULL;
        CLR_RT_HeapBlock_Array *privateKey = NULL;
        CLR_UINT8 *sslCert = NULL;
        volatile int result;
        uint8_t *pk = NULL;
        const char *pkPassword = NULL;
        CLR_UINT32 pkPasswordLength = 0;

        if (hbCert != NULL)
        {
            g_CLR_RT_TypeSystem.FindTypeDef(
                "X509Certificate2",
                "System.Security.Cryptography.X509Certificates",
                x509Certificate2TypeDef);

            arrCert = hbCert[X509Certificate::FIELD___certificate].DereferenceArray();
            arrCert->Pin();

            // there is a client certificate, find if it's a X509Certificate2
            if (hbCert->ObjectCls().Type() == x509Certificate2TypeDef.Type())
            {
                // get private key
                privateKey = hbCert[X509Certificate2::FIELD___privateKey].DereferenceArray();

                // grab the first element, if there is a private key
                if (privateKey)
                {
                    pk = privateKey->GetFirstElement();

                    // get password field
                    CLR_RT_HeapBlock *passwordHb = hbCert[X509Certificate2::FIELD___password].Dereference();

                    // get password length, if there is a password
                    if (passwordHb)
                    {
                        pkPassword = passwordHb->StringText();
                        pkPasswordLength = hal_strlen_s(pkPassword);
                    }
                }
            }

            // get certificate
            sslCert = arrCert->GetFirstElement();
        }

        //result =
        //    (SSL_ServerInit(
        //         sslMode,
        //         sslVerify,
        //         (const char *)sslCert,
        //         sslCert == NULL ? 0 : arrCert->m_numOfElements,
        //         pk,
        //         pk == NULL ? 0 : privateKey->m_numOfElements,
        //         pkPassword,
        //         pkPasswordLength,
        //         sslContext,
        //         useDeviceCertificate)
        //         ? 0
        //         : -1);

        NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));

        if (caCert != NULL)
        {
            arrCert = caCert[X509Certificate::FIELD___certificate].DereferenceArray();

            // If arrCert == NULL then the certificate is an X509Certificate2 which uses a certificate handle
            if (arrCert == NULL)
            {
                arrCert = caCert[X509Certificate::FIELD___handle].DereferenceArray();
                FAULT_ON_NULL(arrCert);

                sslCert = arrCert->GetFirstElement();

                // pass the certificate handle as the data parameter
                result =
                    (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
            }
            else
            {

                arrCert->Pin();

                sslCert = arrCert->GetFirstElement();

                result =
                    (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
            }
        }

        stack.SetResult_I4(sslContext);

        if (FAILED(hr) && (sslContext != -1))
        {
            SSL_ExitContext(sslContext);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureClientInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

        CLR_RT_TypeDef_Index x509Certificate2TypeDef;

        CLR_INT32 sslContext = -1;
        CLR_INT32 sslMode = stack.Arg0().NumericByRef().s4;
        CLR_INT32 sslVerify = stack.Arg1().NumericByRef().s4;
        CLR_RT_HeapBlock *hbCert = stack.Arg2().Dereference();
        CLR_RT_HeapBlock *caCert = stack.Arg3().Dereference();
        bool useDeviceCertificate = (bool)stack.Arg4().NumericByRef().u1;
        CLR_RT_HeapBlock_Array *arrCert = NULL;
        CLR_RT_HeapBlock_Array *privateKey = NULL;
        CLR_UINT8 *sslCert = NULL;
        volatile int result;
        uint8_t *pk = NULL;
        const char *pkPassword = NULL;
        CLR_UINT32 pkPasswordLength = 0;

        if (hbCert != NULL)
        {
            g_CLR_RT_TypeSystem.FindTypeDef(
                "X509Certificate2",
                "System.Security.Cryptography.X509Certificates",
                x509Certificate2TypeDef);

            arrCert = hbCert[X509Certificate::FIELD___certificate].DereferenceArray();
            arrCert->Pin();

            // there is a client certificate, find if it's a X509Certificate2
            if (hbCert->ObjectCls().Type() == x509Certificate2TypeDef.Type())
            {
                // get private key
                privateKey = hbCert[X509Certificate2::FIELD___privateKey].DereferenceArray();

                // grab the first element, if there is a private key
                if (privateKey)
                {
                    pk = privateKey->GetFirstElement();

                    // get password field
                    CLR_RT_HeapBlock *passwordHb = hbCert[X509Certificate2::FIELD___password].Dereference();

                    // get password length, if there is a password
                    if (passwordHb)
                    {
                        pkPassword = passwordHb->StringText();
                        pkPasswordLength = hal_strlen_s(pkPassword);
                    }
                }
            }

            // get certificate
            sslCert = arrCert->GetFirstElement();
        }

        result =
            (SSL_ClientInit(
                 sslMode,
                 sslVerify,
                 (const char *)sslCert,
                 sslCert == NULL ? 0 : arrCert->m_numOfElements,
                 pk,
                 pk == NULL ? 0 : privateKey->m_numOfElements,
                 pkPassword,
                 pkPasswordLength,
                 sslContext,
                 useDeviceCertificate)
                 ? 0
                 : -1);

        NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));

        if (caCert != NULL)
        {
            arrCert = caCert[X509Certificate::FIELD___certificate].DereferenceArray();

            // If arrCert == NULL then the certificate is an X509Certificate2 which uses a certificate handle
            if (arrCert == NULL)
            {
                arrCert = caCert[X509Certificate::FIELD___handle].DereferenceArray();
                FAULT_ON_NULL(arrCert);

                sslCert = arrCert->GetFirstElement();

                // pass the certificate handle as the data parameter
                result =
                    (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
            }
            else
            {

                arrCert->Pin();

                sslCert = arrCert->GetFirstElement();

                result =
                    (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
            }
        }

        stack.SetResult_I4(sslContext);

        if (FAILED(hr) && (sslContext != -1))
        {
            SSL_ExitContext(sslContext);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureAccept___STATIC__VOID__I4__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

        int result = 0;
        bool fRes = true;
        CLR_INT64 *timeout;
        CLR_RT_HeapBlock hbTimeout;

        CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
        CLR_INT32 timeout_ms = -1; // wait forever

        CLR_RT_HeapBlock *socket_info = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket_info);
        socket_entry_t *socket_entry =
            (socket_entry_t *)socket_info[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                .NumericByRef()
                .s4;
        FAULT_ON_NULL(socket_entry);

        // Because we could have been a rescheduled call due to a prior call that would have blocked, we need to see
        // if our handle has been shutdown before continuing.
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
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureConnect___STATIC__VOID__I4__STRING__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int result;

        bool fRes = true;
        CLR_INT64 *timeout;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        CLR_RT_HeapBlock *hb = stack.Arg1().DereferenceString();
        FAULT_ON_NULL_ARG(hb);

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
            nx_tcp_client_socket_connect(socket_ptr, IP_ADDRESS(192, 168, 1, 100), SERVER_PORT, NX_WAIT_FOREVER);

            if (result == SOCK_EWOULDBLOCK || result == SOCK_TRY_AGAIN)
            {
                // non-blocking - allow other threads to run while we wait for socket activity
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
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureRead___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
#define BUFFER_SIZE 1024

    CLR_INT32 handle;

    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT32 totReadWrite;
        bool fRes = true;
        CLR_INT64 *timeout;
        int result = 0;
        CLR_UINT8 *buffer;

        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        FAULT_ON_NULL(arrData);

        CLR_INT32 offset = stack.Arg2().NumericByRef().s4;
        CLR_INT32 timeout_ms = stack.Arg4().NumericByRef().s4;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        CLR_INT32 count = stack.Arg3().NumericByRef().s4;
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
        totReadWrite = stack.m_evalStack[1].NumericByRef().s4;
        buffer = arrData->GetElement(offset + totReadWrite);
        count -= totReadWrite;
        if ((offset + count + totReadWrite) > (int)arrData->m_numOfElements)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }
        while (count > 0)
        {
            // first make sure we have data to read or ability to write
            //while (fRes)
            //{
            //    // check SSL_DataAvailable() in case SSL has already read and buffered socket data
            //    result = SSL_DataAvailable(handle);

            //    if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
            //    {
            //        break;
            //    }

            //    result = Library_sys_net_native_System_Net_Sockets_NativeSocket::Helper__SelectSocket(handle, 0);

            //    if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
            //    {
            //        break;
            //    }

            //    // non-blocking - allow other threads to run while we wait for socket activity
            //    NANOCLR_CHECK_HRESULT(
            //        g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));

            //    // timeout expired
            //    if (!fRes)
            //    {
            //        result = SOCK_SOCKET_ERROR;

            //        ThrowError(stack, SOCK_ETIMEDOUT);

            //        NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            //    }
            //}

            // socket is in the excepted state, so let's bail out
            //if (SOCK_SOCKET_ERROR == result)
            //{
            //    break;
            //}

            UINT status = nx_secure_tls_session_receive(&tls_session, &receive_packet, NX_WAIT_FOREVER);
            if (status == NX_SUCCESS)
            {
            }

            //    result = SSL_Read(handle, (char *)buffer, count);

            // if (result == SSL_RESULT__WOULD_BLOCK)
            //{
            //     continue;
            // }

            // ThrowOnError expects anything other than 0 to be a failure - so return 0 if we don't have an error
            if (result <= 0)
            {
                break;
            }

            buffer += result;
            totReadWrite += result;
            count -= result;

            // read is non-blocking if we have any data
            if (totReadWrite > 0)
            {
                break;
            }

            stack.m_evalStack[1].NumericByRef().s4 = totReadWrite;
        }

        stack.PopValue(); // totReadWrite
        stack.PopValue(); // Timeout

        if (result < 0)
        {
            NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
        }

        stack.SetResult_I4(totReadWrite);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureWrite___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    CLR_INT32 handle;
    NX_PACKET send_packet;

    NANOCLR_HEADER();
    {
        NX_PACKET **packet_ptr_ptr;

        CLR_RT_HeapBlock hbTimeout;
        CLR_INT32 totReadWrite;
        bool fRes = true;
        CLR_INT64 *timeout;
        int result = 0;
        CLR_UINT8 *buffer;

#define BUFFER_SIZE 1024

        UCHAR receive_buffer[BUFFER_SIZE];
        UINT bytes_received;
        UINT status;

        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        FAULT_ON_NULL(arrData);

        CLR_INT32 offset = stack.Arg2().NumericByRef().s4;
        CLR_INT32 timeout_ms = stack.Arg4().NumericByRef().s4;
        CLR_RT_HeapBlock *socket_info = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket_info);
        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        CLR_INT32 count = stack.Arg3().NumericByRef().s4;
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
        totReadWrite = stack.m_evalStack[1].NumericByRef().s4;
        buffer = arrData->GetElement(offset + totReadWrite);
        count -= totReadWrite;
        if ((offset + count + totReadWrite) > (int)arrData->m_numOfElements)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }
        while (count > 0)
        {
            // first make sure we have data to read or ability to write
            while (fRes)
            {
                // check SSL_DataAvailable() in case SSL has already read and buffered socket data
                result = SSL_DataAvailable(handle);

                if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                {
                    break;
                }

                result = Library_sys_net_native_System_Net_Sockets_NativeSocket::Helper__SelectSocket(handle, 1);

                if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                {
                    break;
                }

                // non-blocking - allow other threads to run while we wait for socket activity
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

            // Send data securely
            UINT status = nx_secure_tls_session_send(&tls_session, &send_packet, NX_WAIT_FOREVER);
            if (status == NX_SUCCESS)
            {
            }

            // ThrowOnError expects anything other than 0 to be a failure - so return 0 if we don't have an error
            if (result <= 0)
            {
                break;
            }

            buffer += result;
            totReadWrite += result;
            count -= result;

            stack.m_evalStack[1].NumericByRef().s4 = totReadWrite;
        }

        stack.PopValue(); // totReadWrite
        stack.PopValue(); // Timeout

        if (result < 0)
        {
            NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
        }

        stack.SetResult_I4(totReadWrite);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureCloseSocket___STATIC__I4__OBJECT(
    CLR_RT_StackFrame &stack)
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

        // Disconnect the TCP socket
        status = nx_tcp_socket_disconnect(socket_ptr, NX_WAIT_FOREVER);
        if (status == NX_SUCCESS)
        {

            // Delete the TLS session
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
        ////    CLR_RT_HeapBlock_String::CreateInstance(hbSubject, certificate.nx_secure_x509_subject_identifier));
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


void Library_sys_net_native_System_Net_Security_SslNative::ThrowError(CLR_RT_StackFrame &stack, int errorCode)
{
    NATIVE_PROFILE_CLR_NETWORK();
    CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

    if ((Library_corlib_native_System_Exception::CreateInstance(
            res,
            g_CLR_RT_WellKnownTypes.m_SocketException,
            CLR_E_FAIL,
            &stack)) == S_OK)
    {
        res.Dereference()[Library_sys_net_native_System_Net_Sockets_SocketException::FIELD___errorCode].SetInteger(
            errorCode);
    }
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::ThrowOnError(CLR_RT_StackFrame &stack, int res)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    if (res != 0)
    {
        ThrowError(stack, res);

        NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
    }

    NANOCLR_NOCLEANUP();
}
