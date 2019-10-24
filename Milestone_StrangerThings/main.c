#include <msp430.h>


int length = 0;             //initialize length variable
int counter = 0;
int RED=0;              //initialize RED variable
int GREEN=0;            //initialize GREEN variable
int BLUE=0;             //initialize BLUE variable

void timersetup(void);          //Declare timer setup function
void ledsetup(void);             //Declare led setup function
void UARTsetup(void);            //Declare UART setup function

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               //stop watchdog timer

    timersetup();               //Call timer setup function
    ledsetup();                  //Call led setup function
    UARTsetup();                 //Call UARTsetup function

    __bis_SR_register(LPM0_bits + GIE);     //low power mode and interrupt enabled setup
}



void timersetup(void)           //timer setup function
{
    //timer A0
    TA0CTL |= TASSEL_2 + MC_1;     //smclk, up mode
    TA0CCTL1 |= OUTMOD_7;   //set/reset output
    TA0CCR0 = 255;          //pwm period
    TA0CCR1 = 0;            //initialize red pwm

    //timer A1
    TA1CTL |= TASSEL_2 + MC_1;     //set smclk, up mode
    TA1CCTL1 |= OUTMOD_7;   //set/reset output
    TA1CCTL2 |= OUTMOD_7;   //set/reset output
    TA1CCR0 = 255;          //pwm period
    TA1CCR1 = 0;            //initialize green pwm
    TA1CCR2 = 0;            //initialize blue pwm

}





void ledsetup(void)              //led setup function
{
    P1DIR |= BIT6;          //Set Red led Direction Register to output
    P1SEL |= BIT6;          //Enable pwm for Red led
    P2DIR |= (BIT1 + BIT5); //Set Green(Bit1) and Blue(BIT5) led Direction Register to output
    P2SEL |= (BIT1 + BIT5); //Enable pwm for Green and Blue led

}






void UARTsetup(void)             //UART setup function
{
    P1SEL |= BIT1 + BIT2;                        //sets P1.1 to RX and P1.2 to TX
    P1SEL2 |= BIT1 + BIT2;                       //sets P1.1 to RX and P1.2 to TX

    UCA0CTL1 |= UCSSEL_2;                        //sets BR clock to SMCLCK
    UCA0BR0 = 109;                               //sets Baud rate to 9600
    UCA0BR1 = 0;                                 //sets Baud rate to 9600
    UCA0MCTL = UCBRS_2;

    UCA0CTL1 &= ~UCSWRST;                        //Disables UART reset
    UC0IE |= UCA0RXIE;                           //Enables UART interrupt
}





#pragma vector=USCIAB0RX_VECTOR     //Interrupt routine
__interrupt void USCIAB0RX(void)
{
  while (!(IFG2&UCA0TXIFG));        //Wait until a byte is ready
  if(counter==1)                    //Receive first signal
  {
      UCA0TXBUF=UCA0RXBUF-3;        //Output signal gets input signal minus the 3
      length=UCA0RXBUF-3;           //length equal to input signal - 3
  }
  else if(counter<5)                // because it starts at 1, this will occur when counter = 2, 3, and 4 or the values of the RGB
  {
      switch(counter)               //switch statement for 2nd 3rd and 4th bytes of input signal
      {
      case 2:                       //2nd input signal
          RED=UCA0RXBUF;            //set byte = RED
          break;
      case 3:                       //3rd input signal
          GREEN=UCA0RXBUF;          //set byte = GREEN
          break;
      case 4:                       //4th input signal
          BLUE=UCA0RXBUF;           //set byte = BLUE
          break;
      default:
          break;
      }

  }
  else                              //if it isnt at the first byte or any of the R G B values, then it outputs the Byte
    UCA0TXBUF = UCA0RXBUF;
  if(counter==length+3)             //If  the carriage return byte, then reset back to 0
  {
      counter=0;                    //reset counter back to zero
      TA0CCR1=RED;                   //set RED variable to its timer
      TA1CCR1=GREEN;                //set GREEN variable to its timer
      TA1CCR2=BLUE;                 //set BLUE variable to its timer
  }

  counter++;                        //increment counter by 1
}
