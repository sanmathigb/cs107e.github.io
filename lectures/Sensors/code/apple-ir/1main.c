/*
 * tsop 34838 IR sensor.
 *
   http://irq5.io/2012/07/27/infrared-remote-control-protocols-part-1/
   no signal = 1.

		Signal Part	off (µs)	on (µs)
			leader	9000		4500
			0 bit	560		560
			1 bit	560		1690
			stop	560		–

		we wait for a 9000off.
		then start pulling bits.
	
		not sure what stop is (timeout on on i believe).
 */
#include "uart.h"
#include "timer.h"
#include "printf.h"
#include "reboot.h"

#include "gpio.h"
#include "gpioextra.h"

const unsigned clk = GPIO_PIN2;
const int timeout = 10000;
const unsigned debounce = 3;

// return usecs value was held.
unsigned stable_read(unsigned pin, unsigned val, unsigned timeout) {
	unsigned clocks = 0;

	unsigned time = timer_get_time();
retry:
	while(gpio_read(pin) == val) {
		if((timer_get_time() - time) > timeout)
			return timeout;
	}

	for(clocks = 0; clocks < debounce; clocks++) 
		if(gpio_read(pin) == val)
			goto retry;

	return timer_get_time() - time;
}

// we don't divide b/c, no fp.
int within(unsigned actual, unsigned expected, unsigned eps) { 
	if(actual > (expected + eps) || actual < (expected - eps))
		return 0;
	return 1;
}
int expect(unsigned bit,  unsigned usec, unsigned eps) {
	unsigned time = stable_read(clk, bit, timeout);
	return within(time, usec, eps);
}


unsigned bit(unsigned on, unsigned off) { 
	const unsigned one_tick = 600, eps = 100;

	if(within(on, one_tick, eps)) {
		if(within(off, one_tick, eps))
			return 0;
		if(within(off, one_tick*3, eps*3))
			return 1;
		printf("invalid?   have off=%d, expected %d or %d\n", off, 
						one_tick, one_tick*3);
		return -1;	
	}
	printf("invalid?   have on=%d (off=%d), expected %d or %d\n", 
		on, off, one_tick, one_tick*3);
	return -1;
}

unsigned read_packet(void) {
	unsigned on, off;

retry:
	// wait til we get aligned on the header.
	while(!expect(0, 9000, 500) 
	|| !expect(1, 4500, 500)
	|| !expect(0, 600, 100) 
	|| !expect(1, 600, 100)) 
		;

	unsigned val = 0;
	while(1) {
		if((off = stable_read(clk, 0, timeout)) == timeout) {
			printf("invalid packet, timeout on off-read\n");
			reboot();
			goto retry;
		}

		// stop bit is just 0 and a long on-signal.
		if((on = stable_read(clk, 1, timeout)) == timeout) 
			return val;

		unsigned b = bit(off, on);
		if(b == -1)
			goto retry;
		val = (val << 1) | b;
	}
}

void main(void) {
	uart_init();

  	gpio_set_function(clk, GPIO_FUNC_INPUT);
  	gpio_set_pulldown(clk);

	// reverse engineer the apple protocol.
  	for(int i = 0;  i < 10; i++)
		printf("hit apple remote key: %x\n", read_packet());
  	reboot();
}