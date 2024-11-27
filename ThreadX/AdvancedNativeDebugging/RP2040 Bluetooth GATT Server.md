# Building a Bluetooth GATT Server on the Pi Pico W

### [V. Hunter Adams](https://vanhunteradams.com/)

## Purpose and intended audience for this webpage[¶](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Purpose-and-intended-audience-for-this-webpage)

This webpage documents [this GATT server example](https://github.com/vha3/Hunter-Adams-RP2040-Demos/tree/master/Bluetooth/GATT_Server), demonstrated in the video below. These materials have been assembled for students in [ECE 4760](https://ece4760.github.io/) at Cornell, though I hope that they may be useful for hobbyists that wish to build a Bluetooth interface to their Pi Pico W projects. This webpage focuses primarily on the *implementation* of a GATT server, and only discusses the Bluetooth architecture to the extent that it adds clarity to description of the implementation. The same philosophy is applied to [BTstack](https://bluekitchen-gmbh.com/), the Bluetooth stack implementation which is used on the Pi Pico W. This webpage focuses more on how BTstack is *used*, rather than how it *works*.

This webpage is prerequisite reading for the [GATT client webpage](https://vanhunteradams.com/Pico/BLE/GATT_Client.html).

<iframe width="560" height="315" src="https://www.youtube.com/embed/RutIToHKHXA?si=dnYx0JFkoPd7a0hK" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen="" style="box-sizing: border-box;"></iframe>

## Page contents

> - A brief overview of the Bluetooth stack
>
>   > - [Background information](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Background-information)
>   > - [Hardware utilization on RP2040](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Hardware-utilization-on-RP2040)
>
> - [BTstack and the Bluetooth stack](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#BTstack-and-the-Bluetooth-stack)
>
> - The GATT database
>
>   > - [Attributes](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Attributes)
>   > - [The Attribute Database](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#The-Attribute-Database)
>   > - [Building an attribute database with BTstack](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Building-an-attribute-database-with-BTstack)
>   > - [Adding a custom primary service](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Adding-a-custom-primary-service)
>   > - [Adding characteristics to our custom service](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Adding-characteristics-to-our-custom-service)
>   > - [Compiling our ATT database](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Compiling-our-ATT-database)
>
> - Implementing the GATT server
>
>   > - [A database struct](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#A-database-struct)
>   > - [Read callback](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Read-callback)
>   > - [Write callback](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Write-callback)
>
> - Building a small API for interacting with our server
>
>   > - [Initialization function](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Initialization-function)
>   > - [Functions for updating characteristics](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Functions-for-updating-characteristics)
>
> - [Instantiating the server](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Instantiating-the-server)
>
> - Advertising and connecting with Generic Access Profile (GAP)
>
>   )
>
>   > - [Some background on GAP](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Some-background-on-GAP)
>   > - [Building a GAP advertising packet](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Building-a-GAP-advertising-packet)
>   > - [A packet handler for setting up and starting GAP advertisements](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#A-packet-handler-for-setting-up-and-starting-GAP-advertisements)
>
> - Configuring BTstack
>
>   > - [BTstack features](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#BTstack-features)
>   > - [Memory configuration directives](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Memory-configuration-directives)
>   > - [HCI controller to host configuration](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#HCI-controller-to-host-configuration)
>   > - [Nonvolatile memory directives](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Nonvolatile-memory-directives)
>   > - [Max size of ATT database](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Max-size-of-ATT-database)
>   > - [System configs](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#System-configs)
>
> - [An application that uses this server](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#An-application-that-uses-this-server)
>
> - [Going farther](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Going-farther)
>
> - [Resources](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Resources)

## A brief overview of the Bluetooth stack

### Background information

The Bluetooth Low Energy protocol stack is organized into layers. At the bottom of these layers is an antenna that communicates with analog communications circuits to modulate/demodulate 2.4GHz radio signals into digital symbols. Our application code lives at the top of this stack. **For context and completeness, I'll briefly describe each each protocol in the stack. Please note, however, that most people developing a Bluetooth application will interface only with the \*top\* layers (GAP and GATT) through an API that makes automatic use of the lower layers. Discussion of the lower layers, for our purposes, is only to introduce some vocabulary.**

As illustrated in Fig. 1, these layers can be separated into three sections: Host, Controller, and Application. In the case of BTstack on the Pi Pico W, I've added a fourth layer between the application and controller called "Packet Handlers." As we'll see, packet handlers are the mechanism by which our application interacts with the Bluetooth stack at runtime. Let us start at the bottom layer and work our way to the top.

The physical layer of the Bluetooth stack contains the analog radio communication and modulation/demodulation circuits. It sends and receives data over the air using 2.4GHz GFSK-modulated radio transmissions. These analog transmissions are digitized by receiving devices and communicated to the next layer in the stack: the link layer.

The link layer is responsible for maintaining the state of the radio, structuring packets, computing checksums and computing encryption sequences. The link layer offers two types of channels to the radio: advertising and data. Devices that haven't connected to another device use the advertising channels to broadcast data, advertise themselves, and scan for other devices with which they may connect. The data channels are used by devices that *have* connected to exchange information. The link layer communicates data to both types of channels in packets with a defined structure. These packets include information about the address of the intended recipient, some metadata about the contents of the packet, the payload, and a checksum. This layer may include both custom software and custom hardware. It's a complicated piece of the Bluetooth stack which is often abstracted away from the developer, and with which we interact via the next layer: the host-controller interface.

The Host-Controller Interface provides a standard interface through which a Host can interact with a Controller. This is useful because, as is the case with the Pi Pico W, it allows for the Host and Controller layers of the Bluetooth stack to live on *different* devices. This is a convenient place to split the stack via a standardized interface because all the layers *below* the HCI have hard real-time requirements while the layers *above* the HCI do not. It's often practical to separate the real-time constrained controller from the host, which can implement the protocol stack on a more advanced processor.

The L2CAP layer talks to the HCI layer and has a couple of responsibilities. The first is fragmentation and recombination. The link layer packets have a strict size limit (generally 20 bytes of data payload per packet), but the upper layers may want to communicate (or receive) packets which are longer than this. The L2CAP layer receives packets from the upper layers and fragments them into manageable pieces for the lower layers. It also recombines the pieces of larger packets which it receives from lower layers before emitting the assembled message to the upper layers. In addition to fragmenting and recombining packets, the L2CAP layer performs multiplexing. It takes multiple protocols from the upper layers (mostly ATT and SMP) and puts them into the standard BLE packet format.

Packets from the L2CAP layer will be emitted to either the Attribute Protocol (ATT) and Security Manager Protocol (SMP) layers. The SMP offers support for pairing, bonding, and encryption re-establishment between devices. The ATT layer implements a client/server protocol based on data organized into *attributes* (much more on that later). Each of these attributes includes the data of interest, and also some metadata about that data. This metadata includes things like a Universal Unique Identifier (UUID), access permissions, and a 16-bit handle that acts like the "name" or "address" of that attribute. ATT offers a large set of operations (error handling, read operations, write operations, server intiation, etc.) which are abstracted to an API by BTstack on the Pi Pico W. The *Generic Attribute Profile* builds a hierarchical data abstraction on top of ATT based on services and characteristics. More on those later.

Finally, the Generic Access Profile controls device discovery and connection, security establishment, and general device interaction *outside* the protocol stack.

To write a GATT server for the Pi Pico W, we will *mostly* interact with the GAP and GATT layers of this stack. Though some configuration of the lower levels will also be required.

![missing](https://vanhunteradams.com/Pico/BLE/stack.png)Fig 1: Layers in the Bluetooth stack

### Hardware utilization on RP2040

The Raspberry Pi Pico W uses a CWY43439 Bluetooth and WiFi module, with which it communicates via two PIO state machines and two DMA channels. The C SDK uses the `claim` mechanism to obtain both the PIO state machines and the DMA channels. The SDK will attempt first to put the state machines on PIO block 1 and, if that isn't available, it'll put them on block 0. The CWY43439 implements the physical layer, link layer, and controller-side of the host controller interface. All other layers of the Bluetooth stack are implemented on the RP2040 by way of an RP2040 port of the third-party library [BTstack](https://bluekitchen-gmbh.com/). Please note that I did not contribute to BTstack (thank you, [Matthias Ringwald](https://github.com/mringwal)!), nor did I contribute to porting it to the RP2040 (thank you [Graham Sanderson](https://github.com/kilograham) and [Peter Harper](https://github.com/peterharperuk)!). If anyone more informed than me reads this and notices an error or misunderstanding, please let me know (vha3@cornell.edu) and I'll correct it.

## BTstack and the Bluetooth stack

BTstack provides an infrastructure for interacting with all of the Host-layers of the Bluetooth stack from our application code, down to the Host-Controller Interface. Though BTstack includes an API for issuing commands to these lower layers, your application code will *probably* not need to do so. Instead, we'll make primary use of BTstack's API for higher-level profiles like GAP and GATT, which themselves make automatic use of those lower layers. That said, using BTstack does require some compile-time *configuration* of these lower layers. More on that in a bit.

To our application code, BTstack looks like an asynchronous event generator. We can send it commands (all of which are non-blocking) which may, in turn, issue commands to the Bluetooth module. BTstack responds to those commands via asynchronous events, each of which emits a packet (a `char` array). The BTstack API includes a function that allows for us to determine the *type* of an emitted packet ("what does the data in this packet represent?") and a series of functions for parsing the packet based on its type. The code that we write manages these asynchronous events via packet handlers. Fig. 2, which comes from the [BTstack documentation](https://bluekitchen-gmbh.com/btstack/#architecture/), illustrates this general architecture.



![missing](https://vanhunteradams.com/Pico/BLE/BTSTACK.png)Fig 2: BTstack architecture (figure from [1])

In the case of a GATT client, the commands that we send to BTstack include things like "discover all characteristics in a particular service on a GATT server," "read a characteristic value," or "write this characteristic descriptor." Each of these commands is translated to some number of wireless transmissions to the GATT server. This server responds to these commands/queries and BTstack assembles those responses into packets which it asynchronously emits back to our client application. Our packet-handler implements a state machine which responds to the types of these emitted packet and to the data contained in that packet, thereby gaining an understanding of the organization and contents of the GATT server.

The GATT server, on the other hand, merely responds to the requests that it receives from the GATT client (and possibly also sends some server-initiated updates to the client in the form of characteristic notifications). The GATT server organizes all data into a hierarchical database, the organization of which will be discussed in detail later on. Based on the brief description of GATT client/server roles above, you may notice that the GATT server is "dumb" compared to the GATT client. While the client must implement a relatively complicated series of queries to learn about the services and characteristics contained within a server, the server just does whatever its told to do by the client. This asymmetry is by design! At each layer of the Bluetooth stack, one of the devices is doing more "work" than the other device. GAP scanners do more work than advertisers, ATT clients do more work than servers, etc. This asymmetry allows for Bluetooth-equipped devices with more resources (e.g. phones, laptops, etc.) to offload some of the power/computationally-costly work from Bluetooth-equipped devices with less resources (sensors, small devices, etc.).

This asymmetry also leads to a natural order in which to introduce the concepts associated with building a GATT server and client. We'll start with the server-side device and *then* discuss how to build a client device that is capable of communicating with it. This allows for us to become familiar with some concepts on the relatively-simple server side before deploying them on the relatively-complicated client side. *Also*, for many microcontrollers-oriented readers, it's probably the case that they *just* need to implement a server and needn't worry about the client, which will be running on a phone or computer.

I'm going to introduce these topics in the order that (for me, at least) is easiest to understand, which doesn't necessarily map directly to the order in which events occur when our server and client connect and communicate. I may also discuss some pieces in isolation before later discussing how they integrate. Feedback on ways to improve clarity is welcome.

## The GATT database

A GATT client doesn't know *anything* about the information stored on a server to which it connects. It must learn about the server's contents by asking it questions and listening to its answers. In order for this process to occur, the client and the server must "speak the same language." They must agree on how the information on the server is *accessed.* This agreement allows for the client to ask questions which are comprehensible to the server.

### Attributes

Perhaps it stands to reason that, in order for the client to be able to pose meaningful questions to the server, the server must also store some metainformation *about* the data which it stores. Remember that a client doesn't know anything about the information on a server when it connects. Suppose a GATT server which stores information from a temperature sensor. If that server *only* stored the temperature measurement, the client would have no way of learning that the data represents temperature, that the data is read-only, or any other potentially-useful information (like units or sensor location). For that reason, the smallest unit of data which appears in a GATT server is an **attribute**.

Each attribute is composed of a *handle* (2 bytes), a *type* (2 or 16 bytes), and a *value* (0 to 512 bytes). Each handle represents the address of that particular attribute. We use an attribute's handle to access or refer to it. It's appropriate to think of this handle like a memory address (though it isn't the actual memory address of the attribute). The *type* of an attribute holds a Universally Unique Identifier (UUID) for that attribute. There is a [whole library of UUID values](https://www.bluetooth.com/wp-content/uploads/Files/Specification/Assigned_Numbers.html) which are understood by Bluetooth client devices. This means, for instance, that we could assign our temperature sensor value the UUID associated with temperature and a connecting client would know, based on that UUID, that the data represents temperature. Though these UUID's are 128-bits long, there are 16-bit abbreviated UUID's for the "standard" attribute types. And finally, the attribute *value* holds the data itself. Note that this data needn't represent a measurement, it may instead be organizational information about the database, metadata about another attribute, etc. Each attribute also has associated *permissions*, which let the client know whether that attribute's value is readable, writeable, and whether authentication/authorization permissions are required.

### The Attribute Database

The GATT server organizes all of these attributes into an attribute database, which is well-represented as a table. This database is, fundamentally, a flat structure. Each attribute in the database has its own handle, type, and value. However, the Attribute Profile hierarchichally organizes these attributes into logical groups. The GATT server organizes attributes into *services*, which themselves contain zero or more *characteristics*, which themselves contain zero or more *characteristic descriptors*. We can think of our table, which represents the attribute database, being organized into hierarchichal "chunks."

The advantage of doing this is that it allows for us to generate standard groups of attributes which can be re-used across devices. A service is very analagous to a class in object-oriented programming. Each service contains zero or more characteristics, which are themselves containers for user data. For instance, and Environmental Sensing service may contain characteristics for temperature and humidity. Each of these characteristics contains at least two attributes: a declaration and a value. They may contain *additional* attributes in the form of characteristic descriptors, which provide additional context or metadata about that characteristic.

All of this becomes a bit clearer with a concrete example. Let us construct the following attribute database:

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

### Building an attribute database with BTstack

BTstack allows for us to build a text-description of the attribute database that we'd like to generate, and then a [python compiler](https://github.com/bluekitchen/btstack/blob/master/tool/compile_gatt.py) uses this file to generate a database and some macros for easy access to attribute handles, values, etc. The [custom GATT server demo](https://github.com/vha3/Hunter-Adams-RP2040-Demos/tree/master/Bluetooth/GATT_Server) uses the GATT description linked [here](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Bluetooth/GATT_Server/GATT_Service/server_demo_gattfile.gatt), and copied below.

```
PRIMARY_SERVICE, GAP_SERVICE
CHARACTERISTIC, GAP_DEVICE_NAME, READ, "PICO_SERVER"

PRIMARY_SERVICE, GATT_SERVICE
CHARACTERISTIC, GATT_DATABASE_HASH, READ,

// First custom service
PRIMARY_SERVICE, 0000FF10-0000-1000-8000-00805F9B34FB

// Characteristic A - read only, dynamic, with notifications
CHARACTERISTIC, 0000FF11-0000-1000-8000-00805F9B34FB, DYNAMIC | READ | NOTIFY,
CHARACTERISTIC_USER_DESCRIPTION, READ,

// Characteristic B - write without response, notify
CHARACTERISTIC, 0000FF12-0000-1000-8000-00805F9B34FB, WRITE_WITHOUT_RESPONSE | READ | NOTIFY | DYNAMIC,
CHARACTERISTIC_USER_DESCRIPTION, READ,

// Characteristic C - read only, dynamic, with notifications
CHARACTERISTIC, 0000FF13-0000-1000-8000-00805F9B34FB, DYNAMIC | READ | NOTIFY,
CHARACTERISTIC_USER_DESCRIPTION, READ,

// Characteristic D - write without response, notify
CHARACTERISTIC, 0000FF14-0000-1000-8000-00805F9B34FB, WRITE_WITHOUT_RESPONSE | READ | NOTIFY | DYNAMIC,
CHARACTERISTIC_USER_DESCRIPTION, READ,

// Characteristic E - read only, dynamic, with notifications
CHARACTERISTIC, 0000FF15-0000-1000-8000-00805F9B34FB, WRITE_WITHOUT_RESPONSE | READ | DYNAMIC,
CHARACTERISTIC_USER_DESCRIPTION, READ,

// Characteristic F - write without response, notify
CHARACTERISTIC, 0000FF16-0000-1000-8000-00805F9B34FB, WRITE_WITHOUT_RESPONSE | READ | DYNAMIC,
CHARACTERISTIC_USER_DESCRIPTION, READ,
```

Let us go through a few lines of this to see how it works. The first line of this file declares a primary service called GAP_SERVICE. This is a **mandatory** GATT service that every GATT server must include. This line specifies that the attribute database should start with an attribute of type Primary Service and with a value equal to the UUID associated the the GAP service. The UUID for the GAP service is 0x1800, and is populated by the [python compiler](https://github.com/bluekitchen/btstack/blob/2b49e57bd1fae85ac32ac1f41cdb7c794de335f6/tool/compile_gatt.py#L68). The compiler automatically makes this attribute read-only, and automatically computes its length. A subset of the compiled database (a character array) is shown below. Please note that I've omitted the rest of the database for now, for clarity.

```
const uint8_t profile_data[] =
{
    // ATT DB Version
    1,

    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18, 

    .
    .
    .

}; // total size 234 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_GAP_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_GAP_SERVICE_END_HANDLE 0x0003
.
.
.
```

The interpretation of this compiled line is that it is of length 0x000a (10 bytes), access permissions are 0x0002 (read only, by anyone), the attribute handle is 0x0001 (first line of database), the attribute UUID is 0x2800 (Primary Service) and the attribute value is 0x1800 (UUID of GAP service). Furthermore, the compiler has generated two macros for our convenience. The first is one for the value of the service start handle, and the second is for the value of the service end handle.

This service declaration marks the beginning of the GAP service. All of the attributes which follow this service declaration and occur before the *next* service declaration belong to this service. As you can see in the text-based description of the database above, this service will include a single characteristic. Let's take a closer look at line 2, copied below:

```
CHARACTERISTIC, GAP_DEVICE_NAME, READ, "PICO_SERVER"
```

This line creates a characteristic, which you'll recall includes at least two attributes. These attributes include a *characteristic declaration* and the *characteristic value*. Some characteristics may also include some additional characteristic descriptor attributes, but this one does not. Let's see how this line gets compiled into the attribute database . . .

```
const uint8_t profile_data[] =
{
    // ATT DB Version
    1,

    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18, 
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME - READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a, 
    // 0x0003 VALUE CHARACTERISTIC-GAP_DEVICE_NAME - READ -'PICO_SERVER'
    // READ_ANYBODY
    0x13, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x50, 0x49, 0x43, 0x4f, 0x5f, 0x53, 0x45, 0x52, 0x56, 0x45, 0x52, 
    .
    .
    .
}; // total size 234 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_GAP_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_GAP_SERVICE_END_HANDLE 0x0003
.
.
.

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0003
.
.
.
```

We see two lines, which correspond to the two attributes which compose this characteristic. The first line is the characteristic declaration, and is interpreted as follows: it is of length 0x000d (13 bytes), access permissions are read-only by anyone (0x0002), attribute handle is 0x0002 (second line of database), attribute type is 0x2803 (standard 16-bit UUID for characteristic declaration), and the attribute value includes the UUID for the characteristic (0x2a00 - standard for device name), the handle for the characteristic (0x0003), and the access permissions for the characteristic (0x02 - read only).

The next attribute is of length 0x0013 (19 bytes), access permissions are read-only by anyone (0x0002), attribute handle is 0x0003 (third line of database), attribute type is 0x2a00 (device name standard UUID), and the value is "PICO_SERVER" in ASCII. We see that the compiler has also generated a macro for the value handle for the GAP device name characteristic.

The next line of our textual database description, `PRIMARY_SERVICE, GATT_SERVICE` declares a new primary service. This line marks the beginning of the GATT_SERVICE, and all subsequent characteristics *before* the next primary service declaration will be included in this service. Similarly to before, this line of our text file gets compiled to a single attribute in our database:

```
const uint8_t profile_data[] =
{
    // ATT DB Version
    1,

    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18, 
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME - READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a, 
    // 0x0003 VALUE CHARACTERISTIC-GAP_DEVICE_NAME - READ -'PICO_SERVER'
    // READ_ANYBODY
    0x13, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x50, 0x49, 0x43, 0x4f, 0x5f, 0x53, 0x45, 0x52, 0x56, 0x45, 0x52, 
    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28, 0x01, 0x18, 
    .
    .
    .

}; // total size 234 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_GAP_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_GAP_SERVICE_END_HANDLE 0x0003
#define ATT_SERVICE_GATT_SERVICE_START_HANDLE 0x0004
#define ATT_SERVICE_GATT_SERVICE_END_HANDLE 0x0006
.
.
.

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0003
.
.
.
```

It has created an attribute with length field 0x000a (10 bytes), access permissions 0x0002 (read only by anyone), handle 0x0004 (4th line of database), type 0x2800 (UUID for primary service declaration) and value 0x1801 (UUID for GATT service). The compiler also generated a macro with which we can access the start handle for this service. The next line of our text file, `CHARACTERISTIC, GATT_DATABASE_HASH, READ,` adds a characteristic to this service. As before, this characteristic adds two attributes to our database:

```
const uint8_t profile_data[] =
{
    // ATT DB Version
    1,

    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18, 
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME - READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a, 
    // 0x0003 VALUE CHARACTERISTIC-GAP_DEVICE_NAME - READ -'PICO_SERVER'
    // READ_ANYBODY
    0x13, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x50, 0x49, 0x43, 0x4f, 0x5f, 0x53, 0x45, 0x52, 0x56, 0x45, 0x52, 
    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28, 0x01, 0x18, 
    // 0x0005 CHARACTERISTIC-GATT_DATABASE_HASH - READ
    0x0d, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28, 0x02, 0x06, 0x00, 0x2a, 0x2b, 
    // 0x0006 VALUE CHARACTERISTIC-GATT_DATABASE_HASH - READ -''
    // READ_ANYBODY
    0x18, 0x00, 0x02, 0x00, 0x06, 0x00, 0x2a, 0x2b, 0x71, 0xb4, 0x79, 0x9d, 0xc0, 0xbf, 0xca, 0xd9, 0x09, 0xbf, 0x6e, 0x8c, 0x73, 0xd1, 0x77, 0x8c, 
    .
    .
    .

}; // total size 234 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_GAP_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_GAP_SERVICE_END_HANDLE 0x0003
#define ATT_SERVICE_GATT_SERVICE_START_HANDLE 0x0004
#define ATT_SERVICE_GATT_SERVICE_END_HANDLE 0x0006
.
.
.

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_GATT_DATABASE_HASH_01_VALUE_HANDLE 0x0006
.
.
.
```

The first attribute is a characteristic declaration. It is of length 0x000d (13 bytes), access permissions are read only by anyone (0x0002), handle is 0x0005 (5th row of database), attribute type is 0x2803 (characteristic declaration), and the attribute value includes the characteristic's UUID (0x2b2a - database hash). The following attribute is of length 0x0018 (24 bytes), access permissions are 0x0002 (read only), handle is 0x0006 (6th row of database), type is 0x2b2a (database hash), and the value (16 bytes long) is a 128-bit hash value computed from the database structure. Any client which connects to this server could read this value and store it. If it loses its connection and then re-connects, and can use this value to confirm that the database hasn't changed. If it hasn't, it then doesn't need to re-learn the database structure.

### Adding a custom primary service

The rest of the textual description of the database describes a *custom* GATT service with an associated set of characteristics. The next line, `PRIMARY_SERVICE, 0000FF10-0000-1000-8000-00805F9B34FB`, declares this custom primary service. This line gets compiled as another attribute in our database:

```
const uint8_t profile_data[] =
{
    // ATT DB Version
    1,

    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18, 
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME - READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a, 
    // 0x0003 VALUE CHARACTERISTIC-GAP_DEVICE_NAME - READ -'PICO_SERVER'
    // READ_ANYBODY
    0x13, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x50, 0x49, 0x43, 0x4f, 0x5f, 0x53, 0x45, 0x52, 0x56, 0x45, 0x52, 
    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28, 0x01, 0x18, 
    // 0x0005 CHARACTERISTIC-GATT_DATABASE_HASH - READ
    0x0d, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28, 0x02, 0x06, 0x00, 0x2a, 0x2b, 
    // 0x0006 VALUE CHARACTERISTIC-GATT_DATABASE_HASH - READ -''
    // READ_ANYBODY
    0x18, 0x00, 0x02, 0x00, 0x06, 0x00, 0x2a, 0x2b, 0x71, 0xb4, 0x79, 0x9d, 0xc0, 0xbf, 0xca, 0xd9, 0x09, 0xbf, 0x6e, 0x8c, 0x73, 0xd1, 0x77, 0x8c, 
    // First custom service
    // 0x0007 PRIMARY_SERVICE-0000FF10-0000-1000-8000-00805F9B34FB
    0x18, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28, 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x10, 0xff, 0x00, 0x00, 
    .
    .
    .

}; // total size 234 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_GAP_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_GAP_SERVICE_END_HANDLE 0x0003
#define ATT_SERVICE_GATT_SERVICE_START_HANDLE 0x0004
#define ATT_SERVICE_GATT_SERVICE_END_HANDLE 0x0006
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_START_HANDLE 0x0007
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_END_HANDLE 0x001d
.
.
.

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_GATT_DATABASE_HASH_01_VALUE_HANDLE 0x0006
.
.
.
```

This attribute looks very similar to the primary service declaration attributes that we've seen before, except that its UUID is now 128-bits instead of 16. This attribute is of length 0x0018 (24), access permissions are read only (0x0002), handle is 0x0007 (7th row of database), type is 0x2800 (primary service declaration), and its value is the 128-bit UUID of the custom primary service: 0000FF10-0000-1000-8000-00805F9B34FB. The compiler has also generated a couple macros for accessing the start and end of this service.

### Adding characteristics to our custom service

Every subsequent line of our text-description of the attribute database adds characteristics (and characteristic descriptors) to this service. Let us examine the first of these characteristics:

```
// Characteristic A - read only, dynamic, with notifications
CHARACTERISTIC, 0000FF11-0000-1000-8000-00805F9B34FB, DYNAMIC | READ | NOTIFY,
```

This line adds a characteristic with a 128-bit custom UUID to our custom service. We've specified that this characteristic should be read-only, it should allow for notifications, and it should be ***dynamic\***. Because we've specified that notifications should be allowed for this characteristic, the compiler will automatically generate an additional client configuration characteristic and add it to the attribute database. Because we've indicated that this characteristic should be dynamic, reads/writes to this characteristic will be forwarded to a callback function that we'll write. More on this later. For now, let's look at the attributes that this line has added to our database:

```
const uint8_t profile_data[] =
{
    // ATT DB Version
    1,

    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18, 
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME - READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a, 
    // 0x0003 VALUE CHARACTERISTIC-GAP_DEVICE_NAME - READ -'PICO_SERVER'
    // READ_ANYBODY
    0x13, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x50, 0x49, 0x43, 0x4f, 0x5f, 0x53, 0x45, 0x52, 0x56, 0x45, 0x52, 
    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28, 0x01, 0x18, 
    // 0x0005 CHARACTERISTIC-GATT_DATABASE_HASH - READ
    0x0d, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28, 0x02, 0x06, 0x00, 0x2a, 0x2b, 
    // 0x0006 VALUE CHARACTERISTIC-GATT_DATABASE_HASH - READ -''
    // READ_ANYBODY
    0x18, 0x00, 0x02, 0x00, 0x06, 0x00, 0x2a, 0x2b, 0x71, 0xb4, 0x79, 0x9d, 0xc0, 0xbf, 0xca, 0xd9, 0x09, 0xbf, 0x6e, 0x8c, 0x73, 0xd1, 0x77, 0x8c, 
    // First custom service
    // 0x0007 PRIMARY_SERVICE-0000FF10-0000-1000-8000-00805F9B34FB
    0x18, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28, 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x10, 0xff, 0x00, 0x00, 
    // Characteristic A - read only, dynamic, with notifications
    // 0x0008 CHARACTERISTIC-0000FF11-0000-1000-8000-00805F9B34FB - DYNAMIC | READ | NOTIFY
    0x1b, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x12, 0x09, 0x00, 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x11, 0xff, 0x00, 0x00, 
    // 0x0009 VALUE CHARACTERISTIC-0000FF11-0000-1000-8000-00805F9B34FB - DYNAMIC | READ | NOTIFY
    // READ_ANYBODY
    0x16, 0x00, 0x02, 0x03, 0x09, 0x00, 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x11, 0xff, 0x00, 0x00, 
    // 0x000a CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x0a, 0x00, 0x02, 0x29, 0x00, 0x00, 
    .
    .
    .

}; // total size 234 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_GAP_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_GAP_SERVICE_END_HANDLE 0x0003
#define ATT_SERVICE_GATT_SERVICE_START_HANDLE 0x0004
#define ATT_SERVICE_GATT_SERVICE_END_HANDLE 0x0006
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_START_HANDLE 0x0007
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_END_HANDLE 0x001d
.
.
.

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_GATT_DATABASE_HASH_01_VALUE_HANDLE 0x0006
#define ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE 0x0009
#define ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE 0x000a
.
.
.
```

This line added three attributes to our database! The first of these is a familiar-looking characteristic declaration. It is of length 0x001b (27 bytes), has read-only access properties (0x0002), handle 0x0008 (8th row of database), type 0x2803 (characteristic declaration) and its value includes the access permissions of the characteristic that we're declaring (0x12 - read and write without response), the handle of the characteristic value (0x0009), and the 128-bit UUID of the characteristic (0000FF11-0000-1000-8000-00805F9B34FB).

The next attribute represents the *value* of our custom characteristic. It is of length 0x0016 (22 bytes), has access permissions of 0x0302 (read only, dynamic, and with a long UUID), has handle 0x0009, and 128-bit UUID 0000FF11_0000_1000_8000_00805F9B34FB. Note that this attribute does not include any value! This is because we've indicated it as *dynamic*, so any read/write request to this attribute will be sent to a callback function for us to manage in our own code.

The third attribute is a client configuration attribute that the compiler automatically added because we specified that this characteristic should allow for notifications. This attribute is of length 0x000a (10 bytes), has access permissions of 0x010e (read, write without response, write, and dynamic), handle 0x000a (10th line of database), type 0x2902 (standard UUID for client characteristic configuration), and value 0x0000 (notifications and indications are OFF). Note also that the compiler generated macros for accessing the characteristic value handle and the characteristic configuration handle.

Let's look at one more line of our text-description of the attribute database, and then we will have covered a representative set. The next line, `CHARACTERISTIC_USER_DESCRIPTION, READ,` adds a user description for the characteristic which we've just added.

```
const uint8_t profile_data[] =
{
    // ATT DB Version
    1,

    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18, 
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME - READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a, 
    // 0x0003 VALUE CHARACTERISTIC-GAP_DEVICE_NAME - READ -'PICO_SERVER'
    // READ_ANYBODY
    0x13, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x50, 0x49, 0x43, 0x4f, 0x5f, 0x53, 0x45, 0x52, 0x56, 0x45, 0x52, 
    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28, 0x01, 0x18, 
    // 0x0005 CHARACTERISTIC-GATT_DATABASE_HASH - READ
    0x0d, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28, 0x02, 0x06, 0x00, 0x2a, 0x2b, 
    // 0x0006 VALUE CHARACTERISTIC-GATT_DATABASE_HASH - READ -''
    // READ_ANYBODY
    0x18, 0x00, 0x02, 0x00, 0x06, 0x00, 0x2a, 0x2b, 0x71, 0xb4, 0x79, 0x9d, 0xc0, 0xbf, 0xca, 0xd9, 0x09, 0xbf, 0x6e, 0x8c, 0x73, 0xd1, 0x77, 0x8c, 
    // First custom service
    // 0x0007 PRIMARY_SERVICE-0000FF10-0000-1000-8000-00805F9B34FB
    0x18, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28, 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x10, 0xff, 0x00, 0x00, 
    // Characteristic A - read only, dynamic, with notifications
    // 0x0008 CHARACTERISTIC-0000FF11-0000-1000-8000-00805F9B34FB - DYNAMIC | READ | NOTIFY
    0x1b, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x12, 0x09, 0x00, 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x11, 0xff, 0x00, 0x00, 
    // 0x0009 VALUE CHARACTERISTIC-0000FF11-0000-1000-8000-00805F9B34FB - DYNAMIC | READ | NOTIFY
    // READ_ANYBODY
    0x16, 0x00, 0x02, 0x03, 0x09, 0x00, 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x11, 0xff, 0x00, 0x00, 
    // 0x000a CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x0a, 0x00, 0x02, 0x29, 0x00, 0x00, 
    // 0x000b USER_DESCRIPTION-READ-
    // READ_ANYBODY, WRITE_ANYBODY
    0x08, 0x00, 0x0a, 0x01, 0x0b, 0x00, 0x01, 0x29, 
    .
    .
    .

}; // total size 234 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_GAP_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_GAP_SERVICE_END_HANDLE 0x0003
#define ATT_SERVICE_GATT_SERVICE_START_HANDLE 0x0004
#define ATT_SERVICE_GATT_SERVICE_END_HANDLE 0x0006
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_START_HANDLE 0x0007
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_END_HANDLE 0x001d
.
.
.

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_GATT_DATABASE_HASH_01_VALUE_HANDLE 0x0006
#define ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE 0x0009
#define ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE 0x000a
#define ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_USER_DESCRIPTION_HANDLE 0x000b
.
.
.
```

This line generates one more attribute in our attribute database. This attribue is of length 0x0008 (8 bytes), has access properties 0x010a (read, write, dynamic), has a handle of 0x000b (11th row of database), and a type of 0x2901 (standard UUID for characteristic user description). Note that this attribute *also* doesn't have a value, because it is dynamic. Reads/writes will be managed by a callback function. The compiler also generated a macro for accessing this attribute.

You may have noticed that the names of the macros for the characteristic configuration handle and the user description handle have the correct UUID for the characteristic to which they apply. In much the same way that all characteristics that appear beneath a primary service declaration are associated with that service (until the next service declaration appears), so too do characteristic configurations/descriptions apply to the characteristic declarations which immediately precede them.

The custom GATT server demo includes a handful more characteristics, but at this point we've discussed all relevant topics for understanding those which remain. Let us move on.

### Compiling our ATT database

To compile our ATT database, we simply add a [single line](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/3490e766c48b0fc623680eefca87e267382233ca/Bluetooth/GATT_Server/CMakeLists.txt#L28) to our CMakeLists.txt file. The line linked above is also copied below. This line simply calls the Python compiler on the .gatt file at the directory address provided as an argument. The compiled database will appear in `[project directory]/build/generated`.

```
pico_btstack_make_gatt_header(gatt_server_demo PRIVATE "${CMAKE_CURRENT_LIST_DIR}/GATT_Service/server_demo_gattfile.gatt")
```

## Implementing the GATT server

With our database constructed, we must now implement the logic that allows for a GATT client to issue read and write requests to this database. Actually, we only need to implement *some* of this logic. BTstack will automatically handle read/write requests to all of the database characteristics that we did not specify as being `DYNAMIC`. In the case of the database above, these include the GAP device name and the GATT database hash. For all other characteristics, we must write the callback functions which specify what should be done when a GATT client issues a read or write request.

Please note that the logic discussed in this section appears in the helper file [service_implementation.h](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Bluetooth/GATT_Server/GATT_Service/service_implementation.h).

### A database struct

As a matter of convenience, it's useful to assemble a struct which abstracts the dynamic characteristics of our database. This way, our read and write callback functions can perform operations on human-readable fields of this struct, rather than on slightly-obscure compiler-generated macros. When we initialize this struct, we'll assign the fields the values of these obscure macros *once* (more on that in a bit), and then our application code never needs to deal with them again. We can also use this struct to organize some of the other control and communication interfaces to our database.

The first field in this struct is an object of type `hci_con_handle_t` called `con_handle`. The HCI connection handle is a 12-bit value that identifies the channel connection between two Bluetooth devices (or between separate applications on the same two devices). This value will be automatically provided to us in the LE connection complete event, we needn't worry about initializing it.

The subsequent 16 fields (all of the ones under comment-headers of the variety `Characteristic [X] information`) are the data structures, or pointers to data structures, which hold the actual data associated with our database. When the client makes a read or write request to any of our dynamic characteristics, our callback functions will ultimately access these variables (or the variables to which these point). Recall that the attributes associated with these dynamic characteristics did not have any value field, here's where those values live! These read/write requests specify the characteristic that they'd like to access using that characteristic's *handle*. The next set of fields (all of the ones under comment-headers of the variety `Characteristic [X] handles`) hold these handle values. During intialization, we will assign each of these fields its associated handle value, as generated by the Python GATT compiler.

Finally, the last four fields of this struct are objects of type `btstack_context_callback_registration_t`. We can use these to schedule callback functions for characteristics that our application modifies, and for which we'd like to send a notification to the client. These callback registrations allow for us to send Notifications and Indications independently from the main application. We can add the callback function to the BTstack run loop such that these notifications/indications are sent when it's possible to do so. We'll see this in more detail in just a bit.

In our [service_implementation.h](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Bluetooth/GATT_Server/GATT_Service/service_implementation.h) file, we create [an object of this type](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/3490e766c48b0fc623680eefca87e267382233ca/Bluetooth/GATT_Server/GATT_Service/service_implementation.h#L95). When we initialize our server, we'll associate the fields of this struct with the values generated by the Python compiler. More on that in the Initialization section.

```
// Create a struct for managing this service
typedef struct {

    // Connection handle for service
    hci_con_handle_t con_handle ;

    // Characteristic A information
    char *      characteristic_a_value ;
    uint16_t    characteristic_a_client_configuration ;
    char *      characteristic_a_user_description ;

    // Characteristic B information
    char *      characteristic_b_value ;
    uint16_t    characteristic_b_client_configuration ;
    char *      characteristic_b_user_description ;

    // Characteristic C information
    char *      characteristic_c_value ;
    uint16_t    characteristic_c_client_configuration ;
    char *      characteristic_c_user_description ;

    // Characteristic D information
    char *      characteristic_d_value ;
    uint16_t    characteristic_d_client_configuration ;
    char *      characteristic_d_user_description ;

    // Characteristic E information
    char *      characteristic_e_value ;
    char *      characteristic_e_user_description ;

    // Characteristic F information
    char *      characteristic_f_value ;
    char *      characteristic_f_user_description ;

    // Characteristic A handles
    uint16_t    characteristic_a_handle ;
    uint16_t    characteristic_a_client_configuration_handle ;
    uint16_t    characteristic_a_user_description_handle ;

    // Characteristic B handles
    uint16_t    characteristic_b_handle ;
    uint16_t    characteristic_b_client_configuration_handle ;
    uint16_t    characteristic_b_user_description_handle ;

    // Characteristic C handles
    uint16_t    characteristic_c_handle ;
    uint16_t    characteristic_c_client_configuration_handle ;
    uint16_t    characteristic_c_user_description_handle ;

    // Characteristic D handles
    uint16_t    characteristic_d_handle ;
    uint16_t    characteristic_d_client_configuration_handle ;
    uint16_t    characteristic_d_user_description_handle ;

    // Characteristic E handles
    uint16_t    characteristic_e_handle ;
    uint16_t    characteristic_e_user_description_handle ;

    // Characteristic F handles
    uint16_t    characteristic_f_handle ;
    uint16_t    characteristic_f_user_description_handle ;

    // Callback functions
    btstack_context_callback_registration_t callback_a ;
    btstack_context_callback_registration_t callback_b ;
    btstack_context_callback_registration_t callback_c ;
    btstack_context_callback_registration_t callback_d ;

} custom_service_t ;
```

### Read callback

When a client issues a read request to one of our dynamic characteristics, that request gets emitted to a callback function. These callback functions are typedef'd in [add_db.h](https://github.com/bluekitchen/btstack/blob/master/src/ble/att_db.h#L144) and, [as you can see](https://github.com/bluekitchen/btstack/blob/2b49e57bd1fae85ac32ac1f41cdb7c794de335f6/src/ble/att_db.h#L144), they take a specified set of arguments. When we create our own callback functions, they should use this same set of arguments. Behind the scenes, when BTstack emits a read/write request to our read callback function, it will pass that function the hci connection handle associated with the request, the attribute handle (per our database, as discussed previously), an offset that specifies the start of the attribute *value*, a buffer and a buffer size. We will use this information to take whatever action we desire when a GATT client performs a read request to one of our dynamic characteristics.

These actions will *mostly* make use of the [att database API](https://github.com/bluekitchen/btstack/blob/master/src/ble/att_db.h). As you can see below, our particular read callback function contains a series of conditionals. We check whether the attribute handle of the emitted attribute read request matches any of the attribute handles in our database struct. In the event that there is a match, we use a function fromt he att database API to read the associated value for that characteristic. In the event that the client issued a read request for a characteristic's value or user description, we use the `att_read_handle_blob` function in order to read an array of bytes back to the client. The arguments for this function include the address at which to start reading, the number of bytes to read, the offset from att_read_callback, the buffer and buffer size from att_read_callback. Alternatively, in the event that the client issued a read request for the characteristic configuration, we make use of `att_read_callback_handle_little_endian_16()`, which takes the value to be read, the offset from att_read_callback, the buffer from att_read_callback, and the buffer size from att_read_callback.

In brief, the attribute handle tells use which attribute the client wants to read, and then the att database API from BTstack provides the mechanism for reading the value for the specified attribute.

```
// Read callback (no client configuration handles on characteristics without Notify)
static uint16_t custom_service_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
    UNUSED(con_handle);

    // Characteristic A
    if (attribute_handle == service_object.characteristic_a_handle){
        return att_read_callback_handle_blob(service_object.characteristic_a_value, strlen(service_object.characteristic_a_value), offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_a_client_configuration_handle){
        return att_read_callback_handle_little_endian_16(service_object.characteristic_a_client_configuration, offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_a_user_description_handle) {
        return att_read_callback_handle_blob(service_object.characteristic_a_user_description, strlen(service_object.characteristic_a_user_description), offset, buffer, buffer_size);
    }

    // Characteristic B
    if (attribute_handle == service_object.characteristic_b_handle){
        return att_read_callback_handle_blob(service_object.characteristic_b_value, strlen(service_object.characteristic_b_value), offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_b_client_configuration_handle){
        return att_read_callback_handle_little_endian_16(service_object.characteristic_b_client_configuration, offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_b_user_description_handle) {
        return att_read_callback_handle_blob(service_object.characteristic_b_user_description, strlen(service_object.characteristic_b_user_description), offset, buffer, buffer_size);
    }

    // Characteristic C
    if (attribute_handle == service_object.characteristic_c_handle){
        return att_read_callback_handle_blob(service_object.characteristic_c_value, strlen(service_object.characteristic_c_value), offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_c_client_configuration_handle){
        return att_read_callback_handle_little_endian_16(service_object.characteristic_c_client_configuration, offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_c_user_description_handle) {
        return att_read_callback_handle_blob(service_object.characteristic_c_user_description, strlen(service_object.characteristic_c_user_description), offset, buffer, buffer_size);
    }

    // Characteristic D
    if (attribute_handle == service_object.characteristic_d_handle){
        return att_read_callback_handle_blob(service_object.characteristic_d_value, strlen(service_object.characteristic_d_value), offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_d_client_configuration_handle){
        return att_read_callback_handle_little_endian_16(service_object.characteristic_d_client_configuration, offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_d_user_description_handle) {
        return att_read_callback_handle_blob(service_object.characteristic_d_user_description, strlen(service_object.characteristic_d_user_description), offset, buffer, buffer_size);
    }

    // Characteristic E
    if (attribute_handle == service_object.characteristic_e_handle){
        return att_read_callback_handle_blob(service_object.characteristic_e_value, strlen(service_object.characteristic_e_value), offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_e_user_description_handle) {
        return att_read_callback_handle_blob(service_object.characteristic_e_user_description, strlen(service_object.characteristic_e_user_description), offset, buffer, buffer_size);
    }

    // Characteristic E
    if (attribute_handle == service_object.characteristic_f_handle){
        return att_read_callback_handle_blob(service_object.characteristic_f_value, strlen(service_object.characteristic_f_value), offset, buffer, buffer_size);
    }
    if (attribute_handle == service_object.characteristic_f_user_description_handle) {
        return att_read_callback_handle_blob(service_object.characteristic_f_user_description, strlen(service_object.characteristic_f_user_description), offset, buffer, buffer_size);
    }

    return 0;
}
```

### Write callback

Like the read callback, the write callback takes a [specified set of arguments](https://github.com/bluekitchen/btstack/blob/2b49e57bd1fae85ac32ac1f41cdb7c794de335f6/src/ble/att_db.h#L164). When the GATT client issues a write request to one of our dynamic characteristics, BTstack will emit that write request to our write callback function. It calls that function with arguments that include the hci connection handle, the handle of the attribute that is to be written, the transaction mode (we won't use this), the offset into the attribute value, a pointer to a buffer which contains the data to write, and the length of that buffer. It is up to us, in the callback function that we write, to update the specified attribute values in our database. This callback function also sends a notification to the client in the event that notifications are enabled for the particular characteristic being updated.

As you can see below, the write callback is structurally similar to the read callback. It checks the handle of the attribute that is to be written, and takes different actions based on the handle value. It then uses the data in the buffer to update one of the fields of the database struct. This particular implementation represents characteristic values as char arrays. So, if the attribute being updated is a characteristic *value*, the callback function copies the contents of the buffer to the database array which holds that characteristic's value (being sure to null terminate it).

In the event that the client is performing a write request to a characteristic *configuration* (i.e., turning notifications on/off), then the callback function simply copies the first to bytes of the buffer to the database struct field which represents that characteristic's notification status. As you can see in some of the conditionals below, this field is used to evaluate whether or not the server should send the client a notification that the characteristic value has updated. More on the details of this in a bit.

The other thing you'll notice in some of the conditionals below is `PT_SEM_SAFE_SIGNAL(pt, &BLUETOOTH_READY) ;`. This is a semaphore to an application thread that takes an action based on a changed characteristic value. More on this application in a bit too.

```
// Write callback
static int custom_service_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(buffer_size);

    // Enable/disable notifications
    if (attribute_handle == service_object.characteristic_a_client_configuration_handle){
        service_object.characteristic_a_client_configuration = little_endian_read_16(buffer, 0);
        service_object.con_handle = con_handle;
    }
    // Enable/disable notifications
    if (attribute_handle == service_object.characteristic_b_client_configuration_handle){
        service_object.characteristic_b_client_configuration = little_endian_read_16(buffer, 0);
        service_object.con_handle = con_handle;
    }

    // Write characteristic value
    if (attribute_handle == service_object.characteristic_b_handle) {
        custom_service_t * instance = &service_object ;
        buffer[buffer_size] = 0 ;
        memset(service_object.characteristic_b_value, 0, strlen(service_object.characteristic_b_value)) ;
        memcpy(service_object.characteristic_b_value, buffer, strlen(buffer)) ;
        // If client has enabled notifications, register a callback
        if (instance->characteristic_b_client_configuration) {
            instance->callback_b.callback = &characteristic_b_callback ;
            instance->callback_b.context = (void*) instance ;
            att_server_register_can_send_now_callback(&instance->callback_b, instance->con_handle) ;
        }
        // Alert the application of a bluetooth RX
        PT_SEM_SAFE_SIGNAL(pt, &BLUETOOTH_READY) ;
    }

    // Enable/disable notificatins
    if (attribute_handle == service_object.characteristic_c_client_configuration_handle){
        service_object.characteristic_c_client_configuration = little_endian_read_16(buffer, 0);
        service_object.con_handle = con_handle;
    }

    // Enable/disable notificatins
    if (attribute_handle == service_object.characteristic_d_client_configuration_handle){
        service_object.characteristic_d_client_configuration = little_endian_read_16(buffer, 0);
        service_object.con_handle = con_handle;
    }

    // Write characteristic value
    if (attribute_handle == service_object.characteristic_d_handle) {
        custom_service_t * instance = &service_object ;
        buffer[buffer_size] = 0 ;
        memset(service_object.characteristic_d_value, 0, sizeof(service_object.characteristic_d_value)) ;
        memcpy(service_object.characteristic_d_value, buffer, buffer_size) ;
        // Null-terminate the string
        service_object.characteristic_d_value[buffer_size] = 0 ;
        // If client has enabled notifications, register a callback
        if (instance->characteristic_d_client_configuration) {
            instance->callback_d.callback = &characteristic_d_callback ;
            instance->callback_d.context = (void*) instance ;
            att_server_register_can_send_now_callback(&instance->callback_d, instance->con_handle) ;
        }
        // Alert the application of a bluetooth RX
        PT_SEM_SAFE_SIGNAL(pt, &BLUETOOTH_READY) ;
    }

    // Write characteristic value
    if (attribute_handle == service_object.characteristic_e_handle) {
        custom_service_t * instance = &service_object ;
        buffer[buffer_size] = 0 ;
        if (!strcmp(buffer, "OFF")) {
            memset(service_object.characteristic_e_value, 0, sizeof(service_object.characteristic_e_value)) ;
            memcpy(service_object.characteristic_e_value, buffer, buffer_size) ;
            service_object.characteristic_e_value[buffer_size] = 0 ;
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            // Alert the application of a bluetooth RX
            PT_SEM_SAFE_SIGNAL(pt, &BLUETOOTH_READY) ;
        }
        else if (!strcmp(buffer, "ON")) {
            memset(service_object.characteristic_e_value, 0, sizeof(service_object.characteristic_e_value)) ;
            memcpy(service_object.characteristic_e_value, buffer, buffer_size) ;
            service_object.characteristic_e_value[buffer_size] = 0 ;
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            // Alert the application of a bluetooth RX
            PT_SEM_SAFE_SIGNAL(pt, &BLUETOOTH_READY) ;
        }
    }

    // Write characteristic value
    if (attribute_handle == service_object.characteristic_f_handle) {
        custom_service_t * instance = &service_object ;
        buffer[buffer_size] = 0 ;
        if(atoi(buffer)<16) {
            memset(service_object.characteristic_f_value, 0, strlen(service_object.characteristic_f_value)) ;
            memcpy(service_object.characteristic_f_value, buffer, strlen(buffer)) ;
            // Null-terminate the string
            service_object.characteristic_f_value[buffer_size] = 0 ;
            // Alert the application of a bluetooth RX
            PT_SEM_SAFE_SIGNAL(pt, &BLUETOOTH_READY) ;
        }
    }

    return 0;
}
```

## Building a small API for interacting with our server

We would like for our application code to be able to initialize our GATT server, and we would like for our application to make its *own* updates to some of the characteristics in that server, and optionally notify the client when those updates occur. To facilitate this, we'll build ourselves a small API to use in our application code.

### Initialization function

You'll note that the above read/write callback functions above operate on fields of our database struct. This struct is organized to mirror the att database that a Python compiler generates from our textual description of the server. When we initialize our server, we need to associate the fields of our struct with the values generated by the Python compiler.

The [service_implementation.h file](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Bluetooth/GATT_Server/GATT_Service/service_implementation.h) globally declares an object of type `custom_service_t` (our abstraction of the att server), and an object of type `att_service_handler_t`. This is a BTstack-defined type for handling GATT services. We also declare a series of character arrays that store the user descriptions of each characteristic in our server.

The initialization function takes six arguments, each of which is a pointer to a char array that holds the value for one of our custom characteristics. The function initializes the semaphore that we use to signal the application code, and then it assigns the fields associated with each of our characteristic *values* to the pointers which we pass in as arguments. The function assigns the fields for user descriptions to the addresses of each of the global char arrays that hold these descriptions, and it assigns all the fields associated with attribute *handles* to the macros generated by the Python compiler. This associates all the fields of our handy struct abstraction to the slightly-obscure macros that are automatically generated.

Lastly, the initialization function assigns values to fields of the `att_service_handler_t` object `service_handler`. In particular, it sets the start and end handles to values from the Python-generated database, and it sets the addresses of the read/write callback functions to those of the functions described above.

The last line of this function registers the service handler. This registers read and write callbacks for the handle range specified by the start and end handle fields of the `service_handler`.

```
// Create a callback registration object, and an att service handler object
static att_service_handler_t    service_handler ;
static custom_service_t         service_object ;

// Characteristic user descriptions (appear in LightBlue app)
char characteristic_a[] = "Read-only Counter" ;
char characteristic_b[] = "DDS Frequency" ;
char characteristic_c[] = "String from Pico" ;
char characteristic_d[] = "String to Pico" ;
char characteristic_e[] = "LED Status and Control" ;
char characteristic_f[] = "Color Selection" ;

// Initialize our custom service handler
void custom_service_server_init(char * a_ptr, char * b_ptr, char * c_ptr, char * d_ptr, char * e_ptr, char * f_ptr){

    // Initialize the semaphore
    PT_SEM_SAFE_INIT(&BLUETOOTH_READY, 0) ;

    // Pointer to our service object
    custom_service_t * instance = &service_object ;

    // Assign characteristic value
    instance->characteristic_a_value = a_ptr ;
    instance->characteristic_b_value = b_ptr ;
    instance->characteristic_c_value = c_ptr ;
    instance->characteristic_d_value = d_ptr ;
    instance->characteristic_e_value = e_ptr ;
    instance->characteristic_f_value = f_ptr ;

    // Assign characteristic user description
    instance->characteristic_a_user_description = characteristic_a ;
    instance->characteristic_b_user_description = characteristic_b ;
    instance->characteristic_c_user_description = characteristic_c ;
    instance->characteristic_d_user_description = characteristic_d ;
    instance->characteristic_e_user_description = characteristic_e ;
    instance->characteristic_f_user_description = characteristic_f ;

    // Assign handle values (from generated gatt header)
    instance->characteristic_a_handle = ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE ;
    instance->characteristic_a_client_configuration_handle = ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE ;
    instance->characteristic_a_user_description_handle = ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_USER_DESCRIPTION_HANDLE ;

    instance->characteristic_b_handle = ATT_CHARACTERISTIC_0000FF12_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE ;
    instance->characteristic_b_client_configuration_handle = ATT_CHARACTERISTIC_0000FF12_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE ;
    instance->characteristic_b_user_description_handle = ATT_CHARACTERISTIC_0000FF12_0000_1000_8000_00805F9B34FB_01_USER_DESCRIPTION_HANDLE ;

    instance->characteristic_c_handle = ATT_CHARACTERISTIC_0000FF13_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE ;
    instance->characteristic_c_client_configuration_handle = ATT_CHARACTERISTIC_0000FF13_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE ;
    instance->characteristic_c_user_description_handle = ATT_CHARACTERISTIC_0000FF13_0000_1000_8000_00805F9B34FB_01_USER_DESCRIPTION_HANDLE ;

    instance->characteristic_d_handle = ATT_CHARACTERISTIC_0000FF14_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE ;
    instance->characteristic_d_client_configuration_handle = ATT_CHARACTERISTIC_0000FF14_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE ;
    instance->characteristic_d_user_description_handle = ATT_CHARACTERISTIC_0000FF14_0000_1000_8000_00805F9B34FB_01_USER_DESCRIPTION_HANDLE ;

    instance->characteristic_e_handle = ATT_CHARACTERISTIC_0000FF15_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE ;
    instance->characteristic_e_user_description_handle = ATT_CHARACTERISTIC_0000FF15_0000_1000_8000_00805F9B34FB_01_USER_DESCRIPTION_HANDLE ;

    instance->characteristic_f_handle = ATT_CHARACTERISTIC_0000FF16_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE ;
    instance->characteristic_f_user_description_handle = ATT_CHARACTERISTIC_0000FF16_0000_1000_8000_00805F9B34FB_01_USER_DESCRIPTION_HANDLE ;

    // Service start and end handles (modeled off heartrate example)
    service_handler.start_handle = ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_START_HANDLE ;
    service_handler.end_handle = ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_END_HANDLE ;
    service_handler.read_callback = &custom_service_read_callback ;
    service_handler.write_callback = &custom_service_write_callback ;

    // Register the service handler
    att_server_register_service_handler(&service_handler);
}
```

### Functions for updating characteristics

For each characteristic that we'd like for our application code to be able to modify, we generate a function like the one shown below. This function takes the new value for the characteristic as an argument, and uses `sprintf` to write that value as a character array to array associated with that characteristic. It then checks whether or not notifications have been enabled for that characteristic and, in the event that they have, the function uses `att_server_register_can_send_now_callback` to schedule the callback function for execution at the next available opportunity.

```
// Update Characteristic A value
void set_characteristic_a_value(int value){

    // Pointer to our service object
    custom_service_t * instance = &service_object ;

    // Update field value
    sprintf(instance->characteristic_a_value, "%d", value) ;

    // Are notifications enabled? If so, register a callback
    if (instance->characteristic_a_client_configuration){
        instance->callback_a.callback = &characteristic_a_callback;
        instance->callback_a.context  = (void*) instance;
        att_server_register_can_send_now_callback(&instance->callback_a, instance->con_handle);;
    }
}
```

The callback functions for updating characteristics look like the one shown below. It uses BTstack's [ATT server API](https://github.com/bluekitchen/btstack/blob/master/src/ble/att_server.h) to send a notification for this character. `att_server_notify` takes the HCI connection handle, the characteristic handle, the characteristic value, and the length of the characteristic value.

```
static void characteristic_a_callback(void * context){
    // Associate the void pointer input with our custom service object
    custom_service_t * instance = (custom_service_t *) context ;
    // Send a notification
    att_server_notify(instance->con_handle, instance->characteristic_a_handle, instance->characteristic_a_value, strlen(instance->characteristic_a_value)) ;
}
```

## Instantiating the server

We instantiate our GATT server in `main()` of our [application source file](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Bluetooth/GATT_Server/server_demo.c). After intializing stdio and our [VGA driver](https://vanhunteradams.com/Pico/VGA/VGA.html), we perform server initializations and instantiations. This begins by initializing the CYW43 driver architecture via `cyw43_arch_init()`. For a deep dive into what happens when we call this function, please see [this webpage](https://vanhunteradams.com/Pico/BLE/BTStack_HCI.html).

The subsequent two lines initialize the L2CAP and Security Manager layers of the Bluetooth stack. We then initialize an ATT server by calling `att_server_init`. The first argument of this function, `profile_data`, is the name of the Python-compiler-generated attribute database discussed previously. The next two arguments, both of which are `NULL`, point to read and write callback functions for the ATT server. Because we've written a custom service handler with its own pair of read and write callbacks, we needn't pass anything into these arguments.

The next line calls our own initialization function! We pass pointers to a collection of char arrays which we'll use to store the values for each characteristic. These arrays are globally declared. Recall that this function associates all the fields of our handy database struct with the values generated by the Python compiler. It sets up an ATT service handler object, and associates our read/write callbacks with that service handler.

At this point, the server is setup! We still need to make this server discoverable/connectable to GATT clients, however. We accomplish this by configuring our Generic Access Protocol (GAP) advertising packets by way of the packet handler registered in the next three lines of the initialization sequence below. GAP, and this packet handler, is the next component of the system that we'll discuss.

```
// We send data as formatted strings (just like a serial console)
static char characteristic_a_tx[100] ;
static char characteristic_b_tx[100] ;
static char characteristic_c_tx[100] ;
static char characteristic_d_rx[100] ;
static char characteristic_e_tx[5] ;
static char characteristic_f_tx[2] ;

int main() {
    // Initialize stdio
    stdio_init_all();

    // Initialize the VGA
    initVGA() ;

    /////////// EVERYTHING BELOW INSTANTIATES AND INTIALIZES GATT SERVER ///////////////

    // initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    // Initialize L2CAP and security manager
    l2cap_init();
    sm_init();

    // Initialize ATT server, no general read/write callbacks
    // because we'll set one up for each service
    att_server_init(profile_data, NULL, NULL);   

    // Instantiate our custom service handler
    custom_service_server_init( characteristic_a_tx, characteristic_b_tx,
                                characteristic_c_tx, characteristic_d_rx,
                                characteristic_e_tx, characteristic_f_tx) ;

    // inform about BTstack state
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // register for ATT event
    att_server_register_packet_handler(packet_handler);

    // turn on bluetooth!
    hci_power_control(HCI_POWER_ON);

    .
    .
    .
}
```

## Advertising and connecting with Generic Access Profile (GAP)

Before a client can connect with our server and exchange data over GATT, it must first be made aware of that server's existance, and then it must negotiate a connection with it. Both of these processes are facilitated by the Generic Access Profile (GAP). Let us have a brief discussion about GAP in general, and then we'll take a look at the particular GAP advertising packet used by this project.

### Some background on GAP

GAP defines how devices discover and connect with one another. Within a BLE network, a device may adopt one of four possible GAP roles:

**Broadcaster**: Just transmits data, to be received without acknowledgement by anyone who's listening.

**Observer**: Just receives data from broadcasting devices, without acknowledging receipt.

**Central**: Connects with peripheral devices that it finds by means of their advertising packets.

**Peripheral**: Sends out advertising packets to allow for central devices to find it and subsequently connect with it.

As with all the layers of the BLE protocol, there is an asymmetry present in these roles. It takes more resources to be a *central* than it does to be a *peripheral*. Much like the GATT server that just responds to requests from the GATT client, the GAP peripheral largely just responds to requests from a GAP central device. Our Pico W GATT server will be a GAP peripheral. It will advertise itself to any listening central devices (e.g. a phone running LightBlue, or another Pico W running a GATT client), and then it will accept a connection from that central device. Once connected, the two exchange data via GATT.

The peripheral advertises itself by means of *advertising packets*. These packets contain up to 31 bytes of data (if scan request/response is enabled this could be doubled to 62 bytes), and these data contain information for listening central and observer devices. Broadcasting devices may *only* contain data (e.g. temperature) and some metadata about that data. A peripheral's advertising packet will include information about its *discoverability*, among other things. Much like the attribute database, each of these data structures is specified by a UUID.

It is often the case that the first data byte of a peripheral advertising packet is the Flags AD Type, which specifies the discoverability of the advertising peripheral. A peripheral may advertise itself to be in general-discoverable mode, which means that the peripheral will remain discoverable for as long as necessary. The peripheral may alternatively advertise itself as being in limited discoverable mode, which indicates that it will only be discoverable for a short amount of time (generally, central devices like phones will place these devices at the top of lists of discovered Bluetooth devices to facilitate quick connection by the user). If you've used a device with a "press to connect" button, that button likely put the device into limited discoverable mode. Finally, the peripheral may advertise itself as non-discoverable. This tells listening devices that theymay not report the presence of the peripheral or perform any inquiries about it. Such a device could still send broadcast packets that contain a warning to receiving devices not to follow-up. We will place our device in general discoverable mode.

The rest of the advertising packet might look different for different devices. Some common AD types that a packet might include are the device's local name (for population on lists of discovered devices), its appearance (so that phones and computers can associate icons with the device), its tx power level (so that, by comparing with the rx power level, the central device can generate an estimate of the distance to the peripheral), a complete or partial list of GATT services offered by the device, recommendations about connetion interval ranges for which the peripheral is best suited, a service solicitation (we might see this from an advertising client looking for a particular type of server), service data, or manufacturer-specific data.

Each advertising GAP device also *must* contain a GAP service in its GATT server. Any connected device may read this service without any security requirements, and it offers information about the device name, its appearance, and possibly some preferred connection parameter information.

### Building a GAP advertising packet

Our GATT server's advertising packet lives in [gap_config.h](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Bluetooth/GATT_Server/GAP_Advertisement/gap_config.h), and is copied below. You can see that each AD Type is in its own line for readability. Each of these lines starts with a byte that represents the length of the associated AD Type. This length field is followed by the UUID (represented as a BTstack-provided macro) of the AD Type, and then followed by the data associated with that AD Type.

So, for instance, the first AD Type in our advertising packet is the Flags AD. This AD Type is 2 bytes long, the first byte is the UUID for the Flags AD, and the second byte is the value. Our chosen value (0x06) indicates to listening devices that our device is general discoverable.

The second AD Type is 5 bytes in length, and represents the local name of the advertising device (`BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME`). The data associated with this AD Type is the local name of the device in ASCII. In our case, this is "Pico."

The final AD Type is a complete list of 16-bit service class UUID's. The data associated with this AD Type is the 16-bit version of the UUID for the custom service that we've built for our GATT server. The Pico-based GATT client (discussed later) will filter on this field in order to connect with servers that offer the services that it's looking for. (Please note that BTstack does offer an API for assembling this packet using a series of function calls. It seemed clearer for purposes of this document to write it out explicitly).

```
// Flag for general discoverability
#define APP_AD_FLAGS 0x06

// GAP data packet (must not exceed 32 bytes)
static uint8_t adv_data[] = {
    // Flags general discoverable
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, APP_AD_FLAGS,
    // Name
    0x05, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'P', 'i', 'c', 'o',
    // Custom Service UUID
    0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x10, 0xFF,
};
```

### A packet handler for setting up and starting GAP advertisements

In the code associated with the [Instantiating the server](https://vanhunteradams.com/Pico/BLE/GATT_Server.html#Instantiating-the-server) section above, you'll see the following two lines:

```
// inform about BTstack state
hci_event_callback_registration.callback = &packet_handler;
hci_add_event_handler(&hci_event_callback_registration);
```

These two lines associate a custom packet handler for HCI events that BTstack emits. That custom packet handler is also implemented in [gap_config.h](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Bluetooth/GATT_Server/GAP_Advertisement/gap_config.h), and it's copied below. You can see that this packet handler is called with the packet type and a pointer to a char array which contains the received packet (along with a parameter that represents the size of this array). The packet handler below uses this information to setup and start GAP advertisements when our system boots up.

The handler first confirms that the packet type is indeed an HCI_EVENT_PACKET, and then it extracts the event type from the packet. The first event that the system emits is `BTSTACK_EVENT_STATE`. When a packet of this type is received, the packet handler confirms that the system has booted properly and then it configures and starts the GAP advertisements. In particular, it associates a randomly generated local address with our device and sets the advertising interval to 800 ms. It then sets the advertising type to 0. There exist four different advertising types:

**Connectable undirected**: used for general advertising, allows for any other device to connect. *This is the type that we're using*.

**Connectable directed**: Requests for a *particular* peer device (with a known address) to connect.

**Scannable undirected**: Broadcasts advertising data to active scanners.

**Nonconnectable undirected**: Just broadcasts advertising data.

The next argument specifies the direct address type as 0 (public device address), and sets the direct address to all zeroes (we aren't using connecteable directed advertising, so this field won't be used). The penultimate argument is the channel map, which specifies which of the three advertising channels should be used. And the final argument is the filter policy, which indicates whether or not an address whitelist is being used to filter advertising packets that may be received in response to those we send out. By setting this argument to zero, we tell BTstack that we aren't using a whitelist, we'll accept packets from anybody.

The packet handler then confirms that the length of the advertising packet is less than 31 bytes, and then it sets the advertising data to the char array discussed previously. Finally, it enables GAP advertisements. A central device which receives one of our advertising packets may request to connect with our GATT server, at which point the two can exchange information via GATT.

```
// HCI packet handler
void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;
    if (packet_type != HCI_EVENT_PACKET) return;

    // Retrieve event type from HCI packet
    uint8_t event_type = hci_event_packet_get_type(packet);

    // Switch on event type . . .
    switch(event_type){
        // Setup GAP advertisement
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
            gap_local_bd_addr(local_addr);
            printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
            // setup advertisements
            uint16_t adv_int_min = 800;
            uint16_t adv_int_max = 800;
            uint8_t adv_type = 0;
            bd_addr_t null_addr;
            memset(null_addr, 0, 6);
            gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
            assert(adv_data_len <= 31); // ble limitation
            gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
            gap_advertisements_enable(1);

            break;
        // Disconnected from a client
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            break;
        // Ready to send ATT
        case ATT_EVENT_CAN_SEND_NOW:
            break;
        default:
            break;
    }
}
```

## Configuring BTstack

In addition to the runtime configurations discussed previously (setting up the GATT server, read/write callbacks, packet handlers, GAP advertisements, etc.), BTstack also requires some compile-time configuration. Most of these configurations live in the [`btstack_config.h`](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Bluetooth/GATT_Server/btstack_config.h) file, which uses a collection of macros to specify the system configuration, memory configuration, and enabled features. The contents of this configuration file for our particular project is copied below, in segments. I'd like to go through each set of configurations individually. Please note that the primary resource that I'll be using to discuss these configuration is the [BTstack documentation](https://bluekitchen-gmbh.com/btstack/#how_to/).

### BTstack features

The `ENABLE` peripherals turn on various BTstack features. This particular set of configureations enables support for BLE peripheral mode in both the HCI and Security Manager protocols, enables log_info messages (for debugging), enables log_error messages (also for debugging), and enables hexdumps over printf.

```
// BTstack features that can be enabled
#define ENABLE_LE_PERIPHERAL
#define ENABLE_LOG_INFO
#define ENABLE_LOG_ERROR
#define ENABLE_PRINTF_HEXDUMP
```

### Memory configuration directives

The first directive reserves 4 bytes before actual data for outgoing HCI packets. The size of ACL packets is limited to 259 bytes, which implies an L2CAP MTU of 255 bytes. Only a single HCI connection can be in place at any time. The alignment of ACL chunks is set to 4 bytes, which can be used to align HCI transport writes. The maximum number of items in the security manager lookup queue is set to 3, and the max number of whitelisted GAP LE devices is set to 16. The maximum number of devices in the LE device database is also set to 16.

```
// BTstack configuration. buffers, sizes, ...
#define HCI_OUTGOING_PRE_BUFFER_SIZE 4
#define HCI_ACL_PAYLOAD_SIZE (255 + 4)
#define HCI_ACL_CHUNK_SIZE_ALIGNMENT 4
#define MAX_NR_HCI_CONNECTIONS 1
#define MAX_NR_SM_LOOKUP_ENTRIES 3
#define MAX_NR_WHITELIST_ENTRIES 16
#define MAX_NR_LE_DEVICE_DB_ENTRIES 16
```

### HCI controller to host configuration

On the RP2040, we use HCI Controller to Host Flow Control, as enabled by the first directive below. In order for the HCI Transport to work properly, it must be able to buffer packets and events from controller to host. The directives below specify the maximum numbers and sizes of ACL and SCO packets that can be buffered by the HCI transport.

```
// Enable and configure HCI Controller to Host Flow Control to avoid cyw43 shared bus overrun
#define ENABLE_HCI_CONTROLLER_TO_HOST_FLOW_CONTROL
#define HCI_HOST_ACL_PACKET_LEN (255+4)
#define HCI_HOST_ACL_PACKET_NUM 3
#define HCI_HOST_SCO_PACKET_LEN 120
#define HCI_HOST_SCO_PACKET_NUM 3

// Limit number of ACL/SCO Buffer to use by stack to avoid cyw43 shared bus overrun
#define MAX_NR_CONTROLLER_ACL_BUFFERS 3
#define MAX_NR_CONTROLLER_SCO_PACKETS 3
```

### Nonvolatile memory directives

Bonding information gets stored in nonvolatile flash memory. The directive below specify that no more than 16 LE device database entries can be stored, and no more than the same number of Classic Link Keys (for Bluetooth Classic) can be stored.

```
// Link Key DB and LE Device DB using TLV on top of Flash Sector interface
#define NVM_NUM_DEVICE_DB_ENTRIES 16
#define NVM_NUM_LINK_KEYS 16
```

### Max size of ATT database

BTstack can be configured to use malloc to dynamically allocate memory for services, active connections, and remote devices. Because we're running on a microcontroller, we *don't* want to do this. Instead, we statically allocate memory at compile time with a maximum number of elements as specified by the directives in this file. The directive below specifies that the total size of the ATT database should not excede 512 bytes.

```
// We don't give btstack a malloc, so use a fixed-size ATT DB.
#define MAX_ATT_DB_SIZE 512
```

### System configs

Finally, we let BTstack know that the hardware on which it is running can provide a millisec timer, that it has access to `assert`, enables advanced encryption standard 128, and enables the micro ecc library ([link](https://github.com/kmackay/micro-ecc)) for ecc operations.

```
// BTstack HAL configuration
#define HAVE_EMBEDDED_TIME_MS
// map btstack_assert onto Pico SDK assert()
#define HAVE_ASSERT
// Some USB dongles take longer to respond to HCI reset (e.g. BCM20702A).
#define HCI_RESET_RESEND_TIMEOUT_MS 1000
#define ENABLE_SOFTWARE_AES128
#define ENABLE_MICRO_ECC_FOR_LE_SECURE_CONNECTIONS

#endif // MICROPY_INCLUDED_EXTMOD_BTSTACK_BTSTACK_CONFIG_H
```

## An application that uses this server

In order to demonstrate this GATT server, I have assembled a [simple application](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Bluetooth/GATT_Server/server_demo.c). This application renders the contents of the GATT database on a [VGA display](https://vanhunteradams.com/Pico/VGA/VGA.html), and implements a serial interface through which the user can update some of the characteristics in that database. A connecting GATT client (be it a phone running LightBlue, or another Pi Pico W) can issue read/write requests for each of these notifications, and toggle notifications on/off.

The example application includes three [Protothreads](https://people.ece.cornell.edu/land/courses/ece4760/RP2040/C_SDK_protothreads/index_Protothreads.html) and a timer interrupt running at 50kHz. [The timer interrupt](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/3490e766c48b0fc623680eefca87e267382233ca/Bluetooth/GATT_Server/server_demo.c#L116) implements a [Direct Digital Synthesis](https://vanhunteradams.com/DDS/DDS.html) sine-wave generator with a frequency that is specified by characteristic B in the GATT server. By changing the value of this characteristic, the GATT client can adjust the frequency of the sine wave that gets communicated to an SPI DAC and, ultimately, to a set of speakers.

[One of the three protothreads](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/3490e766c48b0fc623680eefca87e267382233ca/Bluetooth/GATT_Server/server_demo.c#L140) simply runs at 2Hz, incrementing the value of Characteristic A each time it runs and updating the value of the characteristic on the VGA display. If the client has enabled notifications, then it will see Characteristic A increment each time this thread runs.

[Another thread](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/3490e766c48b0fc623680eefca87e267382233ca/Bluetooth/GATT_Server/server_demo.c#L170) is [semaphore-signalled](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/3490e766c48b0fc623680eefca87e267382233ca/Bluetooth/GATT_Server/GATT_Service/service_implementation.h#L232) by our write callback function. Each time the GATT client updates a characteristic, this thread will be signalled to run. When it runs, it simply updates the values of all characteristics on the VGA display.

[The final thread](https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/3490e766c48b0fc623680eefca87e267382233ca/Bluetooth/GATT_Server/server_demo.c#L281) implements a serial user interface that allows for a user to modify the value of Characteristic C. If the GATT client has notifications enabled, then it will see this value change each time the user modifies it.

## Going farther

- [GATT Client webpage](https://vanhunteradams.com/Pico/BLE/GATT_Client.html)

## Resources

**Resources on Bluetooth and BTstack**:

1. [Bluetooth Low Energy: The Developer's Handbook, by Robin Heydon](https://www.amazon.com/Bluetooth-Low-Energy-Developers-Handbook/dp/013288836X)
2. [Getting Started with Bluetooth Low Energy, by Townsent, Cufí, Davidson and Davidson](https://www.amazon.com/Bluetooth-Low-Energy-Developers-Handbook/dp/013288836X)
3. [BTstack documentation](https://bluekitchen-gmbh.com/btstack/#quick_start/index.html)
4. [BTstack source code](https://github.com/bluekitchen/btstack)
5. [Raspberry Pi Pico W example code](https://github.com/raspberrypi/pico-examples/tree/master/pico_w/bt)

**Documentation on some of the other libraries/algorithms used in this project**:

1. [RP2040 VGA driver](https://vanhunteradams.com/Pico/VGA/VGA.html)
2. [Direct Digital Synthesis](https://vanhunteradams.com/DDS/DDS.html)
3. [Protothreads](https://people.ece.cornell.edu/land/courses/ece4760/RP2040/C_SDK_protothreads/index_Protothreads.html)