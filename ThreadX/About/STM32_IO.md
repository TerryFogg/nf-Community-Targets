# STM32 IO









## GPIO



## Testing input with pullup resistor

Sure, I can help you with that. Here’s a simple way to test if a microcontroller unit (MCU) pin is working using an input pull-up resistor. This method uses a button to change the state of the pin.

Here’s a simple circuit diagram:

```
MCU Pin <-----> Button <-----> Vcc
   |
   |
Pull-up Resistor
   |
   |
  GND
```

And here’s how you can implement it:

1. **Connect a button to the MCU pin**: One end of the button should be connected to the MCU pin you want to test, and the other end should be connected to the power supply (Vcc).
2. **Connect a pull-up resistor**: Connect a pull-up resistor (typically 10kΩ) between the MCU pin and the ground (GND). This ensures that the pin reads a `HIGH` signal when the button is not pressed.
3. **Write a simple program**: Write a simple program that reads the state of the pin and outputs the result. Here’s a simple example in Arduino C:

```c
const int pin = 2; // Change this to your pin number

void setup() {
  pinMode(pin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int buttonState = digitalRead(pin);
  Serial.println(buttonState);
  delay(1000);
}
```

This program will print `1` when the button is not pressed and `0` when the button is pressed. If you see these results, it means your MCU pin is working correctly with the input pull-up resistor.

Remember to replace `2` with the number of the pin you’re testing. Also, ensure your MCU supports internal pull-up resistors. If not, you’ll need to add an external one.

Please note that this is a simple test and might not catch all possible issues with the pin. If you suspect there’s a hardware issue with the pin, you might need to use more advanced debugging tools or techniques.

I hope this helps! Let me know if you have any other questions. 😊

​              







## GPIO

***GPIO*** stands for ***general purpose input/output***. It is a type of pin found on an integrated circuit that does not have a specific function. While most pins have a dedicated purpose, such as sending a signal to a certain component, the function of a GPIO pin is customizable and can be controlled by the software.

- **Pin Mode :**

   

  Each port bit of the general-purpose I/O (GPIO) ports can be individually configured by software in several modes:

  - input or output
  - analog
  - alternate function (AF).

- **Pin characteristics :**

  - ***Input*** : no pull-up and no pull-down or pull-up or pull-down
  - ***Output*** : push-pull or open-drain with pull-up or pull-down capability
  - ***Alternate function*** : push-pull or open-drain with pull-up or pull-down capability.

[![GPIO Functional description graph.png](assets/GPIO_Functional_description_graph.png)

#### 1.1. GPIO (pin) output-speed configuration[↑](https://wiki.st.com/stm32mcu/wiki/Getting_started_with_GPIO#)

- Change the rising and falling edge when the pin state changes from high to low or low to high.
- A higher GPIO speed increases the EMI noise from STM32 and increases the STM32 consumption.
- It is good to adapt the GPIO speed to the peripheral speed. For example, **low** speed is optimal for toggling GPIO at 1 Hz, while using SPI at 45 MHz requires ***very high*** speed setting.

[![GPIO Edge.png](assets/GPIO_Edge.png)



All GPIOs are able to drive 5 V and 3.3 V in input mode, but they are only able to generate 3.3V in output push-pull mode































# Low-power modes

By default, the microcontroller is in Run mode after a system or power reset. Several low-power modes are available to save power when the CPU does not need to be kept running, for example when waiting for an external event. The ultra-low-power STM32L476xx supports ***six*** low-power modes to achieve the best compromise between low-power consumption, short startup time, available peripherals and available wake-up sources.

- Sleep mode
- Low-power run mode
- Low-power sleep mode
- Stop 0, Stop1, Stop2 modes
- Standby mode
- Shutdown mode

### 1.1. Voltage regulators[↑](https://wiki.st.com/stm32mcu/wiki/Getting_started_with_PWR#)

Two embedded linear voltage regulators supply most of the digital circuitries: the main regulator (MR) and the low-power regulator (LPR).

- The MR is used in the Run and Sleep modes and in the Stop 0 mode.
- The LPR is used in Low-power run, Low-power sleep, Stop 1 and Stop 2 modes. It is also used to supply the 32 Kbyte SRAM2 in Standby with SRAM2 retention.
- Both regulators are in power-down in Standby and Shutdown modes: the regulator output is in high impedance, and the kernel circuitry is powered down thus inducing zero consumption.

The main regulator has two possible programmable voltage ranges:

- Range 1 with the CPU running at up to 80 MHz.
- Range 2 with a maximum CPU frequency of 26 MHz. All peripheral clocks are also limited to 26 MHz.

[![PWR voltage regulators.png](assets/500px-PWR_voltage_regulators.png)



The Standby mode is used to achieve the lowest power consumption with brown-out reset. The internal regulator is switched off so that the VCORE domain is powered off. The PLL, the MSI RC, the HSI16 RC and the HSE crystal oscillators are also switched off.
RTC can remain active (Standby mode with RTC, Standby mode without RTC).
Brown-out reset (BOR) always remains active in Standby mode.
The state of each I/O during standby mode can be selected by software: I/O with internal pull-up, internal pull-down or floating.

The system can be woken up from standby mode using a SYS_WKUP pin, an [RTC](https://wiki.st.com/stm32mcu/wiki/Getting_started_with_RTC) event (alarm or timer), [IWDG](https://wiki.st.com/stm32mcu/wiki/Getting_started_with_WDOG), or an external reset in NRST pin.
After waking up from Standby mode, program execution restarts in the same way as after a Reset (boot pin sampling, option bytes loading, reset vector is fetched, etc.)

[![PWR Standby.png](assets/700px-PWR_Standby.png)







# APPENDIX 1 - TESTING GPIO

## Testing input with without pull up or pull down resistor

Sure, I can help you with that. Here’s a simple way to test if a microcontroller unit (MCU) pin is working without using a pull-up or pull-down resistor. This method uses a button to change the state of the pin.

Here’s a simple circuit diagram:

```
MCU Pin <-----> Button <-----> Vcc
   |
   |
  GND
```

And here’s how you can implement it:

1. **Connect a button to the MCU pin**: One end of the button should be connected to the MCU pin you want to test, and the other end should be connected to the power supply (Vcc).
2. **Connect the MCU pin to the ground (GND)**: When the button is not pressed, the MCU pin is connected to the ground (GND), so it reads a `LOW` signal.
3. **Write a simple program**: Write a simple program that reads the state of the pin and outputs the result. Here’s a simple example in Arduino C:

```c
const int pin = 2; // Change this to your pin number

void setup() {
  pinMode(pin, INPUT);
  Serial.begin(9600);
}

void loop() {
  int buttonState = digitalRead(pin);
  Serial.println(buttonState);
  delay(1000);
}
```

This program will print `0` when the button is not pressed and `1` when the button is pressed. If you see these results, it means your MCU pin is working correctly.

Remember to replace `2` with the number of the pin you’re testing.

## Testing input with pull up resistor

This method uses a button to change the state of the pin.

Circuit diagram:

```
MCU Pin <-----> Button <-----> Vcc
   |
   |
Pull-up Resistor
   |
   |
  GND
```

1. **Connect a button to the MCU pin**: One end of the button should be connected to the MCU pin you want to test, and the other end should be connected to the power supply (Vcc).
2. **Connect a pull-up resistor**: Connect a pull-up resistor (typically 10kΩ) between the MCU pin and the ground (GND). This ensures that the pin reads a `HIGH` signal when the button is not pressed.
3. **Write a simple program**: Write a simple program that reads the state of the pin and outputs the result. Here’s a simple example in Arduino C:

```c
const int pin = 2; // Change this to your pin number

void setup() {
  pinMode(pin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int buttonState = digitalRead(pin);
  Serial.println(buttonState);
  delay(1000);
}
```

This program will print `1` when the button is not pressed and `0` when the button is pressed. If you see these results, it means your MCU pin is working correctly with the input pull-up resistor.

Remember to replace `2` with the number of the pin you’re testing. Also, ensure your MCU supports internal pull-up resistors. If not, you’ll need to add an external one.

​           

## Testing input with pull down resistor

Sure, I can help you with that. Here’s a simple way to test if a microcontroller unit (MCU) pin is working using an input pull-down resistor. This method uses a button to change the state of the pin.

Here’s a simple circuit diagram:

```
MCU Pin <-----> Button <-----> GND
   |
   |
Pull-down Resistor
   |
   |
  Vcc
```

And here’s how you can implement it:

1. **Connect a button to the MCU pin**: One end of the button should be connected to the MCU pin you want to test, and the other end should be connected to the ground (GND).
2. **Connect a pull-down resistor**: Connect a pull-down resistor (typically 10kΩ) between the MCU pin and the power supply (Vcc). This ensures that the pin reads a `LOW` signal when the button is not pressed.
3. **Write a simple program**: Write a simple program that reads the state of the pin and outputs the result. Here’s a simple example in Arduino C:

```c
const int pin = 2; // Change this to your pin number

void setup() {
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW); // Enable pull-down resistor
  Serial.begin(9600);
}

void loop() {
  int buttonState = digitalRead(pin);
  Serial.println(buttonState);
  delay(1000);
}
```

This program will print `0` when the button is not pressed and `1` when the button is pressed. If you see these results, it means your MCU pin is working correctly with the input pull-down resistor.

Remember to replace `2` with the number of the pin you’re testing. Also, ensure your MCU supports internal pull-down resistors. If not, you’ll need to add an external one.



## Testing  Output with open drain

Sure, I can help you with that. Here’s a simple way to test if a microcontroller unit (MCU) pin is working using an output open-drain configuration. This method uses an LED to indicate the state of the pin.

Here’s a simple circuit diagram:

```
MCU Pin (Open-Drain) <-----> Resistor <-----> LED <-----> Vcc
```

And here’s how you can implement it:

1. **Connect an LED to the MCU pin**: One end of the LED should be connected to the MCU pin you want to test through a resistor (typically 330Ω to 1kΩ depending on the LED and Vcc), and the other end should be connected to the power supply (Vcc).
2. **Configure the MCU pin as open-drain**: In the MCU’s software, configure the pin as an output and set it to open-drain mode. This means the pin can either provide a path to ground (when it’s outputting a `LOW` signal) or be disconnected (when it’s outputting a `HIGH` signal).
3. **Write a simple program**: Write a simple program that toggles the state of the pin and observe the LED. Here’s a simple example in Arduino C:

```c
const int pin = 2; // Change this to your pin number

void setup() {
  pinMode(pin, OUTPUT);
}

void loop() {
  digitalWrite(pin, LOW); // LED on
  delay(1000);
  digitalWrite(pin, HIGH); // LED off
  delay(1000);
}
```

This program will turn the LED on and off every second. If you see the LED blinking, it means your MCU pin is working correctly in open-drain mode.

Remember to replace `2` with the number of the pin you’re testing.

Please note that this is a simple test and might not catch all possible issues with the pin. If you suspect there’s a hardware issue with the pin, you might need to use more advanced debugging tools or techniques.

I hope this helps! Let me know if you have any other questions. 😊

## Testing  Output with open drain push pull

Sure, I can help you with that. Here’s a simple way to test if a microcontroller unit (MCU) pin is working using an output push-pull configuration. This method uses an LED to indicate the state of the pin.

Here’s a simple circuit diagram:

```
MCU Pin (Push-Pull) <-----> Resistor <-----> LED <-----> GND
```

And here’s how you can implement it:

1. **Connect an LED to the MCU pin**: One end of the LED should be connected to the MCU pin you want to test through a resistor (typically 330Ω to 1kΩ depending on the LED and Vcc), and the other end should be connected to the ground (GND).
2. **Configure the MCU pin as push-pull**: In the MCU’s software, configure the pin as an output and set it to push-pull mode. This means the pin can either provide a path to Vcc (when it’s outputting a `HIGH` signal) or a path to ground (when it’s outputting a `LOW` signal).
3. **Write a simple program**: Write a simple program that toggles the state of the pin and observe the LED. Here’s a simple example in Arduino C:

```c
const int pin = 2; // Change this to your pin number

void setup() {
  pinMode(pin, OUTPUT);
}

void loop() {
  digitalWrite(pin, HIGH); // LED on
  delay(1000);
  digitalWrite(pin, LOW); // LED off
  delay(1000);
}
```

This program will turn the LED on and off every second. If you see the LED blinking, it means your MCU pin is working correctly in push-pull mode.

