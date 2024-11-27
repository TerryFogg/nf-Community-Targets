# Building a Bluetooth GATT Client on the Pi Pico W

### [V. Hunter Adams](https://vanhunteradams.com/)

## Purpose and intended audience

This webpage documents this [GATT client demonstration](https://github.com/vha3/Hunter-Adams-RP2040-Demos/tree/master/Bluetooth/GATT_Client), which issues requests to the GATT server discussed on a [separate webpage](https://vanhunteradams.com/Pico/BLE/GATT_Server.html). As discussed on that webpage, a GATT client has no prior knowledge of the information stored on the server to which it connects, but it does have an understanding of the overall structure and layout of that server. This shared understanding of the *organization* of information in a GATT database (into services, characteristics, etc.) allows for the client to ask questions that the server understands. By asking a series of questions ("which services do you offer?", "which characteristics are included in this service?", etc.) the client can build a local model of the server with which it's communicating.

This webpage describes the implementation of a client, demonstrated in the video below, that performs this querying process and that generates a command-line representation of the server's database using ASCII escape sequences. The client allows for the user to enable/disable notifications on characteristics, to issue write requests to characteristics that offer write permissions, and to issue read requests to the characteristics with read permissions. All of this is implemented using [BTstack](https://bluekitchen-gmbh.com/), the Bluetooth stack implementation used by the Pi Pico W. The webpage first discusses the characteristic discovery process, and then the client user interface.

These materials have been assembled for students in [ECE 4760](https://ece4760.github.io/), though I hope that they might be useful to hobbyists building a Bluetooth interface among Pi Pico W devices.

<iframe width="560" height="315" src="https://www.youtube.com/embed/-8GxgmlHbbQ?si=1dE_E-6GitkNhMf8" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen="" style="box-sizing: border-box;"></iframe>

## Page contents

> - [Prerequisite reading](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Prerequisite-reading)
>
> - Server discovery process
>
>   > - [Conceptual overview](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Conceptual-overview)
>   > - [Parsing GAP advertisements](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Parsing-GAP-advertisements)
>   > - [Primary service discovery](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Primary-service-discovery)
>   > - [Characteristic discovery](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Characteristic-discovery)
>   > - [Characteristic descriptor discovery](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Characteristic-descriptor-discovery)
>   > - [Characteristic user description discovery](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Characteristic-user-description-discovery)
>   > - [Characteristic value discovery](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Characteristic-value-discovery)
>   > - [Characteristic configuration discovery](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Characteristic-configuration-discovery)
>   > - [Receiving characteristic notifications](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Receiving-characteristic-notifications)
>
> - Client application
>
>   > - [Printing database contents](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Printing-database-contents)
>   > - [Command-line user interface](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Command-line-user-interface)
>
> - [Starting the client](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Starting-the-client)
>
> - Client application
>
>   > - [Printing database contents](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Printing-database-contents)
>   > - [Command-line user interface](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Command-line-user-interface)
>
> - [Starting the client](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Starting-the-client)
>
> - [Visual representation of the state machine](https://vanhunteradams.com/Pico/BLE/GATT_Client.html#Visual-representation-of-the-state-machine)

## Prerequisite reading

**Before you read this webpage**, please read the [webpage on the GATT server](https://vanhunteradams.com/Pico/BLE/GATT_Server.html). The server webpage provides a brief overview of the Bluetooth stack and describes the structure of a GATT database in detail. This document assumes that the reader already has an understanding of these topics, to the depth that the server webpage describes them.

## Server discovery process

Our GATT client builds a local model of the GATT server by asking it a series of questions. This "series of questions" takes the form of a state machine. Before getting into the details of this state machine, let us have a brief conceptual discussion about the nature of the questions that the client will ask the server. Hopefully, this provides a conceptual handle that we can grab onto as we wade into the weeds of implementation.

### Conceptual overview

The client receives GAP advertisements from peripheral devices, and searches those advertisements for the presence of the service that it's looking for. Once it finds it, it connects to the peripheral and builds its local model of the server. This process is summarized below. Each of the items in this enumerated list will be elaborated upon later in this document.

#### Before connection

1. **Does this GAP advertisement indicate that the peripheral device offers the service I'm looking for**? If yes, the client will initiate a connection process with the peripheral server. If not, it'll keep receiving and parsing GAP advertisements.

#### After connection

1. **Do you offer the service that I'm looking for?**. The client queries the server to confirm that it offers the service that it's looking for. The server responds with the start/end handles and UUID of the service.
2. **Please tell me every characteristic that this service contains.** The server will respond with a series of packets, each containing the start/end handles and UUID's of each characteristic in the specified service. This sequence ends with a packet that means "that's all of them!"
3. **Please tell me every descriptor that each characteristic contains.** For the characteristic specified by the client, the server responds with a sequence of packets that contain the UUID and handle of each descriptor (e.g. client characteristic configuration and user description) for that characteristic. This sequence ends with a packet that means "that's all of them!". The client will then ask the same question about the next characteristic, until it's gotten through all characteristics.
4. **Please tell me each characteristic's user description.** The client searches each characteristic's descriptors for the UUID associated with user description. It then queries the server for the value at the handle associated with that UUID in order to receive the character-array user description. The client repeats this for each characteristic until its gotten through all of them.
5. **Please tell me each characteristic's value.** For the characteristic specified by the client, the server responds with that characteristic's value. The client will then ask the same question about the next characteristic, until it's gotten through all of them.
6. **Please tell me the notification/indication status of each characteristic.**: The client searches each characteristic's descriptors for the UUID associated with characteristic configuration. If it finds it, it queries the server for the value associated with the client configuration characteristic. The client repeats this for each characteristic until its gotten through all of them.
7. **Signal that all characteristics have been discovered. The local model of the server is complete.**

You can think about each item in this list as filling in gaps in the local version of the attribute database that the client is building. As discussed on the [GATT server page](https://vanhunteradams.com/Pico/BLE/GATT_Server.html), these attribute databases are well reprsented as tables. The *particular* attribute database that this GATT client will reconstruct is copied below from the [GATT server page](https://vanhunteradams.com/Pico/BLE/GATT_Server.html). As we move through each step above, I will include a version of this table that only includes the information that the client has been able to fill in to that point.

The database that our client will discover:

| Length (bytes) | Access permissions                                       | Handle | Type                                                         | Value                                                        |
| :------------- | :------------------------------------------------------- | :----- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| 0x0A           | 0x02 (Read-only)                                         | 0x01   | 0x2800 (UUID for primary service)                            | 0x1800 (UUID for GAP service)                                |
| 0x0D           | 0x02 (Read-only)                                         | 0x02   | 0x2803 (UUID for characteristic declaration)                 | 0x2A00 (UUID for device name)                                |
| 0x13           | 0x02 (Read-only)                                         | 0x03   | 0x2A00 (UUID for device name)                                | "PICO_SERVER"                                                |
| 0x0A           | 0x02 (Read-only)                                         | 0x04   | 0x2800 (UUID for primary service)                            | 0x1801 (UUID for GATT service)                               |
| 0x0D           | 0x02 (Read-only)                                         | 0x05   | 0x2803 (UUID for characteristic declaration)                 | 0x2B2A (UUID for database hash)                              |
| 0x18           | 0x02 (Read-only)                                         | 0x06   | 0x2B2A (UUID for database hash)                              | [128-bit hash value computed from database structure]        |
| 0x18           | 0x02 (Read-only)                                         | 0x07   | 0x2800 (UUID for primary service)                            | 0x0000FF1000001000800000805F9B34FB (custom service UUID)     |
| 0x1B           | 0x02 (Read-only)                                         | 0x08   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic A |
| 0x16           | 0x302 (Read-only, dynamic, long UUID)                    | 0x09   | 0x0000FF1100001000800000805F9B34FB (custom characteristic UUID) | [No value, dynamic characteristic!]                          |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0A   | 0x2902 (UUID for client characteristic configuration)        | 0x0000 (notifications/indications off)                       |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0B   | 0x2901 (UUID for characteristic user description)            | [No value, dynamic characteristic!]                          |
| 0x1B           | 0x02 (Read-only)                                         | 0x0C   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic B |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x0D   | 0x0000FF1200001000800000805F9B34FB (custom characteristic UUID) | [No value, dynamic characteristic!]                          |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0E   | 0x2902 (UUID for client characteristic configuration)        | 0x0000 (notifications/indications off)                       |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0F   | 0x2901 (UUID for characteristic user description)            | [No value, dynamic characteristic!]                          |
| 0x1B           | 0x02 (Read-only)                                         | 0x10   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic C |
| 0x16           | 0x302 (Read, dynamic, long UUID)                         | 0x11   | 0x0000FF1300001000800000805F9B34FB (custom characteristic UUID) | [No value, dynamic characteristic!]                          |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x12   | 0x2902 (UUID for client characteristic configuration)        | 0x0000 (notifications/indications off)                       |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x13   | 0x2901 (UUID for characteristic user description)            | [No value, dynamic characteristic!]                          |
| 0x1B           | 0x02 (Read-only)                                         | 0x14   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic D |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x15   | 0x0000FF1400001000800000805F9B34FB (custom characteristic UUID) | [No value, dynamic characteristic!]                          |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x16   | 0x2902 (UUID for client characteristic configuration)        | 0x0000 (notifications/indications off)                       |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x17   | 0x2901 (UUID for characteristic user description)            | [No value, dynamic characteristic!]                          |
| 0x1B           | 0x02 (Read-only)                                         | 0x18   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic E |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x19   | 0x0000FF1500001000800000805F9B34FB (custom characteristic UUID) | [No value, dynamic characteristic!]                          |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1A   | 0x2901 (UUID for characteristic user description)            | [No value, dynamic characteristic!]                          |
| 0x1B           | 0x02 (Read-only)                                         | 0x1B   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic F |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x1C   | 0x0000FF1600001000800000805F9B34FB (custom characteristic UUID) | [No value, dynamic characteristic!]                          |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1D   | 0x2901 (UUID for characteristic user description)            | [No value, dynamic characteristic!]                          |

### Parsing GAP advertisements

There may be many peripheral devices sending GAP advertisements that the client will receive. The client chooses a peripheral for connection based on the contents of its advertising packet. In particular, it looks for an advertisement packet that contains the AD Type associated with a complete list of 16-bit service class UUID's, and that has the 16-bit UUID 0xFF10 associated with it. See [here](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Building-a-GAP-advertising-packet) for a description of the advertising packet construction on the server side, and see below for the code which we use to parse the received packets for the custom service.

This function takes, as its arguments, a 16-bit value which represents the UUID for which it is searching and a pointer to the character array that is the received advertising packet. It parses the packet's AD Types, sizes, and data, and it returns true in the event that it finds the AD Type and data that we're looking for.

```
static bool advertisement_report_contains_service(uint16_t service, uint8_t *advertisement_report){
    // get advertisement from report event
    const uint8_t * adv_data = gap_event_advertising_report_get_data(advertisement_report);
    uint8_t adv_len  = gap_event_advertising_report_get_data_length(advertisement_report);

    // iterate over advertisement data
    ad_context_t context;
    for (ad_iterator_init(&context, adv_len, adv_data) ; ad_iterator_has_more(&context) ; ad_iterator_next(&context)){
        uint8_t data_type = ad_iterator_get_data_type(&context);
        uint8_t data_size = ad_iterator_get_data_len(&context);
        const uint8_t * data = ad_iterator_get_data(&context);
        switch (data_type){
            case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS:
                for (int i = 0; i < data_size; i += 2) {
                    uint16_t type = little_endian_read_16(data, i);
                    if (type == service) return true;
                }
            default:
                break;
        }
    }
    return false;
}
```

The context from which this function gets called is the HCI packet handler. This is the packet handler which manages events being emitted from the HCI layer of the Bluetooth stack, and this is where our discussion of the GATT client state machine begins. This packet handler manages events which occur *before* connection with a peripheral device. After the system boots up, a `BTSTACK_EVENT_STATE` event will be emitted and received by the packet handler. Upon receipt, the packet handler starts the client (initializes `state` to `TC_W4_SCAN_RESULT` and starts the GAP scanner). This packet handler then manages events associated with GAP advertising reports. As you can see, it confirms that the system is in the correct state, then calls the helper function above to parse the received advertisement for the custom service UUID. If it finds it, then it gathers the address of the sender, performs a state transition to `TC_W4_CONNECT`, stops the GAP scanner, and instantiates the connection process with the peripheral.

The subsequent state waits for the connection process to complete before retrieving the HCI connection handle, performing a state transition, and asking the first question to our server. The client calls `gatt_client_discover_primary_services_by_uuid128(handle_gatt_client_event, connection_handle, service_name);`. This is a request to the server to confirm that it offers the primary service specified by the 128-bit custom UUID that we pass in as the third argument. We will parse the server's response in our GATT packet handler, where the state machine continues. We also perform a state transition to `TC_W4_SERVICE_RESULT`.

(Please note that the final case in the packet handler below manages disconnection events).

```
// Callback function for HCI events
static void hci_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;
    if (packet_type != HCI_EVENT_PACKET) return;

    // Retrieve event type
    uint8_t event_type = hci_event_packet_get_type(packet);
    switch(event_type){
        // Starting up
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                gap_local_bd_addr(local_addr);
                printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
                client_start();
            } else {
                state = TC_OFF;
            }
            break;
        // We've received an advertising report!
        case GAP_EVENT_ADVERTISING_REPORT:
            // We're placed into this state by client_start(), above
            if (state != TC_W4_SCAN_RESULT) return;
            // Confirm that this advertising report includes our custom service UUID
            if (!advertisement_report_contains_service(CUSTOM_SERVICE, packet)) return;
            // Store the address of the sender, and the type of the server
            gap_event_advertising_report_get_address(packet, server_addr);
            server_addr_type = gap_event_advertising_report_get_address_type(packet);
            // Perform a state transition
            state = TC_W4_CONNECT;
            // Stop scanning
            gap_stop_scan();
            // Print a little message
            printf("Connecting to device with addr %s.\n", bd_addr_to_str(server_addr));
            // Connect to the server that sent the advertising report
            gap_connect(server_addr, server_addr_type);
            break;
        case HCI_EVENT_LE_META:
            // Wait for connection complete
            switch (hci_event_le_meta_get_subevent_code(packet)) {
                case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                    // Confirm that we are in the proper state
                    if (state != TC_W4_CONNECT) return;
                    // Retrieve connection handle from packet
                    connection_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
                    // initialize gatt client context with handle, and add it to the list of active clients
                    // query primary services
                    // Search for the custom service which was advertised
                    DEBUG_LOG("Search for custom service.\n");
                    // Perform state transition (ATT state machine above)
                    state = TC_W4_SERVICE_RESULT;
                    // Search for our custom service by its 128-bit UUID
                    gatt_client_discover_primary_services_by_uuid128(handle_gatt_client_event, connection_handle, service_name);
                    break;
                default:
                    break;
            }
            break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            // unregister listener
            connection_handle = HCI_CON_HANDLE_INVALID;
            if (listener_registered){
                listener_registered = false;
                gatt_client_stop_listening_for_characteristic_value_updates(&notification_listener);
            }
            printf("Disconnected %s\n", bd_addr_to_str(server_addr));
            // Turn off the LED to indicate disconnection to server
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            // Initialize relevant variables
            k = 0 ;
            k2 = 0 ;
            num_characteristics = 0 ;
            if (state == TC_OFF) break;
            // Start looking for another server
            client_start();
            break;
        default:
            break;
    }
}
```

### Primary service discovery

After we send a primary service discovery request to the server, the server will respond with the start/end handles and UUID of the service that we've asked about. This response comes in the form of a packet, which gets emitted to our GATT client event packet handler. This packet uses the BTstack API function `hci_event_packet_get_type(packet) ;` to extract the *type* of the packet, and then it implements a state machine based on that packet type.

The full state machine is linked [here](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/5837ed8c1799df085b58ed3549ac6253e9c861c0/Bluetooth/GATT_Client/client.c#L144). In order to avoid confusing this discussion with massive blocks of code, I'll only include the relevant pieces of this state machine inline in this document. In the snippet below, you'll note that the GATT client event packet handler is called by BTstack with the HCI channel identifier, a pointer to the packet (a character array), and the size of the packet. Before entering the state machine, the packet handler extracts the packet *type* from the packet. Then, it enters a state machine and takes an action based on its present state. Recall that the HCI packet handler placed us into state `TC_W4_SERVICE_RESULT`.

The GATT server will actually respond to our our service discovery request with *two* packets. The first is of type `GATT_EVENT_SERVICE_QUERY_RESULT`, and the packet payload contains the information of relevance about the primary service. We store this information in an object of type `gatt_client_service_t`, a BTstack structure with fields for the start/end handles of the service and its UUID. The second packet that we receive will be of type `GATT_EVENT_QUERY_COMPLETE`, which indicates that the server has finished the previous request. In this case, we confirm that we haven't had any ATT errors, turn on an LED, initialize some variables, and perform a state transition to `TC_W4_CHARACTERISTIC_RESULT`. Importantly, we *also* issue a second request to the server. This one is `gatt_client_discover_characteristics_for_service(handle_gatt_client_event, connection_handle, &server_service);`, which translates to "please tell me all the characteristics that the primary service contains.

```
// Callback function which manages GATT events. Implements a state machine.
static void handle_gatt_client_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(packet_type);
    UNUSED(channel);
    UNUSED(size);

    // Local variables for storing ATT status and type of packet
    uint8_t att_status;
    uint8_t type_of_packet ;

    // What type of packet did we just receive?
    type_of_packet = hci_event_packet_get_type(packet) ;

    .
    .
    .

    // Packet was not a notification, enter state machine
    switch(state){

        // We've been placed into this state by the HCI callback machine, which
        // has connected to a GATT server and has queried for our custom service UUID
        case TC_W4_SERVICE_RESULT:
            switch(type_of_packet) {
                // This packet contains the result of the service query. Store the service.
                case GATT_EVENT_SERVICE_QUERY_RESULT:
                    // store service (we expect only one)
                    DEBUG_LOG("Storing service\n");
                    gatt_event_service_query_result_get_service(packet, &server_service);
                    break;

                // Finished with query, look for our custom characteristic
                case GATT_EVENT_QUERY_COMPLETE:
                    // Check ATT status to make sure we don't have any errors
                    att_status = gatt_event_query_complete_get_att_status(packet);
                    if (att_status != ATT_ERROR_SUCCESS){
                        printf("SERVICE_QUERY_RESULT, ATT Error 0x%02x.\n", att_status);
                        gap_disconnect(connection_handle);
                        break;  
                    } 
                    // Turn on the LED to indicate connection to server
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
                    // Clear all notification information
                    memset(notifications_enabled, -1, MAX_CHARACTERISTICS) ;
                    // Transition to next state
                    state = TC_W4_CHARACTERISTIC_RESULT;
                    DEBUG_LOG("Search for counting characteristic.\n");
                    // Discover all characteristics contained within custom service.
                    gatt_client_discover_characteristics_for_service(handle_gatt_client_event, connection_handle, &server_service);
                    break;

                default:
                    break;
            }
            break;

            .
            .
            .

            }
    .
    .
    .

    }
}
```

As we transition out of this state, after having made a connection to the server and issuing the primary service discovery request, our understanding of the contents of the remote database looks like that which is shown below. Via connection, we've discovered the GAP service and the device name characteristic contained therein. After our primary service discovery, we also know the handle for our custom primary service of interest. We haven't discovered the GATT service or database hash characteristic, so I've omitted them from the table below.

| Length (bytes) | Access permissions | Handle | Type                                         | Value                                                    |
| :------------- | :----------------- | :----- | :------------------------------------------- | :------------------------------------------------------- |
| 0x0A           | 0x02 (Read-only)   | 0x01   | 0x2800 (UUID for primary service)            | 0x1800 (UUID for GAP service)                            |
| 0x0D           | 0x02 (Read-only)   | 0x02   | 0x2803 (UUID for characteristic declaration) | 0x2A00 (UUID for device name)                            |
| 0x13           | 0x02 (Read-only)   | 0x03   | 0x2A00 (UUID for device name)                | "PICO_SERVER"                                            |
| 0x18           | 0x02 (Read-only)   | 0x07   | 0x2800 (UUID for primary service)            | 0x0000FF1000001000800000805F9B34FB (custom service UUID) |

### Characteristic discovery

The server responds to our primary service characteristic discovery request with a sequence of packets, each of type `GATT_EVENT_CHARACTERISTIC_QUERY_RESULT`. Each of these packets contains the start/end handles and UUID's of one of the characteristics contained in the primary service. The final packet that we receive will again be of type `GATT_EVENT_QUERY_COMPLETE` to indicate that the server is finished responding to our discovery request. As we receive each characteristic's start/end handles and UUID's, we store that information in an object of type `gatt_client_characteristic_t`. This BTstack object contains fields for the start/end handles of each of our characteristics, each characteristic's value handle, its properties (does the characteristic allow for notification/indication?), and its UUID.

The relevant state from the state machine is included below. We log each characteristic that we discover in an array of `gatt_client_characteristic_t` objects called `server_characteristic`. Once we receive the `GATT_EVENT_QUERY_COMPLETE` packet, we store the number of characteristics that we discovered, zero a couple variables, confirm that we haven't had any ATT errors, and then issue a characteristic descriptor discovery request for the *first* characteristic in our list. Finally, we transition state to `TC_W4_CHARACTERISTIC_DESCRIPTOR`.

```
// We've found the custom service, and we've just sent a query for all characteristics.
// We don't know how many characteristics this service contains, we need to discover them.
case TC_W4_CHARACTERISTIC_RESULT:
    switch(type_of_packet) {
        // This packet contains information about a characteristic in the custom service. We'll
        // keep receiving these packets until we've gotten one for each service. Then we'll get
        // a GATT_EVENT_QUERY_COMPLETE packet.
        case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
            // Store the characteristic in a gatt_client_characteristic_t object, and increment to the next
            // element in an array of such objects.
            DEBUG_LOG("Storing characteristic\n");
            gatt_event_characteristic_query_result_get_characteristic(packet, &server_characteristic[k++]);
            break;

        // We've received all characteristics!
        case GATT_EVENT_QUERY_COMPLETE:
            // How many characteristics did we just discover?
            num_characteristics = k ;

            // Reset our two incrementers
            k  = 0 ;
            k2 = 0 ;

            // Check the ATT status for errors
            att_status = gatt_event_query_complete_get_att_status(packet);
            if (att_status != ATT_ERROR_SUCCESS){
                printf("CHARACTERISTIC_QUERY_RESULT, ATT Error 0x%02x.\n", att_status);
                gap_disconnect(connection_handle);
                break;  
            } 

            // Clear the terminal screen
            printf("\033[2J") ;

            // Discover all characteristic descriptors for all characteristics. starting with number "k" (which
            // has been reset to 0 above).
            gatt_client_discover_characteristic_descriptors(handle_gatt_client_event, connection_handle, &server_characteristic[k]) ;

            // State transition
            state = TC_W4_CHARACTERISTIC_DESCRIPTOR ;
            break;

        default:
            break;
    }
    break;
```

Upon leaving this state, we have discovered the *handles* for each of our characteristics and each of our characteristic values, but we've not yet learned the descriptors for those characteristics, or their value. At this point, our understanding of the remote database looks like the following. We know the handles and access permissions for our characteristics, but we don't yet know their values or descriptors.

| Length (bytes) | Access permissions                                       | Handle | Type                                                         | Value                                                        |
| :------------- | :------------------------------------------------------- | :----- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| 0x0A           | 0x02 (Read-only)                                         | 0x01   | 0x2800 (UUID for primary service)                            | 0x1800 (UUID for GAP service)                                |
| 0x0D           | 0x02 (Read-only)                                         | 0x02   | 0x2803 (UUID for characteristic declaration)                 | 0x2A00 (UUID for device name)                                |
| 0x13           | 0x02 (Read-only)                                         | 0x03   | 0x2A00 (UUID for device name)                                | "PICO_SERVER"                                                |
| 0x18           | 0x02 (Read-only)                                         | 0x07   | 0x2800 (UUID for primary service)                            | 0x0000FF1000001000800000805F9B34FB (custom service UUID)     |
| 0x1B           | 0x02 (Read-only)                                         | 0x08   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic A |
| 0x16           | 0x302 (Read-only, dynamic, long UUID)                    | 0x09   | 0x0000FF1100001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x0C   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic B |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x0D   | 0x0000FF1200001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x10   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic C |
| 0x16           | 0x302 (Read, dynamic, long UUID)                         | 0x11   | 0x0000FF1300001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x14   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic D |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x15   | 0x0000FF1400001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x18   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic E |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x19   | 0x0000FF1500001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x1B   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic F |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x1C   | 0x0000FF1600001000800000805F9B34FB (custom characteristic UUID) |                                                              |

### Characteristic descriptor discovery

For *each* characteristic descriptor discovery request that we issue, the server responds to our primary service characteristic discovery request with a sequence of packets, each of type GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT. Each of these packets contains the handles and UUID's of one of the characteristic descriptors contained in the characteristic in question. The final packet that we receive will again be of type GATT_EVENT_QUERY_COMPLETE to indicate that the server is finished responding to our discovery request. As we receive each characteristic descriptor's handle and UUID, we store that information in an object of type gatt_client_characteristic_descriptor_t. This BTstack object contains fields for the handle of each of our characteristic descriptors and its UUID.

We will issue one of these requests for *each* of the characteristics that we discovered in the previous state. The relevant piece of the state machine is included below. After we have learned the descriptors for every characteristic, we go *back* to the first one and issue a `gatt_client_read_characteristic_descriptor` request to the descriptor with the UUID associated with the characteristic user description. We also state transition to `TC_W4_CHARACTERISTIC_DESCRIPTOR_PRINT`.

```
// We're querying each characteristic for its descriptors.
case TC_W4_CHARACTERISTIC_DESCRIPTOR:
    switch(type_of_packet) {
        // We've just received another descriptor for characteristic k. Store it in an array of gatt_characteristic_descriptor objects, and then
        // increment to the next element in an array of such objects.
        case GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT:
            DEBUG_LOG("Storing characteristic descriptor\n") ;
            gatt_event_all_characteristic_descriptors_query_result_get_characteristic_descriptor(packet, &server_characteristic_descriptor[k][k2++]) ;
            break ;

        // We've received all descriptors for characteristic k.
        case GATT_EVENT_QUERY_COMPLETE:
            DEBUG_LOG("Transitioning to Descriptor Print\n") ;
            // Increment k to the next characteristic
            k++ ;
            // Reset k2 (used to count characteristic descriptors) back to 0.
            k2 = 0 ;
            // If we haven't yet queried all of the characteristics that we discovered, query the next characteristic for all its descriptors.
            // We stay in this state to receive these next descriptors.
            if (k < num_characteristics) {
                gatt_client_discover_characteristic_descriptors(handle_gatt_client_event, connection_handle, &server_characteristic[k]) ;
            }
            // If we have received all of the descriptors for all of our characteristics . . .
            else {
                // Reset our characteristic counter to 0.
                k = 0 ;
                // For characteristic k (reset to 0), query the value of the descriptor with UUID CHARACTERISTIC_USER_DESCRIPTION.
                // This descriptor contains the plaintext user description of this characteristic. We're not sure if this is the first or the second
                // descriptor for the characteristic, so check the UUID of each and read the correct one.
                if (server_characteristic_descriptor[k][0].uuid16 == CHARACTERISTIC_USER_DESCRIPTION) {
                    gatt_client_read_characteristic_descriptor(handle_gatt_client_event, connection_handle, &server_characteristic_descriptor[k][0]) ;
                }
                else {
                    gatt_client_read_characteristic_descriptor(handle_gatt_client_event, connection_handle, &server_characteristic_descriptor[k][1]) ;
                }
                // Move to next state.
                state = TC_W4_CHARACTERISTIC_DESCRIPTOR_PRINT ;
            }
            break ;

        default:
            break ;
    }
    break ;
```

Upon leaving this state, we have learned the UUID's and handles associated with all characteristic descriptors. Our understanding of the remote database now looks like the following. We know the handles of all the characteristics and their descriptors, but we've yet to learn the values of any of these attributes.

| Length (bytes) | Access permissions                                       | Handle | Type                                                         | Value                                                        |
| :------------- | :------------------------------------------------------- | :----- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| 0x0A           | 0x02 (Read-only)                                         | 0x01   | 0x2800 (UUID for primary service)                            | 0x1800 (UUID for GAP service)                                |
| 0x0D           | 0x02 (Read-only)                                         | 0x02   | 0x2803 (UUID for characteristic declaration)                 | 0x2A00 (UUID for device name)                                |
| 0x13           | 0x02 (Read-only)                                         | 0x03   | 0x2A00 (UUID for device name)                                | "PICO_SERVER"                                                |
| 0x18           | 0x02 (Read-only)                                         | 0x07   | 0x2800 (UUID for primary service)                            | 0x0000FF1000001000800000805F9B34FB (custom service UUID)     |
| 0x1B           | 0x02 (Read-only)                                         | 0x08   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic A |
| 0x16           | 0x302 (Read-only, dynamic, long UUID)                    | 0x09   | 0x0000FF1100001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0A   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0B   | 0x2901 (UUID for characteristic user description)            |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x0C   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic B |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x0D   | 0x0000FF1200001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0E   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0F   | 0x2901 (UUID for characteristic user description)            |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x10   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic C |
| 0x16           | 0x302 (Read, dynamic, long UUID)                         | 0x11   | 0x0000FF1300001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x12   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x13   | 0x2901 (UUID for characteristic user description)            |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x14   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic D |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x15   | 0x0000FF1400001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x16   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x17   | 0x2901 (UUID for characteristic user description)            |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x18   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic E |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x19   | 0x0000FF1500001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1A   | 0x2901 (UUID for characteristic user description)            |                                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x1B   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic F |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x1C   | 0x0000FF1600001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1D   | 0x2901 (UUID for characteristic user description)            |                                                              |

### Characteristic user description discovery

The server responds to each `gatt_client_read_characteristic_descriptor` request that we issue with two packets. The first is of type `GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT` and contains the user description (a plaintext character array description of the characteristic), and the second is of type `GATT_EVENT_QUERY_COMPLETE` to indicate that the server has finished that request. The client issues one of these requests for *each* of the characteristics, thereby learning the value of each characteristic user description attribute.

After getting through all characteristics, the state machine resets its iterating variables to return to the *first* characteristic in our list. It changes the state to `TC_W4_CHARACTERISTIC_VALUE_PRINT`, and issues the request `gatt_client_read_value_of_characteristic` for the first characteristic.

```
// We'd like to locally store the user descriptions for each characteristic. We'll do so in this state.
        case TC_W4_CHARACTERISTIC_DESCRIPTOR_PRINT:
            switch(type_of_packet) {
                // The result of the characteristic descriptor query for the CHARACTERISTIC_USER_DESCRIPTION. Parse this packet,
                // and store the contents in a global array of descriptors.
                case GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT:
                    DEBUG_LOG("Storing characteristic descriptor\n") ;
                    // The user description is a character array. How long is it?
                    descriptor_length = gatt_event_characteristic_descriptor_query_result_get_descriptor_length(packet);
                    // What is the address of the start of this array?
                    const uint8_t *descriptor = gatt_event_characteristic_descriptor_query_result_get_descriptor(packet);

                    // Store the user description in a global array of characteristic descriptions.
                    memcpy(server_characteristic_user_description[k], descriptor, descriptor_length) ;
                    // Null-terminate the character array.
                    server_characteristic_user_description[k][descriptor_length] = 0 ;

                    break ;

                // Finished with read for this particular characteristic, do we need to read more?
                case GATT_EVENT_QUERY_COMPLETE:
                    // Increment k to the next characteristic
                    k++ ;
                    // Are there still characteristics remaining? If so . . .
                    if (k < num_characteristics) {
                        // Read the user description for the next characteristic, just like before.
                        if (server_characteristic_descriptor[k][0].uuid16 == CHARACTERISTIC_USER_DESCRIPTION) {
                            gatt_client_read_characteristic_descriptor(handle_gatt_client_event, connection_handle, &server_characteristic_descriptor[k][0]) ;
                        }
                        else {
                            gatt_client_read_characteristic_descriptor(handle_gatt_client_event, connection_handle, &server_characteristic_descriptor[k][1]) ;
                        }
                    }
                    // If not . . .
                    else {
                        // Reset both the characteristic and characteristic descriptor iterators
                        k   = 0 ;
                        k2  = 0 ;
                        // Read the value of characteristic k (reset to 0)
                        gatt_client_read_value_of_characteristic(handle_gatt_client_event, connection_handle, &server_characteristic[k]) ;
                        // Transition state
                        state = TC_W4_CHARACTERISTIC_VALUE_PRINT ;
                    }
                    break ;

                default:
                    break ;
            }
            break ;
```

Upon leaving this state, we've learned the values for each of our characteristic descriptor attributes:

| Length (bytes) | Access permissions                                       | Handle | Type                                                         | Value                                                        |
| :------------- | :------------------------------------------------------- | :----- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| 0x0A           | 0x02 (Read-only)                                         | 0x01   | 0x2800 (UUID for primary service)                            | 0x1800 (UUID for GAP service)                                |
| 0x0D           | 0x02 (Read-only)                                         | 0x02   | 0x2803 (UUID for characteristic declaration)                 | 0x2A00 (UUID for device name)                                |
| 0x13           | 0x02 (Read-only)                                         | 0x03   | 0x2A00 (UUID for device name)                                | "PICO_SERVER"                                                |
| 0x18           | 0x02 (Read-only)                                         | 0x07   | 0x2800 (UUID for primary service)                            | 0x0000FF1000001000800000805F9B34FB (custom service UUID)     |
| 0x1B           | 0x02 (Read-only)                                         | 0x08   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic A |
| 0x16           | 0x302 (Read-only, dynamic, long UUID)                    | 0x09   | 0x0000FF1100001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0A   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0B   | 0x2901 (UUID for characteristic user description)            | "Read-only counter"                                          |
| 0x1B           | 0x02 (Read-only)                                         | 0x0C   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic B |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x0D   | 0x0000FF1200001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0E   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0F   | 0x2901 (UUID for characteristic user description)            | "DDS Frequency"                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x10   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic C |
| 0x16           | 0x302 (Read, dynamic, long UUID)                         | 0x11   | 0x0000FF1300001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x12   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x13   | 0x2901 (UUID for characteristic user description)            | "String from Pico"                                           |
| 0x1B           | 0x02 (Read-only)                                         | 0x14   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic D |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x15   | 0x0000FF1400001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x16   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x17   | 0x2901 (UUID for characteristic user description)            | "String to Pico"                                             |
| 0x1B           | 0x02 (Read-only)                                         | 0x18   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic E |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x19   | 0x0000FF1500001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1A   | 0x2901 (UUID for characteristic user description)            | "LED Status and Control"                                     |
| 0x1B           | 0x02 (Read-only)                                         | 0x1B   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic F |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x1C   | 0x0000FF1600001000800000805F9B34FB (custom characteristic UUID) |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1D   | 0x2901 (UUID for characteristic user description)            | "Color Selection"                                            |

### Characteristic value discovery

The server responds to each gatt_client_read_value_of_characteristic request that we issue with two packets. The first is of type GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT and contains the characteristic value, and the second is of type GATT_EVENT_QUERY_COMPLETE to indicate that the server has finished that request. The client issues one of these requests for each of the characteristics, thereby learning the value of each characteristic.

After getting through all characteristics, the state machine resets its iterating variables to return to the first characteristic in our list. It searches the characteristic descriptors for occurrences of the characteristic user configuration descriptor and, when it finds it, it issues a read request to that characteristic and transition state to TC_W4_CHARACTERISTIC_NOTIFICATIONS. If it never finds such a characteristic descriptor, it transitions to state TC_W4_READY, where it remains.

```
// We'd like to read the value of each characteristic. We'll do so in this state.
        case TC_W4_CHARACTERISTIC_VALUE_PRINT:
            switch(type_of_packet) {

                // We've just received the result of a characteristic value query. This packet contains that value
                case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
                    DEBUG_LOG("Storing characteristic value\n") ;
                    // The characteristic value is a character array. How long is it?
                    descriptor_length = gatt_event_characteristic_value_query_result_get_value_length(packet);
                    // What is the address to the start of this array?
                    const uint8_t *descriptor = gatt_event_characteristic_value_query_result_get_value(packet);

                    // Store the characteristic value in a global array of characteristic values
                    memcpy(server_characteristic_values[k], descriptor, descriptor_length) ;
                    // Null-terminate the character array.
                    server_characteristic_values[k][descriptor_length] = 0 ;
                    break ;

                // We've finished receiving the value for a particular characteristic
                case GATT_EVENT_QUERY_COMPLETE:
                    // Move to the next characteristic
                    k++ ;
                    // Do we still have more characteristics to query for value? If so . . .
                    if (k < num_characteristics) {
                        // Read the next one!
                        gatt_client_read_value_of_characteristic(handle_gatt_client_event, connection_handle, &server_characteristic[k]) ;
                    }
                    // If not . . .
                    else {
                        k = 0 ;
                        k2 = 0 ;

                        // Find the next characteristic with a descriptor.
                        while((k<num_characteristics) && (server_characteristic_descriptor[k][0].uuid16!=CHARACTERISTIC_CONFIGURATION)) {
                            k++ ;
                        }

                        // Have we gone thru all characteristics? If not, read the next configuration
                        if (k<num_characteristics) {
                            DEBUG_LOG("QUERYING FOR CONFIGURATION: %d\n", k) ;
                            gatt_client_read_characteristic_descriptor(handle_gatt_client_event, connection_handle, &server_characteristic_descriptor[k][0]) ;
                            // Transition state
                            state = TC_W4_CHARACTERISTIC_NOTIFICATIONS ;
                        }
                        // If so, move to next state
                        else {
                            // Signal thread that all characteristics have been acquired
                            PT_SEM_SAFE_SIGNAL(pt, &characteristics_discovered) ;

                            // Transition to an idle state
                            state = TC_W4_READY ;

                            // Turn on notifications
                            // Register handler for notifications
                            listener_registered = true;
                            gatt_client_listen_for_characteristic_value_updates(&notification_listener, handle_gatt_client_event, connection_handle, NULL);
                        }
                    }
                    break ;

                default:
                    break ;

            }
            break ;
```

Upon leaving this state, the client's understanding of the remote server looks like the following:

| Length (bytes) | Access permissions                                       | Handle | Type                                                         | Value                                                        |
| :------------- | :------------------------------------------------------- | :----- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| 0x0A           | 0x02 (Read-only)                                         | 0x01   | 0x2800 (UUID for primary service)                            | 0x1800 (UUID for GAP service)                                |
| 0x0D           | 0x02 (Read-only)                                         | 0x02   | 0x2803 (UUID for characteristic declaration)                 | 0x2A00 (UUID for device name)                                |
| 0x13           | 0x02 (Read-only)                                         | 0x03   | 0x2A00 (UUID for device name)                                | "PICO_SERVER"                                                |
| 0x18           | 0x02 (Read-only)                                         | 0x07   | 0x2800 (UUID for primary service)                            | 0x0000FF1000001000800000805F9B34FB (custom service UUID)     |
| 0x1B           | 0x02 (Read-only)                                         | 0x08   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic A |
| 0x16           | 0x302 (Read-only, dynamic, long UUID)                    | 0x09   | 0x0000FF1100001000800000805F9B34FB (custom characteristic UUID) | "1"                                                          |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0A   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0B   | 0x2901 (UUID for characteristic user description)            | "Read-only counter"                                          |
| 0x1B           | 0x02 (Read-only)                                         | 0x0C   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic B |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x0D   | 0x0000FF1200001000800000805F9B34FB (custom characteristic UUID) | "400"                                                        |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0E   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0F   | 0x2901 (UUID for characteristic user description)            | "DDS Frequency"                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x10   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic C |
| 0x16           | 0x302 (Read, dynamic, long UUID)                         | 0x11   | 0x0000FF1300001000800000805F9B34FB (custom characteristic UUID) | ""                                                           |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x12   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x13   | 0x2901 (UUID for characteristic user description)            | "String from Pico"                                           |
| 0x1B           | 0x02 (Read-only)                                         | 0x14   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic D |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x15   | 0x0000FF1400001000800000805F9B34FB (custom characteristic UUID) | ""                                                           |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x16   | 0x2902 (UUID for client characteristic configuration)        |                                                              |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x17   | 0x2901 (UUID for characteristic user description)            | "String to Pico"                                             |
| 0x1B           | 0x02 (Read-only)                                         | 0x18   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic E |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x19   | 0x0000FF1500001000800000805F9B34FB (custom characteristic UUID) | "OFF"                                                        |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1A   | 0x2901 (UUID for characteristic user description)            | "LED Status and Control"                                     |
| 0x1B           | 0x02 (Read-only)                                         | 0x1B   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic F |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x1C   | 0x0000FF1600001000800000805F9B34FB (custom characteristic UUID) | "15"                                                         |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1D   | 0x2901 (UUID for characteristic user description)            | "Color Selection"                                            |

### Characteristic configuration discovery

The client continues to search characteristic descriptors for the UUID associated with characteristic configuration. It moves through the characteristics in order, issuing a read request to that descriptor each time it finds one. The server responds to these reqests with two packets. The first is of type GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT and contains the value of the descriptor (0 for notifications disabled, 1 for notifications enabled). The second packet is of type GATT_EVENT_QUERY_COMPLETE and indicates that the server has finished with the latest request.

After getting through all characteristics, the client has fully populated its local version of the server database. It signals a Protothread via a semaphore that all characteristics have been discovered, which starts the user interface.

```
// Obtain the notification status of each characteristic
        case TC_W4_CHARACTERISTIC_NOTIFICATIONS:
            switch(type_of_packet) {
                // Received the result of a notification query
                case GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT:
                    DEBUG_LOG("Storing characteristic notification status\n") ;
                    // The characteristic value is a character array. How long is it?
                    descriptor_length = gatt_event_characteristic_value_query_result_get_value_length(packet);
                    // What is the address to the start of this array?
                    const uint8_t *descriptor = gatt_event_characteristic_value_query_result_get_value(packet);
                    // Store the characteristic configuration
                    server_characteristic_configurations[k] = little_endian_read_16(descriptor, 0) ;
                    // Set the notifications as enabled or disabled
                    if ((char)server_characteristic_configurations[k]) {
                        notifications_enabled[k] = 1 ;
                    }
                    else {
                        notifications_enabled[k] = 0 ;
                    }
                    break ;
                // Finished with a query. Do we need to do another?
                case GATT_EVENT_QUERY_COMPLETE:
                    // Increment k one time
                    k++ ;
                    // Find the next characteristic with a descriptor.
                    while((k<num_characteristics) && (server_characteristic_descriptor[k][0].uuid16!=CHARACTERISTIC_CONFIGURATION)) {
                        k++ ;
                    }
                    // Have we gone thru all characteristics? If not, read the next configuration
                    if (k<num_characteristics) {
                        gatt_client_read_characteristic_descriptor(handle_gatt_client_event, connection_handle, &server_characteristic_descriptor[k][0]) ;
                    }
                    // If so, move to next state
                    else {
                        // Signal thread that all characteristics have been acquired
                        PT_SEM_SAFE_SIGNAL(pt, &characteristics_discovered) ;

                        // Transition to an idle state
                        state = TC_W4_READY ;

                        // Turn on notifications
                        // Register handler for notifications
                        listener_registered = true;
                        gatt_client_listen_for_characteristic_value_updates(&notification_listener, handle_gatt_client_event, connection_handle, NULL);
                    }
                    break ;


                default:
                    break ;

            }
            break ;
```

The client has complete knowledge of the server's attribute database upon leaving this state:

| Length (bytes) | Access permissions                                       | Handle | Type                                                         | Value                                                        |
| :------------- | :------------------------------------------------------- | :----- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| 0x0A           | 0x02 (Read-only)                                         | 0x01   | 0x2800 (UUID for primary service)                            | 0x1800 (UUID for GAP service)                                |
| 0x0D           | 0x02 (Read-only)                                         | 0x02   | 0x2803 (UUID for characteristic declaration)                 | 0x2A00 (UUID for device name)                                |
| 0x13           | 0x02 (Read-only)                                         | 0x03   | 0x2A00 (UUID for device name)                                | "PICO_SERVER"                                                |
| 0x18           | 0x02 (Read-only)                                         | 0x07   | 0x2800 (UUID for primary service)                            | 0x0000FF1000001000800000805F9B34FB (custom service UUID)     |
| 0x1B           | 0x02 (Read-only)                                         | 0x08   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic A |
| 0x16           | 0x302 (Read-only, dynamic, long UUID)                    | 0x09   | 0x0000FF1100001000800000805F9B34FB (custom characteristic UUID) | "1"                                                          |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0A   | 0x2902 (UUID for client characteristic configuration)        | 0                                                            |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0B   | 0x2901 (UUID for characteristic user description)            | "Read-only counter"                                          |
| 0x1B           | 0x02 (Read-only)                                         | 0x0C   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic B |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x0D   | 0x0000FF1200001000800000805F9B34FB (custom characteristic UUID) | "400"                                                        |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0E   | 0x2902 (UUID for client characteristic configuration)        | 0                                                            |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0F   | 0x2901 (UUID for characteristic user description)            | "DDS Frequency"                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x10   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic C |
| 0x16           | 0x302 (Read, dynamic, long UUID)                         | 0x11   | 0x0000FF1300001000800000805F9B34FB (custom characteristic UUID) | ""                                                           |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x12   | 0x2902 (UUID for client characteristic configuration)        | 0                                                            |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x13   | 0x2901 (UUID for characteristic user description)            | "String from Pico"                                           |
| 0x1B           | 0x02 (Read-only)                                         | 0x14   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic D |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x15   | 0x0000FF1400001000800000805F9B34FB (custom characteristic UUID) | ""                                                           |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x16   | 0x2902 (UUID for client characteristic configuration)        | 0                                                            |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x17   | 0x2901 (UUID for characteristic user description)            | "String to Pico"                                             |
| 0x1B           | 0x02 (Read-only)                                         | 0x18   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic E |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x19   | 0x0000FF1500001000800000805F9B34FB (custom characteristic UUID) | "OFF"                                                        |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1A   | 0x2901 (UUID for characteristic user description)            | "LED Status and Control"                                     |
| 0x1B           | 0x02 (Read-only)                                         | 0x1B   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic F |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x1C   | 0x0000FF1600001000800000805F9B34FB (custom characteristic UUID) | "15"                                                         |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1D   | 0x2901 (UUID for characteristic user description)            | "Color Selection"                                            |

### Receiving characteristic notifications

In the event that notifications are enabled for a particular characteristic, the server will autonomously send updated characteristic values to the client. Our client must receive those notifications, parse them for the characteristic handle for which they apply, and update the local database accordingly. The code snippet below accomplishes this, and lives *outside* the rest of the state machine. That is to say, the code below runs *every time* the client receives a packet of type GATT_EVENT_NOTIFICATION, regardless of the value of the state.

```
// Did we just receive a notification? If so, update the associated characteristic value
    // and signal the protothread which refreshes values in the terminal UI.
    if (type_of_packet == GATT_EVENT_NOTIFICATION) {
        // How many bytes in the payload?
        uint32_t value_length = gatt_event_notification_get_value_length(packet);
        // What is the value handle for the notification we've just received
        uint16_t notification_handle = gatt_event_notification_get_value_handle(packet) ;
        // What is the address of the start of the data for this packet (a byte array)
        const uint8_t *value = gatt_event_notification_get_value(packet);
        // Initialize our characteristic identifier to -1 (invalid)
        int which_characteristic = -1 ;
        // Loop thru each characteristic, compare that characteristic's value handle to the value
        // handle of the notification we've received. A match indicates that this is a notification
        // for the associated characteristic.
        for (int i=0; i<num_characteristics; i++) {
            if (notification_handle==server_characteristic[i].value_handle) {
                which_characteristic = i ;
            }
        }
        // Did we find a value handle match?
        if (which_characteristic>=0) {
            // If so, copy the received data to the value buffer of the associated characteristic
            memcpy(server_characteristic_values[which_characteristic], value, value_length) ;
            // Null-terminate the character array
            server_characteristic_values[which_characteristic][value_length] = 0 ;
            // Make sure notifications are labeled as on for this characteristic
            // notifications_enabled[which_characteristic] = 1 ;
            // Semaphore-signal the protothread which will refresh values in the UI
            PT_SEM_SAFE_SIGNAL(pt, &characteristics_discovered) ;
        }
        // Exit the callback
        return;
    }
```

As an example, suppose that characteristics have been enabled for characteristic A (the "Read-only counter," and suppose that the value of this characteristic has been updated from 1 to 2. On the client, our local database will look like the following *after* having received and parsed the notification from the server.

| Length (bytes) | Access permissions                                       | Handle | Type                                                         | Value                                                        |
| :------------- | :------------------------------------------------------- | :----- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| 0x0A           | 0x02 (Read-only)                                         | 0x01   | 0x2800 (UUID for primary service)                            | 0x1800 (UUID for GAP service)                                |
| 0x0D           | 0x02 (Read-only)                                         | 0x02   | 0x2803 (UUID for characteristic declaration)                 | 0x2A00 (UUID for device name)                                |
| 0x13           | 0x02 (Read-only)                                         | 0x03   | 0x2A00 (UUID for device name)                                | "PICO_SERVER"                                                |
| 0x18           | 0x02 (Read-only)                                         | 0x07   | 0x2800 (UUID for primary service)                            | 0x0000FF1000001000800000805F9B34FB (custom service UUID)     |
| 0x1B           | 0x02 (Read-only)                                         | 0x08   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic A |
| 0x16           | 0x302 (Read-only, dynamic, long UUID)                    | 0x09   | 0x0000FF1100001000800000805F9B34FB (custom characteristic UUID) | "2"                                                          |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0A   | 0x2902 (UUID for client characteristic configuration)        | 1                                                            |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0B   | 0x2901 (UUID for characteristic user description)            | "Read-only counter"                                          |
| 0x1B           | 0x02 (Read-only)                                         | 0x0C   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic B |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x0D   | 0x0000FF1200001000800000805F9B34FB (custom characteristic UUID) | "400"                                                        |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x0E   | 0x2902 (UUID for client characteristic configuration)        | 0                                                            |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x0F   | 0x2901 (UUID for characteristic user description)            | "DDS Frequency"                                              |
| 0x1B           | 0x02 (Read-only)                                         | 0x10   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic C |
| 0x16           | 0x302 (Read, dynamic, long UUID)                         | 0x11   | 0x0000FF1300001000800000805F9B34FB (custom characteristic UUID) | ""                                                           |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x12   | 0x2902 (UUID for client characteristic configuration)        | 0                                                            |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x13   | 0x2901 (UUID for characteristic user description)            | "String from Pico"                                           |
| 0x1B           | 0x02 (Read-only)                                         | 0x14   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic D |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x15   | 0x0000FF1400001000800000805F9B34FB (custom characteristic UUID) | ""                                                           |
| 0x0A           | 0x10e (Read, write without response, write, dynamic)     | 0x16   | 0x2902 (UUID for client characteristic configuration)        | 0                                                            |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x17   | 0x2901 (UUID for characteristic user description)            | "String to Pico"                                             |
| 0x1B           | 0x02 (Read-only)                                         | 0x18   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic E |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x19   | 0x0000FF1500001000800000805F9B34FB (custom characteristic UUID) | "OFF"                                                        |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1A   | 0x2901 (UUID for characteristic user description)            | "LED Status and Control"                                     |
| 0x1B           | 0x02 (Read-only)                                         | 0x1B   | 0x2803 (UUID for characteristic declaration)                 | Access permissions, handle, and 128-bit UUID of custom characteristic F |
| 0x16           | 0x306 (Read, write without response, dynamic, long UUID) | 0x1C   | 0x0000FF1600001000800000805F9B34FB (custom characteristic UUID) | "15"                                                         |
| 0x08           | 0x10a (Read, write, dynamic)                             | 0x1D   | 0x2901 (UUID for characteristic user description)            | "Color Selection"                                            |

## Client application

The [GATT client demonstration](https://github.com/vha3/Hunter-Adams-RP2040-Demos/tree/master/Bluetooth/GATT_Client) includes a command-line user application that allows for the user to visualize the contents of the database, enable/disable notifications for characteristics, issue read requests for particular characteristics, and issue write requests for particular characteristics. This application is implemented by means of two Protothreads. One of the protothreads writes all characteristic values to the screen whenever it is semaphore-signalled to do so, and the other implements a command-line user interface.

### Printing database contents

In the final state of the server discovery state machine discussed above, a semaphore is used to trigger the thread below to run once. This thread uses ASCII escape sequences to clear the terminal screen and write all the characteristic handles, user descriptions, and values. After having done so, it waits until it is semaphore-signalled again to repeat the process. This will only occur if the user interface thread resets the server discovery state machine, or if the client receives a notification from the server.

```
// Protothread that prints all characteristic values
static PT_THREAD (protothread_client(struct pt *pt))
{
    PT_BEGIN(pt) ;

    // For incrementing through characteristics
    static int counter = 0 ;

    while(1) {

        // Wait to be signalled by the GATT state machine
        PT_SEM_SAFE_WAIT(pt, &characteristics_discovered) ;

        // Save cursor position
        printf("\033[s") ;
        // Move cursor to 10th row
        printf("\033[10;0H") ;

        // Make cursor invisible
        printf("\033[?25l") ;

        // Print all characteristic values
        printf("Discovered characteristics:\n\n") ;
        for (counter = 0; counter < num_characteristics; counter++) {
            printf("Characteristic ID.:\033[K\t %c\n", (counter+97)) ;
            printf("User description:\033[K\t %s\n", server_characteristic_user_description[counter]) ;
            // printf("Config:\033[K\t\t\t %04x\n", notifications_enabled[counter]) ;
            printf("Access permissions:\033[K\t ") ;
            // Only print notification status if characteristic allows for notifications
            parsePermissions(server_characteristic[counter].properties) ;
            if ((notifications_enabled[counter]>=0) && (notifications_enabled[counter]<2)) {
                if (notifications_enabled[counter]) printf("\033[1mNotify:\033[K\t\t\t ON\033[22m\n") ;
                else printf("Notify:\033[K\t\t\t OFF\n") ;
            }
            printf("Value:\033[K\t\t\t %s\n\n", server_characteristic_values[counter]) ;
        }
        // Erase down
        printf("\033[J") ;

        // Restore cursor position
        printf("\033[s") ;
    }

    PT_END(pt) ;
}
```

### Command-line user interface

The second protothread implements a command-line user interface that allows for the user to interact with the GATT client and server. In particular, the user may request to read the values of all characteristics on the server (which will then be updated in the terminal window). The user may also enable/disable notifications for characteristics which allow notifications (if they attempt to do so on a characteristic that does *not* allow notifications, they receive an error message). And the user may use the serial terminal to update the values of characteristics with write permisssions.

In order to enable/disable notifications, the thread resets the server discovery state machine state to TC_W4_ENABLE_NOTIFICATIONS_COMPLETE, and then uses the BTstack function `gatt_client_write_client_characteristic_configuration` to turn notifications on or off for a particular characteristic. In order to write to a characteristic, the thread uses `gatt_client_write_value_of_characteristic_without_response`. It then resets the state to TC_W4_CHARACTERISTIC_VALUE_PRINT and re-reads all characteristic values.

```
// Protothread which implements a user interface
static PT_THREAD (protothread_ui(struct pt *pt))
{
    PT_BEGIN(pt) ;

    // For holding user input
    static char temp_var ;
    static char temp_var2 ;

    while(1) {
        // Move cursor to line above data
        printf("\033[10;0H") ;
        // Erase to top
        printf("\033[1J") ;
        // Move cursor to top
        printf("\033[H") ;

        // Print user instructions
        sprintf(pt_serial_out_buffer, "Type 0 to refresh characteristic values.\n\r") ;
        serial_write ;
        sprintf(pt_serial_out_buffer, "Type the ID of a characteristic to write.\n\r") ;
        serial_write ;
        sprintf(pt_serial_out_buffer, "Type the ID of a characteristic (UPPERCASE) to toggle notifications on/off.\n\r") ;
        serial_write ;

        // Wait for user input
        serial_read ;

        // Parse user input
        temp_var = pt_serial_in_buffer[0]-97 ;
        temp_var2 = pt_serial_in_buffer[0]-65 ;

        // Did the user specify a valid (lowercase) characteristic ID?
        if ((temp_var>=0) && (temp_var<num_characteristics)) {

            // Does this characteristic include write permissions?
            if (server_characteristic[temp_var].properties & (1u<<2)) {
                // Move cursor to row 4
                printf("\033[4;0H") ;

                // Prompt for user input
                sprintf(pt_serial_out_buffer, "Please type a value for characteristic %c.\n\r\033[K", temp_var+97) ;
                serial_write ;
                serial_read ;

                // Send user input to server
                int status = gatt_client_write_value_of_characteristic_without_response(connection_handle, server_characteristic[temp_var].value_handle, strlen(pt_serial_in_buffer), pt_serial_in_buffer);
            }

            // If characteristic does not include write permissions, tell the user.
            else {
                printf("\033[4;0H") ;
                sprintf(pt_serial_out_buffer, "\033[1mNo write permission for that characteristic.\n\r\033[22m") ;
                serial_write ;
                PT_YIELD_usec(500000) ;
            }
        }

        // Did the user alternatively specify a valid (UPPERCASE) characteristic ID?
        else if ((temp_var2>=0) && (temp_var2<num_characteristics)) {

            // Does this characteristic include notifications?
            if (server_characteristic[temp_var2].properties & (1u<<4)) {

                // Wait until our state machine is ready for a new instruction
                PT_YIELD_UNTIL(pt, state==TC_W4_READY) ;

                // If notifications are presently enabled, disable them
                if (notifications_enabled[temp_var2]) {
                    state = TC_W4_ENABLE_NOTIFICATIONS_COMPLETE;
                    gatt_client_write_client_characteristic_configuration(handle_gatt_client_event, connection_handle,
                        &server_characteristic[temp_var2], GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NONE);
                }

                // If notifications are presently disabled, enable them
                else {
                    state = TC_W4_ENABLE_NOTIFICATIONS_COMPLETE;
                    gatt_client_write_client_characteristic_configuration(handle_gatt_client_event, connection_handle,
                        &server_characteristic[temp_var2], GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION);
                }
            }

            // If not, tell the user
            else {
                printf("\033[4;0H") ;
                sprintf(pt_serial_out_buffer, "\033[1mNo notification permission for that characteristic.\n\r\033[22m") ;
                serial_write ;
                PT_YIELD_usec(500000) ;
            }
        }

        // Wait until the state machine is ready to receive a new instruction
        PT_YIELD_UNTIL(pt, state==TC_W4_READY) ;

        // Reset characteristic counters
        k = 0 ;
        k2 = 0 ;

        // Refresh all characteristic values
        state = TC_W4_CHARACTERISTIC_VALUE_PRINT ;
        gatt_client_read_value_of_characteristic(handle_gatt_client_event, connection_handle, &server_characteristic[k]) ;
    }

    PT_END(pt) ;
}
```

## Starting the client

To start our GATT client, we initialize stdio, initialize our semaphore, and start the CYW43. We then initialize the L2CAP protocol, the security manager, and initialize an att server on our client (we could actually eliminate this for communicating with another Pico-based peripheral). We initialize the GATT client, which gives us access to the BTstack GATT client API, and then register our HCI packet handler. Finally, we turn on the Bluetooth device and schedule our application threads.

```
int main() {

    // Initialize stdio
    stdio_init_all();

    // Make cursor invisible
    printf("\033[?25l") ;

    // Initialize the semaphore
    PT_SEM_SAFE_INIT(&characteristics_discovered, 0) ;

    // initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    // Initialize L2CAP and Security Manager
    l2cap_init();
    sm_init();
    sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);

    // setup empty ATT server - only needed if LE Peripheral does ATT queries on its own, e.g. Android and iOS
    att_server_init(NULL, NULL, NULL);

    // Initialize GATT client
    gatt_client_init();

    // Register the HCI event callback function
    hci_event_callback_registration.callback = &hci_event_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // Turn on!
    hci_power_control(HCI_POWER_ON);

    // Add and schedule threads
    pt_add_thread(protothread_client) ;
    pt_add_thread(protothread_ui) ;
    pt_sched_method = SCHED_ROUND_ROBIN ;
    pt_schedule_start ;

}
```

## Visual representation of the state machine

The flowchart below summarizes the content above.

![missing](https://vanhunteradams.com/Pico/BLE/state_machine.png)GATT client state machine.