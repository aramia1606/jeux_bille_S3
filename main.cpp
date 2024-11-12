#include "mbed.h"
//#include "C12832.h"

BusIn joy(p15,p12,p13,p16);

BusOut leds(LED1,LED2,LED3,LED4);


DigitalIn fire(p14);
int main()
{
    while(1) {
        if (fire) {
            leds=0xf;
        } else {
            leds=joy;
        }
        
        //Stores x and y output values
        float x, y;

        //Calls the "GetXValue" and "GetYValue" and stores it in x and y
        x = joy.GetXValue();
        y = joy.GetYValue();

        //Prints the values to the terminal
        pc.printf("X: %f, Y: %f\n", x, y);

        //If the button flag is pressed switch the led
        if (joystick.get_button_flag()) {
            r_led = !r_led;
        }
        wait(0.1);
    }
}
