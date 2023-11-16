#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#define XTAL 16000000

#define LED_PIN     GPIO_PIN_1
#define BUTTON_PIN  GPIO_PIN_4

/* Pines para sensores */
#define SENSOR_PORT_BASE GPIO_PORTD_BASE
#define IRSensor1_PIN GPIO_PIN_0
#define IRSensor2_PIN GPIO_PIN_1
#define IRSensor3_PIN GPIO_PIN_2
#define IRSensor4_PIN GPIO_PIN_3

/* Pines para LEDS */
#define RED_LED_PORT_BASE GPIO_PORTC_BASE
#define RED1_PIN GPIO_PIN_4
#define RED2_PIN GPIO_PIN_5
#define RED3_PIN GPIO_PIN_6
#define RED4_PIN GPIO_PIN_7

#define GREEN_LED_PORT_BASE GPIO_PORTB_BASE
#define GREEN1_PIN GPIO_PIN_3
#define GREEN2_PIN GPIO_PIN_2
#define GREEN3_PIN GPIO_PIN_7
#define GREEN4_PIN GPIO_PIN_6

int prevSensor1State = -1;
int prevSensor2State = -1;
int prevSensor3State = -1;
int prevSensor4State = -1;

void initUART5(void);
void initGPIO(void);

int main(void) {
    // Seteamos reloj a  50 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Inicializamos UART5
    initUART5();

    // Inicializamos GPIO
    initGPIO();

    while (1) {
        // Se leen los puertos
        int sensor1Read = GPIOPinRead(SENSOR_PORT_BASE, IRSensor1_PIN);
        int sensor2Read = GPIOPinRead(SENSOR_PORT_BASE, IRSensor2_PIN);
        int sensor3Read = GPIOPinRead(SENSOR_PORT_BASE, IRSensor3_PIN);
        int sensor4Read = GPIOPinRead(SENSOR_PORT_BASE, IRSensor4_PIN);

        /* SENSOR 1 */
        if (sensor1Read != prevSensor1State) {
            if (sensor1Read == 0) {
                UARTCharPut(UART5_BASE, '1');

            }
            else {
                UARTCharPut(UART5_BASE, '2');
            }
            prevSensor1State = sensor1Read;
        }
        if (sensor1Read == 0) {
            GPIOPinWrite(RED_LED_PORT_BASE, RED1_PIN, RED1_PIN); // enciende led rojo
            GPIOPinWrite(GREEN_LED_PORT_BASE, GREEN1_PIN, 0x00);
        }
        else {
            GPIOPinWrite(RED_LED_PORT_BASE, RED1_PIN, 0x00); // enciende led verde
            GPIOPinWrite(GREEN_LED_PORT_BASE, GREEN1_PIN, GREEN1_PIN);
        }

        /* SENSOR 2*/
        if (sensor2Read != prevSensor2State) {
            if (sensor2Read == 0) {
                UARTCharPut(UART5_BASE, '3');

            }
            else {
                UARTCharPut(UART5_BASE, '4');
            }
            prevSensor2State = sensor2Read;
        }
        if (sensor2Read == 0) {
            GPIOPinWrite(RED_LED_PORT_BASE, RED2_PIN, RED2_PIN); // enciende led rojo
            GPIOPinWrite(GREEN_LED_PORT_BASE, GREEN2_PIN, 0x00);
        }
        else {
            GPIOPinWrite(RED_LED_PORT_BASE, RED2_PIN, 0x00); // enciende led verde
            GPIOPinWrite(GREEN_LED_PORT_BASE, GREEN2_PIN, GREEN2_PIN);
        }

        /* SENSOR 3*/
        if (sensor3Read != prevSensor3State) {
            if (sensor3Read == 0) {
                UARTCharPut(UART5_BASE, '5');

            }
            else {
                UARTCharPut(UART5_BASE, '6');
            }
            prevSensor3State = sensor3Read;
        }
        if (sensor3Read == 0) {
            GPIOPinWrite(RED_LED_PORT_BASE, RED3_PIN, RED3_PIN); // enciende led rojo
            GPIOPinWrite(GREEN_LED_PORT_BASE, GREEN3_PIN, 0x00);
        }
        else {
            GPIOPinWrite(RED_LED_PORT_BASE, RED3_PIN, 0x00); // enciende led verde
            GPIOPinWrite(GREEN_LED_PORT_BASE, GREEN3_PIN,   GREEN3_PIN);
        }
        /* SENSOR 4*/
        if (sensor4Read != prevSensor4State) {
            if (sensor4Read == 0) {
                UARTCharPut(UART5_BASE, '7');

            }
            else {
                UARTCharPut(UART5_BASE, '8');
            }
            prevSensor4State = sensor4Read;
        }
        if (sensor4Read == 0) {
            GPIOPinWrite(RED_LED_PORT_BASE, RED4_PIN, RED4_PIN); // enciende led rojo
            GPIOPinWrite(GREEN_LED_PORT_BASE, GREEN4_PIN, 0x00);
        }
        else {
            GPIOPinWrite(RED_LED_PORT_BASE, RED4_PIN, 0x00); // enciende led verde
            GPIOPinWrite(GREEN_LED_PORT_BASE, GREEN4_PIN, GREEN4_PIN);
        }
        SysCtlDelay(100000);
    }
}

void initUART5(void) {
    // Habilitamos UART5
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Configuramos los pines para UART5
    GPIOPinConfigure(GPIO_PE4_U5RX);
    GPIOPinConfigure(GPIO_PE5_U5TX);
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Inicializamos el UART5
    UARTConfigSetExpClk(UART5_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
}

void initGPIO(void) {
    // Habilitamos puertos a utilizar
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // Configuramos los pines para sensores
    GPIOPinTypeGPIOInput(SENSOR_PORT_BASE, IRSensor1_PIN);
    GPIOPinTypeGPIOInput(SENSOR_PORT_BASE, IRSensor2_PIN);
    GPIOPinTypeGPIOInput(SENSOR_PORT_BASE, IRSensor3_PIN);
    GPIOPinTypeGPIOInput(SENSOR_PORT_BASE, IRSensor4_PIN);

    // Configuramos los pines para LEDs
    GPIOPinTypeGPIOOutput(RED_LED_PORT_BASE, RED1_PIN);
    GPIOPinTypeGPIOOutput(RED_LED_PORT_BASE, RED2_PIN);
    GPIOPinTypeGPIOOutput(RED_LED_PORT_BASE, RED3_PIN);
    GPIOPinTypeGPIOOutput(RED_LED_PORT_BASE, RED4_PIN);

    GPIOPinTypeGPIOOutput(GREEN_LED_PORT_BASE, GREEN1_PIN);
    GPIOPinTypeGPIOOutput(GREEN_LED_PORT_BASE, GREEN2_PIN);
    GPIOPinTypeGPIOOutput(GREEN_LED_PORT_BASE, GREEN3_PIN);
    GPIOPinTypeGPIOOutput(GREEN_LED_PORT_BASE, GREEN4_PIN);
}
