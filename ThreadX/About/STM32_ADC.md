# Getting Started with STM32 - Working with ADC and DMA



One of the most common peripherals on many modern microcontrollers is the analog-to-digital converter (ADC). These embedded devices read an analog voltage (usually somewhere between 0 V and the given reference voltage) and report it as a binary value. The exact implementation of the ADC can change among STM32 chips, as some use the [successive-approximation register (SAR)](https://en.wikipedia.org/wiki/Successive_approximation_ADC) technique while others rely on [sigma-delta modulation](https://en.wikipedia.org/wiki/Delta-sigma_modulation) for more resolution (but lower speeds). Make sure you check your datasheet to find out which ADC is in your particular chip!

In this tutorial, we will show how to use the ADC on the STM32L476 with STM32CubeIDE and HAL. We’ll also introduce the direct memory access (DMA) controller to demonstrate how you might handle moving (relatively) large amounts of data in your microcontroller.

**Introducing the DMA**

If you are working with large amounts of data, whether from the ADC or trying to pipe the contents of memory over the UART, you might find that your processor quickly gets bogged down. If every byte has to pass through the CPU, you’ll quickly run out of cycles to do useful things! That’s where the separate direct memory access controller comes in--it can help transfer data without any code running.

We might normally think about memory transfer in a microcontroller in this (overly simplified) fashion:

![DMA diagram](https://www.digikey.com.au/maker-media/e895fdfc-eaf2-4602-be05-823a84410b62)

We have a number of peripherals controlled by the central processing unit (CPU). Each time we want to send data from one peripheral to the other, the CPU must read data from that peripheral’s data register (or memory) and send it to another peripheral (or memory). If we start working with large amounts of data, we can quickly overwhelm the CPU if it has to touch every byte of data.

Luckily for us, many modern microcontrollers (and microprocessors) include a DMA controller, which is a separate peripheral that can be configured to automatically send data from one peripheral to another (including memory). There is a small amount of overhead involved in setting up the DMA, but once it’s done, the CPU can work on other things while the DMA moves data.

For example, let’s say that we wanted to collect data from the ADC and store it to a large buffer in memory. We can configure to the DMA to do just that:

![DMA diagram](https://www.digikey.com.au/maker-media/d5573030-464f-4801-8de7-75c126d7f9a2)

Because of the overhead involved in setting up the DMA, using it is usually only worthwhile if you need to move long, contiguous streams of data. If you need to take only one ADC reading, setting up the DMA controller is likely not worth the extra CPU cycles.

**Required Components**

You will need the following components to complete this tutorial: [https://www.digikey.com/short/pfr0r8](https://www.digikey.com.au/short/pfr0r8)

Note that any Nucleo board may be used, but steps are shown for the Nucleo-L476RG.

**Hardware Hookup**

For the few demonstrations that follow, we will need a Nucleo board (I’m using a Nucleo-L476RG) and a potentiometer connected to A0:

![Nucleo connections Fritzing](https://www.digikey.com.au/maker-media/2d9f1e45-e470-4844-b148-fea1294dca76)

**Demo 1: Single ADC Reading**

Getting the STM32 to take 1 ADC reading is relatively straightforward. Start a new project in STM32CubeIDE with C. Change **PA10** (which is connected to header pin D2) to **GPIO_Output**. Change **PA0** to **ADC1_IN5**. Under *Categories*, go to *Analog*, and select **ADC1**. Change *IN5* to **IN5 Single-ended**, which allows us to use PA0 as an ADC pin. The ADC configuration settings can stay at default.

The [mbed Nucleo-L476RG page](https://os.mbed.com/platforms/ST-Nucleo-L476RG/) has a great pinout diagram, if you need help finding which pin goes where on the Nucleo board.

![Configure Nucleo with STM32CubeIDE](https://www.digikey.com.au/maker-media/30a41a9d-9434-4217-a4d2-f025590f7c29)

Open *main.c* and change the following (note that I’ve left out the bottom portion of the auto-generated Cube functions):

 

Copy Code

```
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint16_t raw;
  char msg[10];
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    // Test: Set GPIO pin high
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

    // Get ADC value
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    raw = HAL_ADC_GetValue(&hadc1);

    // Test: Set GPIO pin low
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

    // Convert to string and print
    sprintf(msg, "%hu\r\n", raw);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    // Pretend we have to do something else for a while
    HAL_Delay(1);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

// Auto-generated HAL code continues
// ...
```

 

 

Build and debug the project. Run the code and bring up a serial terminal. When you connect the terminal to your Nucleo board, you should see a string of numbers showing the raw ADC values. 0 means 0 V and 4095 means 3.3 V (or whatever you might have VREF set to). Try turning the potentiometer knob to change the values.

![Testing ADC on STM32](https://www.digikey.com.au/maker-media/f9210753-823d-43c2-a6cf-06f3f35bcc52)

You can connect an oscilloscope probe to D2 to measure the time it takes to set up the ADC and perform one conversion, which we see is about 9.6 μs.

![Timing ADC on STM32](https://www.digikey.com.au/maker-media/812bd6c0-8d7d-402d-9249-de87897ad3eb)

**Demo 2: DMA with UART**

If we want to take multiple ADC readings to fill a buffer without CPU intervention, then we will need to rely on DMA. One of the easiest ways to see DMA in action is to use it in conjunction with the UART. So, we’ll create a rather large buffer filled with arbitrary text, and we’ll tell the DMA to send that data, one byte at a time, to the UART peripheral. The UART will then send out the data to our serial terminal program.

To do this, we’ll first configure the UART to output data. We’ll create our message as a long string in memory, and then, we’ll configure our DMA to read that message, one byte at a time, to the UART transmitter.

![Configure DMA on STM32](https://www.digikey.com.au/maker-media/1cf98761-6838-42a6-954e-3528dda5f463)

You can use the same circuit from the first demo, but we won’t use the potentiometer. 

Create a new STM32 project, and under *System Core*, select **DMA**. Add a new DMA request in DMA1 (the STM32L476 has 2 DMA peripherals). Change the request to **USART2_TX**, and you can leave the rest of the settings at their defaults.

![Configure DMA in STM32 CubeMX](https://www.digikey.com.au/maker-media/a9381209-f300-4acb-8801-41e4af45a601)

Open main.c, and copy in the following code. Note that I’ve left out some of the auto-generated Cube/HAL functions for brevity. However, you will still need to define the DMATransferComplete() function as shown. This function acts as a callback that we assign in main.

 

Copy Code

```
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void DMATransferComplete(DMA_HandleTypeDef *hdma);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  char msg[] =  "Long boat holystone pirate log driver hulk nipperkin cog. " \
                "Buccaneer me lass poop deck spyglass maroon jib spike. Come" \
                "about maroon skysail Corsair bilge water Arr long clothes " \
                "transom.\r\n";
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_DMA_RegisterCallback(&hdma_usart2_tx, HAL_DMA_XFER_CPLT_CB_ID,
                            &DMATransferComplete);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    huart2.Instance->CR3 |= USART_CR3_DMAT;
    HAL_DMA_Start_IT(&hdma_usart2_tx, (uint32_t)msg,
                    (uint32_t)&huart2.Instance->TDR, strlen(msg));

    HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

// Auto-generated HAL code
// ...

/* USER CODE BEGIN 4 */
void DMATransferComplete(DMA_HandleTypeDef *hdma) {

  // Disable UART DMA mode
  huart2.Instance->CR3 &= ~USART_CR3_DMAT;

  // Toggle LD2
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}
/* USER CODE END 4 */

// Rest of main.c
// ...
```

 

 

Build and debug. Open a serial terminal, and you should see your message being printed every second.

![Testing UART with DMA](https://www.digikey.com.au/maker-media/8e574da4-f7af-455a-a6a5-84e34cbc4797)

You should also see the LED on LD2 flashing every second. This should tell you that the callback is getting called as part of the interrupt service routine whenever the DMA is done transferring all the data from the memory buffer.

**Demo 3: DMA with ADC**

The DMA is a great tool to use with the ADC when you want to transfer lots of samples to memory continuously. It can be used for audio sampling, a custom oscilloscope, etc. The STM32 HAL makes it a little easier to use, as there’s some built-in functions that control the DMA with the ADC, specifically. For this reason, I wanted to show how to set up the DMA manually in the previous example.

Once again, use the circuit from Demo 1, as we’ll need the potentiometer.

Feel free to start a new project in STM32CubeIDE again, if you wish (or use the one from Demo 1). You’ll need *PA0* assigned to **ADC1_IN5**. We’ll be using LD2 (PA5) to time the transfer (instead of PA10) for this example.

Under *Analog*, click **ADC1**, and change *IN5* to **IN5 Single-ended**. In the *Parameter Settings*, change *Continuous Conversion Mode* to **Enabled**.

We can’t set the ADC pin to DMA mode yet, as we need to first assign the DMA request. So, under *System Core*, click on **DMA**. Under *DMA1*, add a new request, and change it to **ADC1**. Change *Mode* to **Circular**, which allows the DMA to wrap around to the top of our buffer when it’s done filling it up (creating a circular buffer). Leave the rest of the settings alone.

![Setting up DMA with ADC in STM32 CubeMX](https://www.digikey.com.au/maker-media/aa86a50c-982e-4c3c-a1cb-4aa65abf0936)

Go back to **Analog > ADC1**. In the settings, change *DMA Continuous Requests* to **Enabled**. The rest of the settings can be left as default.

![Configure ADC for use with DMA in STM32 CubeMX](https://www.digikey.com.au/maker-media/8d6676f9-34d7-4a18-8f01-b58e0e7a8703)

Save and generate code. Open *main.c*. Change the code to the following:

 

Copy Code

```
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUF_LEN 4096
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint16_t adc_buf[ADC_BUF_LEN];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUF_LEN);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

// Auto-generated HAL code
// ...

/* USER CODE BEGIN 4 */

// Called when first half of buffer is filled
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

// Called when buffer is completely filled
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

/* USER CODE END 4 */

// Rest of main.c
// ...
```

 

 

Note again the addition of two callbacks: HAL_ADC_ConvHalfCpltCallback() and HAL_ADC_ConvCpltCallback(). The first is called whenever the ADC (and DMA) fills up half the buffer. The second function is called whenever the second half the buffer is filled. You can use this to create an easy double (or “ping pong”) buffer. Do something with the data in half the buffer while the second half is being filled by the DMA.

Run the program in the debugger. Add a breakpoint in one of the HAL_ADC callbacks and let the program stop there. Hover your mouse over the adc_buf variable to peek inside the buffer. You should see it filled with ADC conversion values.

![Viewing DMA buffer in STM32CubeIDE](https://www.digikey.com.au/maker-media/a49e4e20-4012-484b-affb-e28a9bec7622)

We’re not doing anything with this data, as this is just an example to show how to fill up a buffer with DAC values without using the CPU, which should be idling in our while(1) loop! Note that both halves of the buffer get filled, as the DMA clock does not stop when we stop the CPU with the debugger, so it will just keep working to put values into the buffer.

If you connect an oscilloscope to D13, you should see the line toggling each time one of the callback functions runs.

![Timing ADC and DMA transfer in STM32](https://www.digikey.com.au/maker-media/d08f031d-d1b9-492e-b67e-e5eca0ec925b)

As you can see, it takes about 472 μs to fill up one half of the buffer. That means in a little under a millisecond, we can fill up the entire buffer--4096 samples--without using the CPU! This comes out to be around 4 Msamples/sec. 

If you take a look at the ADC clocks in STM32CubeIDE, you can see that they are set to 64 MHz and asynchronous from the system clock. If you connect the ADC clock to an 80 MHz source, you can get that sampling rate up even a little higher. Additionally, we are using 12-bit conversions. You can sacrifice some resolution for faster sample and convert times.

**Resources**

I hope this helps you get started with the ADC and DMA on the STM32! The ADC is a wonderful tool, and it offers a vast array of features that you can customize and tweak. Similarly, the DMA can be a bit daunting to set up, but once you do, you’ll be looking for excuses to use it on lots of projects.

Here are some resources that might help you in your journey: