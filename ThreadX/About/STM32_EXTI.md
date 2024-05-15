# Extended interrupt and event controller (EXTI)



There are two types of interrupts configurable and direct.. The *Nested Vectored Interrupt Controller*, or NVIC, supports many more than are usually implemented on the STM32 cores. They are not all freely assignable and have a specific purpose.  Some are reserved for processor, memory or data bus events and peripherals.

NOTE: There is a limitation of 16 external interrupts on the GPIO lines with one 1 interrupt per line number. E.G. You cannot have external interrupts on PA0 and PB0 at the same time.

The remaining EXTI lines are connected to peripherals like RTC, Ethernet, and USB for features like Wakeup and Alarm events.

On the STM32H7 series, the EXTI peripheral has 7 interrupts associated with it  which gives 16 lines on the EXTI peripheral which can be used with GPIO pins.

EXIT 0 ...4 have individual interrupts, while 5 through 9 and 10 through 15 need some extra work to determine the source of the interrupt.

- EXTI line 0
- EXTI line 1
- EXTI line 2
- EXTI line 3
- EXTI line 4
- EXTI line 5 through 9
- EXTI line 10 through 15

 The lines themselves are connected using muxes to GPIO pins as in the following diagram:

[![img](https://hackaday.com/wp-content/uploads/2021/03/stm32f4_exti_gpio_mapping-themed.png?w=667)](https://hackaday.com/wp-content/uploads/2021/03/stm32f4_exti_gpio_mapping-themed.png)STM32F4 EXTI to GPIO peripheral mapping. (RM0090, 12.2.5)













## Setting Up External Interrupts

The steps involved in setting up an external interrupt on a GPIO pin can be summarized as follows:

- Enable `SYSCFG` (except on F1).
- Enable `EXTI` in `RCC` (except on F1).
- Set `EXTI_IMR` register for the pin to enable the line as an interrupt.
- Set `EXTI_FTSR` & `EXTI_RTSR` registers for the pin for trigger on falling and/or rising edge.
- Set `NVIC` priority on interrupt.
- Enable interrupt in the `NVIC` register.

For example an STM32F4 family MCU, we would enable the SYSCFG (System Configuration controller) peripheral first.







The register SYSCFG_EXTICR1 shows that for a particular EXTIx, 4 bits are used and can only select 1 of the 16 ports lines

SYSCFG external interrupt configuration register 1 (SYSCFG_EXTICR1)

Bits 15:0 EXTIx[3:0]: EXTI x configuration (x = 0 to 3)
These bits are written by software to select the source input for the EXTI input for external interrupt / event detection.
0000: PA[x] pin
0001: PB[x] pin
0010: PC[x] pin
0011: PD[x] pin
0100: PE[x] pin
0101: PF[x] pin
0110: PG[x] pin
0111: PH[x] pin
1000: PI[x] pin
1001: PJ[x] pin
1010: PK[x] pin
Other configurations: reserved



| EXTI Line | Selected Port |
| --------- | ------------- |
| EXTI1     | PA0,          |
|           |               |
|           |               |



The Extended Interrupt and event controller (EXTI) manages wakeup through configurable
and direct event inputs. It provides wakeup requests to the power controller (PWR), and
generates interrupt requests to the CPU NVIC and to the SRD domain DMAMUX2, and
events to the CPU event input.
The EXTI wakeup requests allow the system to be woken up from Stop mode, and the CPU
to be woken up from CStop mode.
In addition, both the interrupt request and event request generation can be used in Run
mode.

## EXTI main features

Signals from I/O's or peripherals able to generate a pulses with the following attributes.

### Configurable event

- Selectable active trigger edge
- Interrupt pending status register bit
- Individual Interrupt and event generation mask
- Software trigger capability

### Direct events

- Fixed rising edge active trigger
- No interrupt pending status register bit in the EXTI (the interrupt pending status is
  provided by the peripheral generating the event)
- No software trigger capability

Both types of events can wakeup the SRD power Domain and can generate wakeup events when the system is in STOP mode CPU is in CSTOP mode

Different logic implementations can be used, depending on the EXTI event input type and wakeup target(s). The applicable features are controlled from register bits:

- Active trigger edge enable 
  - EXTI rising trigger selection register (EXTI_RTSR1), (EXTI_RTSR2),(EXTI_RTSR3)
  - EXTI falling trigger selection register (EXTI_FTSR1), (EXTI_FTSR2), (EXTI_FTSR3) 
- Software trigger
  - EXTI software interrupt event register (EXTI_SWIER1),  (EXTI_SWIER2),  (EXTI_SWIER3)

- CPU Interrupt enable 
  - EXTI interrupt mask register (EXTI_CPUIMR1), (EXTI_CPUIMR2), (EXTI_CPUIMR3) 
  -  CPU Event enable, (EXTI_CPUEMR1), (EXTI_CPUEMR2), (EXTI_CPUEMR3) 
- SRD domain wakeup pending, 
  - EXTI SRD pending mask register (EXTI_SRDPMR1),(EXTI_SRDPMR2),(EXTI_SRDPMR3)

## EXTI event input mapping

| EVent Input      | Souce             | EVent input TYpe    |
| ---------------- | ----------------- | ------------------- |
| *****<u>0-15</u> | <u>EXTI[15:0]</u> | <u>Configurable</u> |
| 16               | PVD               | Configurable        |
| 17-19            | RTC               | Configurable        |
| 20-21            | COMPARE           | Configurable        |
| 22-25            | I2C               | Direct              |
| 26-33            | USART             | Direct              |
| 34-35            | LPUART            | Direct              |
| 36-41            | SPI               | Direct              |
| ...              |                   |                     |
| ...              |                   |                     |

*****F**or the sixteen GPIO event inputs, the associated GPIO pin has to be selected in the SYSCFG_EXTICRn register. The same pin from each GPIO maps to the corresponding EXTI event input.All EXTI event inputs are OR-ed together and connected to the CPU event input.**

The direct event inputs are enabled in the respective peripheral that generating the event. The configurable events are enabled by enabling at least one of the trigger edges.

### EXTI CPU interrupt procedure

| Order | Description                                                  |
| ----- | ------------------------------------------------------------ |
| 1     | Unmask the event input interrupt by setting the corresponding mask bits in the EXTI_CPUIMR register. |
| 2     | Enable the event input by setting either one or both the corresponding trigger edge enable bits in EXTI_RTSR and EXTI_FTSR registers |
| 3     | Enable the associated interrupt source in the CPU NVIC or use the SEVONPEND, so that an interrupt coming from the CPU interrupt signal can be detected by the CPU after a WFI/WFE instruction |

### EXTI CPU event procedure

| Order | Description                                                  |
| ----- | ------------------------------------------------------------ |
| 1     | Unmask the event input by setting the corresponding mask bits of the EXTI_CPUEMR register |
| 2     | Enable the event input by setting either one or both the corresponding trigger edge enable bits in EXTI_RTSR and EXTI_FTSR registers |
| 3     | The CPU event signal is detected by the CPU after a WFE instruction. For configurable event inputs, there is no EXTI pending bit to clear. |

### EXTI SRD domain wakeup for Autonomous run mode procedure

| Order | Description                                                  |
| ----- | ------------------------------------------------------------ |
| 1     | Mask the event input for waking up the CPU by clearing both the corresponding mask bits in the EXTI_CPUIMR and/or EXTI_CPUEMR registers. |
| 2     | For configurable event inputs, enable the event input by setting either one or both the corresponding trigger edge enable bits in EXTI_RTSR and EXTI_FTSR registers |
| 3     | Direct events automatically generate an SRD domain wakeup    |
| 4     | Select the SRD domain wakeup mechanism in EXTI_SRDPMR.<br/>– When SRD domain wakeup without pending (EXTI_PMR = 0) is selected, the Wakeup is automatically cleared following the clearing of the event input.<br/>– When SRD domain wakeup with pending (EXTI_PMR = 1) is selected, the Wakeup needs to be cleared by a selected SRD domain pend clear source.<br/>A pending SRD domain wakeup signal can also be cleared by firmware by<br/>clearing the associated EXTI_SRDPMR register bit. |
| 5     | An SRD domain interrupt is generated after the SRD domain wakeup:<br/>– Configurable event inputs generate a pulse on SRD domain interrupt.<br/>– Direct event inputs activate the SRD domain interrupt until the event input is<br/>cleared in the peripheral. |

### EXTI software interrupt/event trigger procedure

Any of the configurable event inputs can be triggered from the software interrupt/event
register (the associated CPU interrupt and/or CPU event shall be enabled by their
respective procedure). Follow the steps below

| Order | Description                                                  |
| ----- | ------------------------------------------------------------ |
| 1     | Enable the event input by setting at least one of the corresponding edge trigger bits in the EXTI_RTSR and/or EXTI_FTSR registers. |
| 2     | Unmask the software interrupt/event trigger by setting at least one of the corresponding mask bits in the EXTI_CPUIMR and/or EXTI_CPUEMR registers. |
| 3     | Trigger the software interrupt/event by writing 1 to the corresponding bit in the EXTI_SWIER register |
| 4     | The event input can be disabled by clearing the EXTI_RTSR and EXTI_FTSR register bits.<br/> |







## S

```
RCC->APB2ENR |= (1 << RCC_APB2ENR_SYSCFGCOMPEN_Pos);
```

The `SYSCFG` peripheral manages the external interrupt lines to the `GPIO` peripherals, i.e. the mapping between a `GPIO` peripheral and the `EXTI` line. Say if we want to use PB0 and PB4 as the input pins for our encoder’s A & B signals, we would have to set the lines in question to the appropriate `GPIO` peripheral. For port B, this would be done in `SYSCFG_EXTICR1` and `SYSCFG_EXTICR2`, as each 32-bit register covers a total of four `EXTI` lines:

[![img](https://hackaday.com/wp-content/uploads/2021/03/stm32f4_syscfg_exticr1-themed.png?w=800)](https://hackaday.com/wp-content/uploads/2021/03/stm32f4_syscfg_exticr1-themed.png)SYSCFG_EXTICR1 register for STM32F4 MCUs. (RM0090, 9.2.3)

While somewhat confusing at first glance, setting these registers is relatively straightforward. E.g. for PB0:

```
SYSCFG->EXTICR[0] |= (((``uint32_t``) 1) << 4);
```

As each line’s section in the register is four bits, we left-shift the appropriate port value to reach the required position. For PB4 we do the same thing, but in the second register, and without left shift, as that register starts with line 4.

At this point we’re almost ready to configure the EXTI & NVIC registers. First, we need to enable the GPIO peripheral we intend to use, and set the pins to input mode in pull-up configuration, as here for PB0:

```
RCC->AHB1ENR |= (1 << RCC_AHBENR_GPIOBEN_Pos);``GPIOB->MODER &= ~(0x3);``GPIOB->PUPDR &= ~(0x3);``GPIOB-&>PUPDR |= (0x1);
```

Say we want to set PB0 to trigger on a falling edge, we have to first enable Line 0, then configure the trigger registers:

```
pin = 0;``EXTI->IMR |= (1 << pin);``EXTI->RTSR &= ~(1 << pin);``EXTI->FTSR |= (1 << pin);
```

All of these registers are quite straight-forward, with each line having its own bit.

With that complete, we merely have to enable the interrupts now, and ensure our interrupt handlers are in place. First the NVIC, which is done most easily via the standard CMSIS functions, as here for PB0, with interrupt priority level 0 (the highest):

```
NVIC_SetPriority(EXTI0_IRQn, 0);``NVIC_EnableIRQ(EXTI0_IRQn);
```

