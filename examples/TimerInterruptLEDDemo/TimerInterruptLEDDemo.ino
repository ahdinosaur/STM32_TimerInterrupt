/****************************************************************************************************************************
   TimerInterruptLEDDemo.ino
   For STM32 boards
   Written by Khoi Hoang

   Built by Khoi Hoang https://github.com/khoih-prog/STM32_TimerInterrupt
   Licensed under MIT license

   Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
   unsigned long miliseconds), you just consume only one STM32 timer and avoid conflicting with other cores' tasks.
   The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
   Therefore, their executions are not blocked by bad-behaving functions / tasks.
   This important feature is absolutely necessary for mission-critical tasks.

   Based on SimpleTimer - A timer library for Arduino.
   Author: mromani@ottotecnica.com
   Copyright (c) 2010 OTTOTECNICA Italy

   Based on BlynkTimer.h
   Author: Volodymyr Shymanskyy

   Version: 1.0.1

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
    1.0.0   K Hoang      30/10/2020 Initial coding
    1.0.1   K Hoang      06/11/2020 Add complicated example ISR_16_Timers_Array using all 16 independent ISR Timers.
*****************************************************************************************************************************/

/*
   Notes:
   Special design is necessary to share data between interrupt code and the rest of your program.
   Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
   variable can not spontaneously change. Because your function may change variables while your program is using them,
   the compiler needs this hint. But volatile alone is often not enough.
   When accessing shared variables, usually interrupts must be disabled. Even with volatile,
   if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
   If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
   or the entire sequence of your code which accesses the data.
*/

#if !( defined(STM32F0) || defined(STM32F1) || defined(STM32F2) || defined(STM32F3)  ||defined(STM32F4) || defined(STM32F7) || \
       defined(STM32L0) || defined(STM32L1) || defined(STM32L4) || defined(STM32H7)  ||defined(STM32G0) || defined(STM32G4) || \
       defined(STM32WB) || defined(STM32MP1) )
  #error This code is designed to run on STM32F/L/H/G/WB/MP1 platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "STM32TimerInterrupt.h"
// Don't define STM32_STM32_TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the system.
#define STM32_TIMER_INTERRUPT_DEBUG      1

#include "STM32TimerInterrupt.h"

#ifndef LED_BUILTIN
  #define LED_BUILTIN       PB0               // Pin 33/PB0 control on-board LED_GREEN on F767ZI
#endif

#ifndef LED_BLUE
  #define LED_BLUE          PB7               // Pin 73/PB7 control on-board LED_BLUE on F767ZI
#endif

#ifndef LED_RED
  #define LED_RED           PB14              // Pin 74/PB14 control on-board LED_BLUE on F767ZI
#endif
   
#include "STM32TimerInterrupt.h"
#include "STM32_ISR_Timer.h"

#define TIMER_INTERVAL_MS         100
#define HW_TIMER_INTERVAL_MS      50

// Depending on the board, you can select STM32 Hardware Timer from TIM1-TIM22
// For example, F767ZI can select Timer from TIM1-TIM14
// If you select a Timer not correctly, you'll get a message from ci[ompiler
// 'TIMxx' was not declared in this scope; did you mean 'TIMyy'? 

// Init STM32 timer TIM1
STM32Timer ITimer(TIM1);

// Init STM32_ISR_Timer
// Each STM32_ISR_Timer can service 16 different ISR-based timers
STM32_ISR_Timer ISR_Timer;

#define TIMER_INTERVAL_0_5S           500L
#define TIMER_INTERVAL_1S             1000L
#define TIMER_INTERVAL_1_5S           1500L

void TimerHandler(void)
{
  ISR_Timer.run();
}

// In STM32, avoid doing something fancy in ISR, for example complex Serial.print with String() argument
// The pure simple Serial.prints here are just for demonstration and testing. Must be eliminate in working environment
// Or you can get this run-time error / crash
void doingSomething1()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void doingSomething2()
{
  digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
}
void doingSomething3()
{
  digitalWrite(LED_RED, !digitalRead(LED_RED));
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nTimerInterruptLEDDemo on " + String(BOARD_NAME));
  Serial.println("CPU Frequency = " + String(F_CPU / 1000000) + " MHz");

  // Instantiate HardwareTimer object. Thanks to 'new' instanciation, HardwareTimer is not destructed when setup() function is finished.
  //HardwareTimer *MyTim = new HardwareTimer(Instance);

  // configure pin in output mode
  pinMode(LED_BUILTIN,  OUTPUT);
  pinMode(LED_BLUE,     OUTPUT);
  pinMode(LED_RED,      OUTPUT);

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    Serial.println("Starting  ITimer OK, millis() = " + String(millis()));
  }
  else
    Serial.println("Can't set ITimer correctly. Select another freq. or interval");

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_Timer
  ISR_Timer.setInterval(TIMER_INTERVAL_0_5S,  doingSomething1);
  ISR_Timer.setInterval(TIMER_INTERVAL_1S,    doingSomething2);
  ISR_Timer.setInterval(TIMER_INTERVAL_1_5S,  doingSomething3);
}


void loop()
{
  /* Nothing to do all is done by hardware. Even no interrupt required. */
}
