#include <16F886.h>
#device adc = 10 *= 16
#device PASS_STRINGS = IN_RAM

#FUSES NOWDT     //No Watch Dog Timer
#FUSES HS        //High speed Osc (> 4mhz for PCM/PCH) (>10mhz for PCD)
#FUSES PUT       //Power Up Timer
#FUSES NOMCLR    //Master Clear pin not enabled
#FUSES NOPROTECT //Code not protected from reading
#FUSES NOCPD     //No EE protection
#FUSES BROWNOUT  //Brownout reset
#FUSES IESO      //Internal External Switch Over mode enabled
#FUSES FCMEN     //Fail-safe clock monitor enabled
#FUSES NOLVP     //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NODEBUG   //No Debug mode for ICD
#FUSES NOWRT     //Program memory not write protected
#FUSES BORV40    //Brownout reset at 4.0V
#FUSES RESERVED  //Used to set the reserved FUSE bits
#FUSES INTRC_IO

#use delay(clock = 8M)

#use rs232(baud = 9600, parity = N, xmit = PIN_C6, rcv = PIN_C7, bits = 8)

#define RUN_BUTTON PIN_B7

#define SLAVE_ADDRESS 0xB0
#define DISPLAY_ADDRESS 0xB4
#define REGISTER_SIZE 8
#define DISPLAY_CMD_SEND_VALUE 2
#define DISPLAY_CMD_SEND_LONG_TEXT 5
#define DISPLAY_CMD_CLS 6
#define DISPLAY_CMD_SETPOS 8
#define DEVICE_DEF 1

#use i2c(MASTER, I2C1, FORCE_HW)

void displayLongText(char *text);
void displayValue(int16 value[]);
void clearDisplay();
void setDisplayPos(int pos);

int16 f;
float y;
#INT_TIMER1
void timer1_isr()
{
   set_timer1(x);
   output_toggle(PIN_B6);
}

void main()
{
   setup_adc_ports(sAN0);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);

   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
   set_timer1(x);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);

   delay_us(100);

   while (1)
   {
      f = read_adc() / 2;
      y = ((0.000004 * f * 65536) - 1) / (0.000004 * f);
      x = y;
      setDisplayPos(1);
      displayLongText("Frequency: ");
      displayValue(f);
      setDisplayPos(17);
      displayLongText("TimeValue: ");
      displayValue(y);
      delay_ms(1000);
      clearDisplay();
   }
}

void displayLongText(char *text)
{
   int i;
   i2c_start();
   i2c_write(DISPLAY_ADDRESS);
   i2c_write(DISPLAY_CMD_SEND_LONG_TEXT);
   for (i = 0; text[i] != '\0'; i++)
   {
      i2c_write(text[i]);
   }
   i2c_write('\0');
   i2c_stop();
}

void displayValue(int16 value[])
{

   i2c_start();
   i2c_write(DISPLAY_ADDRESS);
   i2c_write(DISPLAY_CMD_SEND_VALUE);
   i2c_write((int)(value >> 8));
   i2c_write((int)(value & 0xFF));
   i2c_write('\0');
   i2c_stop();
}

void clearDisplay()
{

   i2c_start();
   i2c_write(DISPLAY_ADDRESS);
   i2c_write(DISPLAY_CMD_CLS);
   i2c_stop();
}

void setDisplayPos(int pos)
{
   i2c_start();
   i2c_write(DISPLAY_ADDRESS);
   i2c_write(DISPLAY_CMD_SETPOS);
   i2c_write(pos);
   i2c_write('\0');
   i2c_stop();
}
