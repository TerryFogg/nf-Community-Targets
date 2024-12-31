# Displays

## ER-TFTM028-4 V4.0  



### RP2XXX connection

| GPIO | RP2XXX Function | LCD Connector Pin |           | Description                                                  |
| ---- | --------------- | ----------------- | --------- | ------------------------------------------------------------ |
| GP16 | SPI0_RX         | 25                | (D/C)     | Data/Command for 4 wire interface connection                 |
| GP17 | SPI0_CSn        | 23                | LDC_/CS   | LCD Chip select Low = enable                                 |
| GP18 | SPI0_SCK        | 24                | LCD_SCL   | Serial interface clock in 4-wire serial data interface       |
| GP19 | SPI0_TX         | 27                | LCD_SDI   | Serial Interface, data is applied on the rising edge of the SCL signal |
| GP20 | I2C0_SDA        | 31                | CTP_SDA   | Capacitive Touch Screen, serial data input/output            |
| GP21 | I2C0_SCL        | 30                | CTP_/CS   | Capacitive Touch Screen, Serial Clock Input (I2C)            |
| GP22 | Gpio            | 39                | CTP_INT   | Capacitive Touch Interrupt active low                        |
| GP26 | Gpio            | 29                | BL_ON/OFF | Backlight Low:OFF High:ON                                    |
| GP27 | Gpio            | 21                | /RESET_NC | Master Reset, active low.                                    |
| GND  |                 |                   |           |                                                              |
| 3.3V |                 |                   |           |                                                              |
|      |                 |                   |           |                                                              |



