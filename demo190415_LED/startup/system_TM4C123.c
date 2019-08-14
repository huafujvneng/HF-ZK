#include <stdint.h>
#include <stdbool.h>

#include "../driverlib/sysctl.h"
#include "../driverlib/rom.h"

void SystemInit(void) __attribute__((section("BOOTUP")));
void SystemInit(void)
{
		//SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
		//ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_INT);
		ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}
