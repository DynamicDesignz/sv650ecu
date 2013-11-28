/*
 * Functions for writing to the Oasis 4 char 7 segment display
 */

#include "sv650_reader.h"

const char DISPPOS[4] = { 0xc0, 0xc2, 0xc4, 0xc6 };

/*
 * Gets the LCD value for a given char
 */
byte 
get_display_char(char val)
{
    return pgm_read_byte_near(display_table + int(val));
}

/*
 * Gets the LCD value for a given byte value
 */
byte 
get_display_hex(int val)
{
    return pgm_read_byte_near(display_table + val + 0x30);
}

void 
display_chars(char digit1, char digit2, char digit3, char digit4)
{

    digitalWrite(CS,LOW);
    shiftOut(MOSI, CLK, LSBFIRST, DISPPOS[0]);
    shiftOut(MOSI, CLK, LSBFIRST, get_display_char(digit1));
    shiftOut(MOSI, CLK, LSBFIRST, DISPPOS[1]);
    shiftOut(MOSI, CLK, LSBFIRST, get_display_char(digit2));
    shiftOut(MOSI, CLK, LSBFIRST, DISPPOS[2]);
    shiftOut(MOSI, CLK, LSBFIRST, get_display_char(digit3));
    shiftOut(MOSI, CLK, LSBFIRST, DISPPOS[3]);
    shiftOut(MOSI, CLK, LSBFIRST, get_display_char(digit4));
    digitalWrite(CS,HIGH);    
}

void 
display_values(char digit1, char digit2, char digit3, char digit4)
{

    digitalWrite(CS,LOW);
    shiftOut(MOSI, CLK, LSBFIRST, DISPPOS[0]);
    shiftOut(MOSI, CLK, LSBFIRST, digit1);
    shiftOut(MOSI, CLK, LSBFIRST, DISPPOS[1]);
    shiftOut(MOSI, CLK, LSBFIRST, digit2);
    shiftOut(MOSI, CLK, LSBFIRST, DISPPOS[2]);
    shiftOut(MOSI, CLK, LSBFIRST, digit3);
    shiftOut(MOSI, CLK, LSBFIRST, DISPPOS[3]);
    shiftOut(MOSI, CLK, LSBFIRST, digit4);
    digitalWrite(CS,HIGH);    
}

void 
display_char(int position, char digit)
{
    digitalWrite(CS,LOW);
    shiftOut(MOSI, CLK, LSBFIRST, DISPPOS[position]);
    shiftOut(MOSI, CLK, LSBFIRST, get_display_char(digit));
    digitalWrite(CS,HIGH);    
}

/*
 * Print error code to LED display from the table
 */
void 
print_led_error(char tps_adjust, int idx) {
   int i = 0;
   byte led_digits[4] = { 0xc0, 0xc2, 0xc4, 0xc6 };

   led.sendDigit(led_digits[0], tps_adjust);

    for (i = 1; i < 4; i++) {
        led.sendDigit(led_digits[i], error_table[idx].led[i-1]);
    } 
}


#ifdef ENABLE_TEMP
/*
 * Print water temp
 */
void 
print_led_temp()
{
    unsigned int adc_value;
    unsigned int temp;
    int holding;
    char display[4];

    // temp is all of byte 0 and top two bits of byte 1
    adc_value = sbytes[0] << 2;
    adc_value += (sbytes[1] & 0xc0) >> 6;

    // covert temp into a value fareinheit
    // and then use led.sendDigit()

    display[3] = get_display_char('F');
    if (adc_value <= 42) {
        // temp == HI 
        display[0] = get_display_char('H');
        display[1] = get_display_char('I');
        display[2] = 0;
    } else if (adc_value >= 525) {
        // temp == --- 
        display[0] = get_display_char('L'); 
        display[1] = get_display_char('O'); 
        display[2] = 0;
    } else {
        temp = pgm_read_byte_near(temp_table + (adc_value - 42)) + 60;
        serial_printf("temp is %u\n", temp);

        holding = temp % 10;
        display[2] = get_display_hex(holding);
        temp -= holding;
        if (temp > 199) {
            display[0] = get_display_hex(2);
            temp -= 200;
        } else if (temp > 99) {
            display[0] = get_display_hex(1);
            temp -= 100;
        } else {
            display[0] = 0;
        }

        display[1] = get_display_hex(temp / 10);

    }
    display_values(display[0], display[1], display[2], display[3]);
}

/*
 * Notify user of bad Temp condition
 */
void 
print_led_bad_temp()
{
    char display[4] = { 'B', 'A', 'D', 'T' };  // BADT
#ifdef DEBUG
    serial_printf("print_led_bad_temp\n");
#endif
    display_chars(display[0], display[1], display[2], display[3]);
}
#endif

/*
 * Notify user of bad EFI condition
 */
void 
print_led_bad_efi()
{
    char display[4] = { 'B', 'A', 'D', 'E' };  // BADE
#ifdef DEBUG
    serial_printf("print_led_bad_efi\n");
#endif
    display_chars(display[0], display[1], display[2], display[3]);
}

