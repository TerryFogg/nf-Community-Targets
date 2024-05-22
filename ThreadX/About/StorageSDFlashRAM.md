

# Storage, Flash, SD, Ram disks

## Flash

In both NOR and NAND Flash, the memory is organized into erase blocks. This architecture helps maintain lower cost while maintaining performance. For example, a smaller block size enables faster erase cycles. The downside of smaller blocks, however, is an increase in die area and memory cost. Because of its lower cost per bit, NAND Flash can more cost-effectively support smaller erase blocks compared to NOR Flash. The typical block size available today ranges from 8KB to 32KB for NAND Flash and 64KB to 256KB for NOR Flash.

Erase operations in NAND Flash are straightforward while in NOR Flash, each byte needs to be written with ‘0’ before it can be erased. This makes the erase operation for NOR Flash much slower than for NAND Flash. 

The NOR Flash architecture provides enough address lines to map the entire memory range. This gives the advantage of random access and short read times, which makes it ideal for code execution. Another advantage is 100% known good bits for the life of the part. Disadvantages include larger cell size resulting in a higher cost per bit and slower write and erase speeds.

NAND Flash, in contrast, has a much smaller cell size and much higher write and erase speeds compared to NOR Flash. Disadvantages include the slower read speed and an I/O mapped type or indirect interface, which is more complicated and does not allow random access. It is important to note that code execution from NAND Flash is achieved by shadowing the contents to a RAM, which is different than code execution directly from NOR Flash. Another major disadvantage is the presence of bad blocks. NAND Flash typically have 98% good bits when shipped with additional bit failure over the life of the part, thus requiring the need for error correcting code (ECC) functionality within the device.

## RAM

RAM disks are stored in volatile memory (RAM), which allows for extremely fast read and write operations compared to non-volatile storage like Flash memory.

- RAM disks are ideal for storing temporary data during program execution (e.g., intermediate results, buffers, caches).
- Since RAM is volatile, the data is lost when the system restarts, making it suitable for transient storage.

## Secure Digital (SD) Cards

SD cards, including microSD cards, use NAND flash memory. These cards rely on one or two small NAND flash memory chips, similar to those found in USB memory sticks and SSDs. Additionally, they have a tiny processor to manage data flow and instructions

There are many non-volatile flash memory card standards:

Although not the fastest, by far, the most used and available type of memory card adheres to the Secure Digital standard.

There are several formats for the SD card form factor:

- standard (original): 32.0 x 24.0 x 2.1 mm
- miniSD: 21.5 x 20.0 x 1.4 mm
- microSD: 15.0 x 11.0 x 1.0 mm

Currently, there are at least [four classes of SD cards](https://www.sdcard.org/developers/sd-standard-overview/):

```shell
+------+---------------------+-------------+
|      | Name                | Capacity    |
+------+---------------------+-------------+
| SD   | standard            | <= 2GB      |
| SDHC | [H]igh capacity     | 2GB - 32GB  |
| SDXC | e[X]tended capacity | 32GB - 2TB  |
| SDUC | [U]ltra capacity    | 2TB - 128TB |
+------+---------------------+-------------+
```

Many speed class scales exist for SD cards, new speeds are added as technology progresses.

```shell
+---------+-----------------------------------------------+
|         |                     Speed                     |
|         |                     Scale                     |
+---------+-------+-----------+-------------+-------------+
|  Speed  | Speed | UHS Speed | Video Speed | SD Express  |
|         | Class |   Class   |    Class    | Speed Class |
+---------+-------+-----------+-------------+-------------+
| 600MB/s |       |           |             | E600        |
| 450MB/s |       |           |             | E450        |
| 300MB/s |       |           |             | E300        |
| 150MB/s |       |           |             | E150        |
| 90MB/s  |       |           | V90         |             |
| 60MB/s  |       |           | V60         |             |
| 30MB/s  |       | 3         | V30         |             |
| 10MB/s  | 10    | 1         | V10         |             |
| 6MB/s   | 6     |           | V6          |             |
| 4MB/s   | 4     |           |             |             |
| 2MB/s   | 2     |           |             |             |
+---------+-------+-----------+-------------+-------------+
```

 FAT32 was the default for SD and SDHC cards until the introduction of SDXC, when [exFAT](https://www.baeldung.com/linux/filesystem-limits#exfat) became the new default:

- SD: FAT16/FAT32
- SDHC: FAT32
- SDXC: exFAT
- SDUC: exFAT

