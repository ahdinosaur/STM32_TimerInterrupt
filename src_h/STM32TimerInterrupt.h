/****************************************************************************************************************************
   STM32TimerInterrupt.h
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

#pragma once

#if !( defined(STM32F0) || defined(STM32F1) || defined(STM32F2) || defined(STM32F3)  ||defined(STM32F4) || defined(STM32F7) || \
       defined(STM32L0) || defined(STM32L1) || defined(STM32L4) || defined(STM32H7)  ||defined(STM32G0) || defined(STM32G4) || \
       defined(STM32WB) || defined(STM32MP1) )
  #error This code is designed to run on STM32F/L/H/G/WB/MP1 platform! Please check your Tools->Board setting.
#endif

#define STM32_TIMER_INTERRUPT_VERSION       "1.0.1"

#ifndef STM32_TIMER_INTERRUPT_DEBUG
  #define STM32_TIMER_INTERRUPT_DEBUG       0
#endif

class STM32TimerInterrupt;

typedef STM32TimerInterrupt STM32Timer;

typedef void (*timerCallback)  (void);


class STM32TimerInterrupt
{
  private:
    TIM_TypeDef*    _timer;
    HardwareTimer*  _hwTimer = NULL;

    timerCallback   _callback;        // pointer to the callback function
    float           _frequency;       // Timer frequency
    uint32_t        _timerCount;      // count to activate timer

  public:

    STM32TimerInterrupt(TIM_TypeDef* timer)
    {
      _timer = timer;
      
      _hwTimer = new HardwareTimer(_timer);
      
      _callback = NULL;
    };
    
    ~STM32TimerInterrupt()
    {
      if (_hwTimer)
        delete _hwTimer;
    }

    // frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to STM32-hal-timer.c
    bool setFrequency(float frequency, timerCallback callback)
    {
      // select timer frequency is 1MHz for better accuracy. We don't use 16-bit prescaler for now.
      // Will use later if very low frequency is needed.
      _frequency  = 1000000;
      _timerCount = (uint32_t) _frequency / frequency;

#if (STM32_TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("STM32TimerInterrupt: Timer Input Freq (Hz) = " + String(_hwTimer->getTimerClkFreq()) + ", _fre = " + String(_frequency)
                     + ", _count = " + String((uint32_t) (_timerCount)));          
#endif

      _hwTimer->setCount(0, MICROSEC_FORMAT);
      _hwTimer->setOverflow(_timerCount, MICROSEC_FORMAT);

      _hwTimer->attachInterrupt(callback);
      _hwTimer->resume();

      return true;
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to STM32-hal-timer.c
    bool setInterval(unsigned long interval, timerCallback callback)
    {
      return setFrequency((float) (1000000.0f / interval), callback);
    }

    bool attachInterrupt(float frequency, timerCallback callback)
    {
      return setFrequency(frequency, callback);
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to STM32-hal-timer.c
    bool attachInterruptInterval(unsigned long interval, timerCallback callback)
    {
      return setFrequency( (float) ( 1000000.0f / interval), callback);
    }

    void detachInterrupt()
    {
      _hwTimer->detachInterrupt();
    }

    void disableTimer(void)
    {
      //_hwTimer->detachInterrupt();
      _hwTimer->pause();
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void reattachInterrupt()
    {
      setFrequency(_frequency, _callback);
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void enableTimer(void)
    {
      //setFrequency(_frequency, _callback);
      _hwTimer->setCount(0, MICROSEC_FORMAT);
      _hwTimer->resume();
    }

    // Just stop clock source, clear the count
    void stopTimer(void)
    {
      _hwTimer->pause();
      _hwTimer->setCount(0, MICROSEC_FORMAT);
    }

    // Just reconnect clock source, start current count from 0
    void restartTimer(void)
    {
      _hwTimer->setCount(0, MICROSEC_FORMAT);
      _hwTimer->resume();
    }
}; // class STM32TimerInterrupt
