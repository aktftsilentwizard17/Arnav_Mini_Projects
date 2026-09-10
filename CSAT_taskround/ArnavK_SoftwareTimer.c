#include "sam.h"
#include <stdio.h>
#include <stdint.h>

#define MCK_FREQ    120000000UL       // Master Clock Frequency (Hz)
#define TIMER_DIV   128               // Divider value MCK/128
#define TICK_HZ     1                 // 1Hz tick value

static void initialize_timer(void)
{
    // Enable clock for TC channel 0
    PMC->PMC_PCER0 = (1 << ID_TC0);

    // Disable TC channel clock before configuration
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;

    TC0->TC_CHANNEL[0].TC_CMR =
    TC_CMR_TCCLKS_TIMER_CLOCK4 |   // Clock = MCK/128
    TC_CMR_WAVE |                  // Waveform mode
    TC_CMR_WAVSEL_UP_RC;           // Count up to RC then reset

    // RC value for desired tick frequency
    uint32_t RC_value = MCK_FREQ / TIMER_DIV / TICK_HZ;
    TC0->TC_CHANNEL[0].TC_RC = RC_value;

    // Disable all interrupts, using polling method
    TC0->TC_CHANNEL[0].TC_IDR = 0xFFFFFFFF;

    // Enable and trigger the counter
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

int main(void)
{
    SystemInit(); //predefined in sam.h to initialize the SAM system
    initialize_timer();

    uint32_t total_seconds = 0;

    while (1)
    {
        uint32_t status = TC0->TC_CHANNEL[0].TC_SR;
        if (status & TC_SR_CPCS)
        {
            total_seconds++;

            uint32_t hours = (total_seconds / 3600) % 24;
            uint32_t minutes = (total_seconds % 3600) / 60;
            uint32_t seconds = total_seconds % 60;

            printf("%02lu:%02lu:%02lu\n", hours, minutes, seconds);
        }
    }
    return 0;
}