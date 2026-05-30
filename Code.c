#include <P18F4550.h>
#include <stdio.h>

#pragma config FOSC=HS
#pragma config PWRT=OFF
#pragma config WDT=OFF
#pragma config DEBUG=OFF,LVP=OFF

void int_lcd(void);
void lcdcmd(unsigned char value);
void lcddata(unsigned char value);
void MSDelay(unsigned int itime);
void send_string(char array[]);
void display_temperature();
unsigned int read_ADC();
void setup_ADC();
k
#define ldata PORTD // PortD acts as 8 data lines
#define rs PORTEbits.RE0
#define en PORTEbits.RE1

void main()
{
    char str[] = "Temperature:";
    int_lcd();
    setup_ADC();
    send_string(str);
    
    while (1) {
        lcdcmd(0xC0); // Move cursor to the second line
        display_temperature(); // Display actual temperature
        MSDelay(2000);
    }
}

void send_string(char array[])
{
    int i = 0;
    for (i = 0; array[i] != '\0'; i++) {
        lcddata(array[i]);
        MSDelay(10);
    }
}

void int_lcd()
{
    TRISD = 0x00;
    TRISE = 0x00;
    ADCON1 = 0x0F; // Disable analog input
    MSDelay(250);
    lcdcmd(0x38); // Initialize LCD as 2-row, 16-column in 8-bit mode
    MSDelay(250);
    lcdcmd(0x0E); // Display ON, Cursor ON
    MSDelay(15);
    lcdcmd(0x01); // Clear Display
    MSDelay(15);
    lcdcmd(0x06); // Shift Cursor Right
    MSDelay(15);
    lcdcmd(0x80); // Set cursor at the beginning
}

void lcdcmd(unsigned char value)
{
    ldata = value;
    rs = 0;
    en = 1;
    MSDelay(1);
    en = 0;
}

void lcddata(unsigned char value)
{
    ldata = value;
    rs = 1;
    en = 1;
    MSDelay(1);
    en = 0;
}

void MSDelay(unsigned int itime)
{
    unsigned int i, j;
    for (i = 0; i < itime; i++)
        for (j = 0; j < 135; j++);
}

void setup_ADC()
{
    ADCON1 = 0x0E; // Configure AN0 as analog, others as digital
    ADCON2 = 0b10010101; // Right justified, 12TAD, FOSC/16
    ADCON0 = 0x01; // Enable ADC, Select AN0
}

unsigned int read_ADC()
{
    ADCON0bits.GO = 1; // Start conversion
    while (ADCON0bits.GO); // Wait for conversion to complete
    
    return ((ADRESH << 8) + ADRESL); // Return ADC result
}

void display_temperature()
{
    unsigned int adc_value;
    int temperature;
    char temp_string[10];

    adc_value = read_ADC();
    temperature = ((adc_value * 500) / 1023); // Convert ADC to Celsius
    
    lcdcmd(0xC0); // Move cursor to second line
    sprintf(temp_string, "%d'C  ", temperature);
    send_string(temp_string);
}
