#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Define pins for the LEDs.
#define GREEN_LED1_PIN PB0
#define GREEN_LED2_PIN PD7
#define GREEN_LED3_PIN PD6
#define YELLOW_LED1_PIN PC5
#define YELLOW_LED2_PIN PC4
#define YELLOW_LED3_PIN PC3
#define RED_LED1_PIN PC2
#define RED_LED2_PIN PC1

// Define pins for the temperature sensor.
#define TEMPERATURE_SENSOR_PIN PC0

void init_ADC();
uint16_t read_ADC(uint8_t channel);
void init_LEDs();
void control_LEDs(double temperature);

/**
 * @brief Main function.
 * 
 * Initializes the LCD, ADC, and LEDs. Then, it continuously reads the temperature from the ADC,
 * controls the LEDs based on the temperature, and displays the temperature on the LCD.
 */
int main(void) {
	//Setup LCD and initialize ADC and LEDs.
	LCD_Setup();
    init_ADC();
    init_LEDs();

    // Enable global interrupts.
    sei();

    while (1) {
        // Read temperature from ADC.
        uint16_t adc_value = read_ADC(TEMPERATURE_SENSOR_PIN);

        // Convert ADC value to a temperature in Celsius.
        double temperature_celsius = (adc_value * 500.0) / 1023.0;

        // Convert Celsius temperature to Fahrenheit.
        double temperature_fahrenheit = (temperature_celsius * 9 / 5) + 32;

        // Control LEDs based on temperature.
        control_LEDs(temperature_celsius);

		// Convert the temperature reading to a string to be displayed on the LCD.
        int temp_integer_celsius = (int)temperature_celsius;
        int temp_fractional_celsius = (int)((temperature_celsius - temp_integer_celsius) * 100); // get 2 decimal places.
        char tempString[32];
        sprintf(tempString, "%d.%02dC   %d.%02dF", temp_integer_celsius, temp_fractional_celsius,
                (int)temperature_fahrenheit, (int)((temperature_fahrenheit - (int)temperature_fahrenheit) * 100));

		// Display the temperature on the LCD.
        LCD_Clear();
        LCD_GotoXY(0, 0);
        LCD_PrintString("Temperatures: ");
        LCD_GotoXY(0, 1);
        LCD_PrintString(tempString);

		// Delay for some time before next reading.
        _delay_ms(1000);
    }
    return 0;
}

/**
 * @brief Initializes the Analog-to-Digital Converter (ADC) for temperature sensing.
 * 
 * This function sets the reference voltage to AVCC with an external capacitor at the AREF pin
 * and configures the ADC prescaler to 128 for a 16MHz clock to get an ADC clock of 125kHz.
 */
void init_ADC() {
    // Set reference voltage to AVCC with external capacitor at AREF pin.
    ADMUX |= (1 << REFS0);

    // Set ADC prescaler to 128 for 16MHz clock to get ADC clock of 125kHz.
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
	// Enable ADC.
    ADCSRA |= (1 << ADEN);
}

/**
 * @brief Reads the analog value from the specified ADC channel.
 * 
 * @param channel The ADC channel to read from.
 * @return The 10-bit digital value obtained from the ADC conversion.
 */
uint16_t read_ADC(uint8_t channel) {
    // Select ADC channel.
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    // Start single conversion.
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to complete.
    while (ADCSRA & (1 << ADSC));

    // Return ADC value.
    return ADC;
}

/**
 * @brief Initializes the LED pins as output for controlling the LEDs.
 * 
 * This function sets the pins connected to the LEDs as output pins.
 */
void init_LEDs() {
    // Set LED pins as output.
    DDRB |= (1 << GREEN_LED1_PIN);
	DDRD |= (1 << GREEN_LED2_PIN) | (1 << GREEN_LED3_PIN);
    DDRC |= (1 << YELLOW_LED1_PIN) | (1 << YELLOW_LED2_PIN) | (1 << YELLOW_LED3_PIN) |
            (1 << RED_LED1_PIN) | (1 << RED_LED2_PIN);
}

/**
 * @brief Controls the LEDs based on the temperature reading.
 * 
 * This function turns on or off LEDs based on the provided temperature value.
 * Different LEDs correspond to different temperature thresholds.
 * 
 * @param temperature The temperature value in Celsius.
 */
void control_LEDs(double temperature) {
    // Control green LEDs.
    PORTB = (temperature >= 20) ? (PORTB | (1 << GREEN_LED1_PIN)) : (PORTB & ~(1 << GREEN_LED1_PIN));
    PORTD = (temperature >= 28) ? (PORTD | (1 << GREEN_LED2_PIN)) : (PORTD & ~(1 << GREEN_LED2_PIN));
    PORTD = (temperature >= 36) ? (PORTD | (1 << GREEN_LED3_PIN)) : (PORTD & ~(1 << GREEN_LED3_PIN));

    // Control yellow LEDs.
    PORTC = (temperature >= 44) ? (PORTC | (1 << YELLOW_LED1_PIN)) : (PORTC & ~(1 << YELLOW_LED1_PIN));
    PORTC = (temperature >= 52) ? (PORTC | (1 << YELLOW_LED2_PIN)) : (PORTC & ~(1 << YELLOW_LED2_PIN));
    PORTC = (temperature >= 60) ? (PORTC | (1 << YELLOW_LED3_PIN)) : (PORTC & ~(1 << YELLOW_LED3_PIN));

    // Control red LEDs.
    PORTC = (temperature >= 68) ? (PORTC | (1 << RED_LED1_PIN)) : (PORTC & ~(1 << RED_LED1_PIN));
    PORTC = (temperature >= 76) ? (PORTC | (1 << RED_LED2_PIN)) : (PORTC & ~(1 << RED_LED2_PIN));
}
