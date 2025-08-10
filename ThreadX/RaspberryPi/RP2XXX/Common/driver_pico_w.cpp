
#include "pico/cyw43_arch.h"
#include "System.Device.Network.h"
#include "nx_api.h"

const char *WIFI_SSID = "198.0.0.1";
const char *WIFI_PASSWORD = "password";

#define WIFI_CONNECT_TIMEOUT 10000 /* tmieout in ms */
#define NX_DRIVER_STACK_SIZE 1024
#define WIFI_AUTH_TYPE       CYW43_AUTH_WPA_TKIP_PSK
#define NX_DRIVER_THREAD_INTERVAL (NX_IP_PERIODIC_RATE / 10)

void nx_driver_pico_w(NX_IP_DRIVER *driver_req_ptr);

static void pico_w_thread_entry(ULONG thread_input);
static uint32_t pico_w_initialize(NX_IP_DRIVER *driver_req_ptr);
static uint32_t pico_w_enable(NX_IP_DRIVER *driver_req_ptr);
static uint32_t pico_w_disable(NX_IP_DRIVER *driver_req_ptr);
static uint32_t pico_w_packet_send(NX_PACKET *packet_ptr);

static TX_THREAD nx_driver_pico_w_thread;
static UCHAR nx_driver_pico_w_thread_stack[NX_DRIVER_STACK_SIZE];
static UCHAR nx_driver_pico_w_link_up;
static UCHAR buffer[1514];

void nx_driver_pico_w(NX_IP_DRIVER *driver_req_ptr)
{
    static UCHAR start = NX_FALSE;
    if (!start)
    {
        nx_driver_hardware_initialize = pico_w_initialize;
        nx_driver_hardware_enable = pico_w_enable;
        nx_driver_hardware_disable = pico_w_disable;
        nx_driver_hardware_packet_send = pico_w_packet_send;

        start = NX_TRUE;
    }

    Network_Request_Processing(driver_req_ptr);
}
struct pbuf;
uint32_t pbuf_copy_partial(const struct pbuf *p, void *dataptr, uint32_t len, uint32_t offset)
{
    (void)p;
    (void)dataptr;
    (void)len;
    (void)offset;
        /* Stub function as it will never hit. */
        return 0;
}
static void pico_w_thread_entry(ULONG thread_input)
{
    NX_IP *ip_ptr = nx_driver_information.nx_driver_information_ip_ptr;

    NX_PARAMETER_NOT_USED(thread_input);

    for (;;)
    {
        if (nx_driver_information.nx_driver_information_ip_ptr->nx_ip_driver_link_up)
        {
            // Obtain the IP internal mutex before processing the IP event
            tx_mutex_get(&(ip_ptr->nx_ip_protection), TX_WAIT_FOREVER);
            {
                cyw43_arch_poll();
            }
            // Release the IP internal mutex before processing the IP event
            tx_mutex_put(&(ip_ptr->nx_ip_protection));
        }
        tx_thread_sleep(NX_DRIVER_THREAD_INTERVAL);
    }
}
int cyw43_tcpip_link_status(cyw43_t *self, int itf)
{
    if (nx_driver_pico_w_link_up)
        return CYW43_LINK_UP;
    else
        return cyw43_wifi_link_status(self, itf);
}
void cyw43_cb_tcpip_init(cyw43_t *self, int itf)
{
    if (!nx_driver_pico_w_link_up)
        return;
    nx_driver_information.nx_driver_information_ip_ptr->nx_ip_driver_link_up = NX_TRUE;
    _nx_ip_driver_link_status_event(
        nx_driver_information.nx_driver_information_ip_ptr,
        nx_driver_information.nx_driver_information_interface->nx_interface_index);
}
void cyw43_cb_tcpip_deinit(cyw43_t *self, int itf)
{
    if (!nx_driver_pico_w_link_up)
        return;
    nx_driver_information.nx_driver_information_ip_ptr->nx_ip_driver_link_up = NX_FALSE;
    _nx_ip_driver_link_status_event(
        nx_driver_information.nx_driver_information_ip_ptr,
        nx_driver_information.nx_driver_information_interface->nx_interface_index);
}
void cyw43_cb_tcpip_set_link_up(cyw43_t *self, int itf)
{
    nx_driver_pico_w_link_up = NX_TRUE;
}
void cyw43_cb_tcpip_set_link_down(cyw43_t *self, int itf)
{
    nx_driver_pico_w_link_up = NX_FALSE;
}
void cyw43_cb_process_ethernet(void *cb_data, int itf, size_t len, const uint32_t *buf)
{
    NX_PACKET *packet_ptr;
    if (!nx_driver_pico_w_link_up)
    {
        return;
    }

    // Allocate a packet to receive data
    if (nx_packet_allocate(
            nx_driver_information.nx_driver_information_packet_pool_ptr,
            &packet_ptr,
            NX_RECEIVE_PACKET,
            NX_NO_WAIT))
    {
        // No packet available
        return;
    }

    // Adjust packet starting address to align IP header to 4 bytes boundary
    packet_ptr->nx_packet_prepend_ptr += 2;
    packet_ptr->nx_packet_append_ptr += 2;

    // Append data to packet_ptr
    if (nx_packet_data_append(
            packet_ptr,
            (void *)buf,
            len,
            nx_driver_information.nx_driver_information_packet_pool_ptr,
            NX_NO_WAIT))
    {
        // Error, release the packet
        nx_packet_release(packet_ptr);
        return;
    }
    // Everything is OK, transfer the packet to NetX.
    nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, packet_ptr);
}
uint32_t pico_w_initialize(NX_IP_DRIVER *driver_req_ptr)
{
    UCHAR mac[6];
    uint32_t priority = 0;

    if (cyw43_arch_init())
    {
        return NX_DRIVER_ERROR;
    }
    cyw43_arch_enable_sta_mode();
    cyw43_hal_get_mac(0, mac);
    nx_driver_update_hardware_address(mac);
    // Get priority of IP thread.
    tx_thread_info_get(tx_thread_identify(), NX_NULL, NX_NULL, NX_NULL, &priority, NX_NULL, NX_NULL, NX_NULL, NX_NULL);

    // Create the driver thread
    // The priority of network thread is lower than IP thread
    tx_thread_create(
        &nx_driver_pico_w_thread,
        "Driver Thread",
        pico_w_thread_entry,
        0,
        nx_driver_pico_w_thread_stack,
        NX_DRIVER_STACK_SIZE,
        priority + 1,
        priority + 1,
        TX_NO_TIME_SLICE,
        TX_DONT_START);

    return NX_SUCCESS;
}
uint32_t pico_w_enable(NX_IP_DRIVER *driver_req_ptr)
{
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, WIFI_AUTH_TYPE, WIFI_CONNECT_TIMEOUT))
    {
        printf("failed to connect.\n");
        return NX_DRIVER_ERROR;
    }
    tx_thread_reset(&nx_driver_pico_w_thread);
    tx_thread_resume(&nx_driver_pico_w_thread);
    return NX_SUCCESS;
}
uint32_t pico_w_disable(NX_IP_DRIVER *driver_req_ptr)
{
    tx_thread_suspend(&nx_driver_pico_w_thread);
    tx_thread_terminate(&nx_driver_pico_w_thread);
    return NX_SUCCESS;
}
uint32_t pico_w_packet_send(NX_PACKET *packet_ptr)
{
    int ret;
    ULONG bytes_copied;
    if (packet_ptr->nx_packet_next)
    {
        if (packet_ptr->nx_packet_length > sizeof(buffer))
        {
            // Buffer too small to hold packet, release the packet
            NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
            nx_packet_transmit_release(packet_ptr);
            return NX_DRIVER_ERROR;
        }

        // Extract data from packet to contigous buffer
        if (nx_packet_data_retrieve(packet_ptr, buffer, &bytes_copied))
        {
            // Error, release the packet
            nx_packet_release(packet_ptr);
            return NX_DRIVER_ERROR;
        }
        ret = cyw43_send_ethernet(&cyw43_state, 0, bytes_copied, buffer, false);
    }
    else
    {
        ret = cyw43_send_ethernet(
            &cyw43_state,
            0,
            packet_ptr->nx_packet_length,
            packet_ptr->nx_packet_prepend_ptr,
            false);
    }
    NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
    nx_packet_transmit_release(packet_ptr);
    if (ret)
    {
        return NX_DRIVER_ERROR;
    }
    return NX_SUCCESS;
}
