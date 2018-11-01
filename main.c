/*
 * Name: Josephine Ruiter, Abbey Dengler
 * Class: EGR226
 * Professor: Zuidema
 * Midterm project
 * Date: October, 2018
 *
 *  */


#include "msp.h"
#include <stdint.h>
#include <stdio.h>

//PROTOTYPES--------------------------------------------------
void initkey(); //function for initializing the keypad
void initbuttons(); //function for initializing the push buttons
void initLED(); //function for initializing the LEDs
char readkeypad(); //function reads the keypad
int get_pwm();  //function returns a speed for pwm
int get_option();  //function returns a number from menu
void timerA_Motor(int pwm);   //passed pwm and sets the speed using timerA
void timerA_servo(int option);   //opens door using timerA
void timerA_lights(int pwm, int color);   //controls lights using timerA
uint8_t debounce1(); //function prototype for debouncing a switch

//Functions For LCD
void systick_start(void); //prototype for initializing timer
void delay_ms(unsigned); //function prototype for delaying for x ms
void LCD_init(void);  //initializes the LCD
void delay_microsec(unsigned microsec);
void PulseEnablePin(void); //sequences the enable pin
void pushNibble(uint8_t nibble);  //puts one nibble onto data pins
void pushByte(uint8_t byte);    //pushes most significant 4 bits to data pins with the push nibble function
void write_command( uint8_t command); //writes one bit of command by calling pushByte() with the command parameter
void dataWrite(uint8_t data);   //will write one bit of data by calling pushByte()

void main_menu(); //for the display menu
void door_menu(); //for the display menu for door
void motor_menu(); //for the display menu for motor
void lights_menu(); //for the display menu for light color
void lights_menu2(); //for the display menu to get pwm
//-------------------------------------------------------------

 int main(void) {



    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
                 WDT_A_CTL_HOLD;
   systick_start(); //Start up Systick

   initkey(); //initialize keypad all columns and rows as inputs with pull up resistors
   initbuttons(); //function for initializing the push buttons
   initLED(); //function for initializing the LEDs
   LCD_init(); //LCD initialization

   int option=0;
   int color=0;
   //int pwm=0;


    while (1)
    {
        main_menu();
        option= get_option();
//FOR THE MENU SELECTS-------------------
 //DOOR
        if (option== 1)
        {
            door_menu();
            option=0;
            option= get_option();
            if (option==1)
            {
                P5-> OUT &= ~BIT0;  //set output 0 of RED LED
                P5-> OUT |= BIT1;  //set output of GREEN LED to 1
                timerA_servo(option);  //opens door

            }
            if (option==2)
            {
                P5-> OUT &= ~BIT1;  //set output of GREEN LED to 0
                P5-> OUT |= BIT0;  //set output of RED LED to 1
                timerA_servo(option);//DOOR CLOSED
            }

            option=0;

        }
 //MOTOR----------------------------
        if (option== 2)
        {
            motor_menu();

            timerA_Motor(get_pwm());

            option=0;

        }
//LIGHT Brightness-------------------
        if (option== 3)
        {
            lights_menu();
            color=(get_option());
            lights_menu2();
            timerA_lights(get_pwm(), color);

            option=0;

        }
        delay_ms(1000);





    }

}

 // FUNCTIONS-----------------------------------------------------------------------------------------------------------


 //---------------------------------------------------------------------------------------------------------------------
//Function initializes the keypad as inputs
//MSP P4.1-P4.7
void initkey(void)
{
    P4-> SEL0 &= ~BIT7;  //Row 1
    P4-> SEL1 &= ~BIT7;
    P4-> DIR &= ~BIT7; //input
    P4-> REN |= BIT7;  //enable resistor
    P4-> OUT |= BIT7;  //set input

    P4-> SEL0 &= ~BIT6;  //Row 2
    P4-> SEL1 &= ~BIT6;
    P4-> DIR &= ~BIT6; //input
    P4-> REN |= BIT6;  //enable resistor
    P4-> OUT |= BIT6;  //set input

    P4-> SEL0 &= ~BIT5;  //Row 3
    P4-> SEL1 &= ~BIT5;
    P4-> DIR &= ~BIT5; //input
    P4-> REN |= BIT5;  //enable resistor
    P4-> OUT |= BIT5;  //set input

    P4-> SEL0 &= ~BIT4;  //Row 4
    P4-> SEL1 &= ~BIT4;
    P4-> DIR &= ~BIT4; //input
    P4-> REN |= BIT4;  //enable resistor
    P4-> OUT |= BIT4;  //set input

    P4-> SEL0 &= ~BIT3;  //Column 1
    P4-> SEL1 &= ~BIT3;
    P4-> DIR &= ~BIT3; //input
    P4-> REN |= BIT3;  //enable resistor
    P4-> OUT |= BIT3;  //set input

    P4-> SEL0 &= ~BIT2;  //Column 2
    P4-> SEL1 &= ~BIT2;
    P4-> DIR &= ~BIT2; //input
    P4-> REN |= BIT2;  //enable resistor
    P4-> OUT |= BIT2;  //set input

    P4-> SEL0 &= ~BIT1;  //Column 3
    P4-> SEL1 &= ~BIT1;
    P4-> DIR &= ~BIT1; //input
    P4-> REN |= BIT1;  //enable resistor
    P4-> OUT |= BIT1;  //set input
  //this could be done in one set by using a combined number, but this helps for wiring to have rows and columns labeled
}
//---------------------------------------------------------------------------------------------------------------------------------
void initbuttons() //function for initializing the push buttons
{
    P3-> SEL0 &= ~BIT6;  //SWITCH ONE LIGHTS
    P3-> SEL1 &= ~BIT6;
    P3-> DIR &= ~BIT6; //input
    P3-> REN |= BIT6;  //enable resistor
    P3-> OUT |= BIT6;  //set input
    P3 -> IE |= BIT6; //make an interrupt

    P3-> SEL0 &= ~BIT7;  //SWITCH two STOP
    P3-> SEL1 &= ~BIT7;
    P3-> DIR &= ~BIT7; //input
    P3-> REN |= BIT7;  //enable resistor
    P3-> OUT |= BIT7;  //set input
    P3 -> IE |= BIT7; //make an interrupt

    __enable_interrupt();
    NVIC_EnableIRQ(PORT3_IRQn);
}
//----------------------------------------------------------------------------------------------------------------------------
void initLED() //function for initializing the LEDs
{
    P5-> SEL0 &= ~BIT0;  //LED RED
    P5-> SEL1 &= ~BIT0;
    P5-> DIR |= BIT0; //output
    P5-> OUT |= BIT0;  //set output 1

    P5-> SEL0 &= ~BIT1;  //LED GREEN
    P5-> SEL1 &= ~BIT1;
    P5-> DIR |= BIT1; //output
    P5-> OUT &= ~BIT1;  //set output 0

    P7-> SEL0 &= ~BIT6;  //LED BLUE
    P7-> SEL1 &= ~BIT6;
    P7-> DIR |= BIT6; //output
    P7-> OUT &= ~BIT6;  //set output 0

    P7-> SEL0 &= ~BIT5;  //LED ORANGE
    P7-> SEL1 &= ~BIT5;
    P7-> DIR |= BIT5; //output
    P7-> OUT &= ~BIT5;  //set output 0

    P7-> SEL0 &= ~BIT7;  //LED YELLOW
    P7-> SEL1 &= ~BIT7;
    P7-> DIR |= BIT7; //output
    P7-> OUT &= ~BIT7;  //set output 0
//Timer A
    P7->SEL0 |= BIT5; //Orange
    P7->SEL1 &= ~(BIT5);
    P7->DIR |= BIT5;  // P7.5 set TA1.3

    P7->SEL0 |= BIT6;  //Blue
    P7->SEL1 &= ~(BIT6);
    P7->DIR |= BIT6;  // P7.6 set TA1.2

    P7->SEL0 |= BIT7;  //Yellow
    P7->SEL1 &= ~(BIT7);
    P7->DIR |= BIT7;  // P7.7 set TA1.1

    TIMER_A1->CCR[0]  = 24000-1;              // PWM Period (# cycles of clock)
    TIMER_A1->CCTL[3] = 0b11100000;     // CCR1 reset/set mode 7
    TIMER_A1->CCTL[2] = 0b11100000;     // CCR1 reset/set mode 7
    TIMER_A1->CCTL[1] = 0b11100000;     // CCR1 reset/set mode 7
    TIMER_A1->CCR[3]  =0; // CCR1 PWM duty cycle
    TIMER_A1->CCR[2]  =0; // CCR1 PWM duty cycle
    TIMER_A1->CCR[3]  =0; // CCR1 PWM duty cycle

}
//-------------------------------------------------------------------------------------------------------------------------
//Function reads the keypad
//MSP P4.1-P4.7
//uses pieces from Zuidema and Kandalaft slides
char readkeypad()
{

    char val = ',';
    uint8_t row;
    int col;

    for (col=3; col>= 1; col--) //loop through column 1 (P4.3) 2(P4.2) and 3(4.1)
    {
        P4->DIR |= (0b0001 <<col); //sets current column to output
        P4->OUT &= ~(0b0001 <<col); //sets current column output to 0
        __delay_cycles(30000);    //waits .01 seconds before checking

        row= (P4-> IN & 0b11111110); // copies what bit goes low
        while((P4-> IN &(0b11110000))!=0b11110000); //wait until a button is not pressed


        // col 1 and row 1= 1
        if (row == 0b01110110) val= 49; //char 1
        // col 1 and row 2= 4
        if (row == 0b10110110) val= 52; //char 4
        // col 1 and row 3= 7
        if (row == 0b11010110) val= 55; //char 7
        // col 1 and row 4= *
        if (row == 0b11100110) val= 42; //char *

        // col 2 and row 1= 2
        if (row == 0b01111010) val= 50; //char 2
        // col 2 and row 2= 5
        if (row == 0b10111010) val= 53; //char 5
        // col 2 and row 3= 8
        if (row == 0b11011010) val= 56; //char 8
        // col 2 and row 4= 0
        if (row == 0b11101010) val= 48; //char 0

        // col 3 and row 1= 3
        if (row == 0b01111100) val= 51; //char 3
        // col 3 and row 2= 6
        if (row == 0b10111100) val= 54; //char 6
        // col 3 and row 3= 9
        if (row == 0b11011100) val= 57;  //char 9
        // col 3 and row 4= #
       if (row == 0b11101100) val= 35; //char #

       P4->DIR &= ~(0b0001 <<col); //sets current column to input again
       P4->OUT |= (0b0001 <<col); //sets current column to 1
    }
    return val;
}
//---------------------------------------------------------------------------------------------------------------------------------------
int get_pwm() //function to get a pin from keypad, returns speed for pwm
{
    char a[3];
    a[2]= '0';
    a[1]= '0';
    a[0]= '0';

    char temp;
    int speed=0;
    int status=1;
   // printf("Enter a PWM between 0 and 100 and a # when done:\n");
    while (status)
    {
    temp= readkeypad();

        if ((temp!= 35) && (temp !=44)) //check for #
        {

        a [2]= a [1]; //shuffles array
        a [1]= a [0];
        a [0]=temp;


        }

        int hundreds, tens, ones;
        hundreds= (int)a[2];
        tens= (int)a[1];
        ones= (int)a[0];

        speed= ((100*(hundreds-48)) + (10*(tens-48)) + (ones-48)); //converts chars to int

        if (temp == 35)
        {
            if (speed>=101)
            {
              //  printf("Not valid\nEnter a PWM between 0 and 100 and a # when done:\n");
                a[2]= '0';
                a[1]= '0';
                a[0]= '0';
            }

            else
            {
             //   printf("%d\n", speed);
                status=0;               //breaks loop
            }

        }



        __delay_cycles(30000); //wait to read again

    }
    return speed;
}
//--------------------------------------------------------------------------------------------------------------------------------
int get_option()  //function returns a number from menu
{

    char temp= ',';
    int option=0;
    int status=1;

    while (status)
    {
        temp= readkeypad();


        option= (temp-48); //converts chars to int
        if (!(temp== ','))
            status=0;

    }
    return option;
}

//------------------------------------------------------------------------------------------------------------------------------------
void timerA_Motor(pwm)   //passed pwm and sets the speed using timerA pin 2.4
{
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~(BIT4);
    P2->DIR |= BIT4;  // P2.4 set TA0.1


     TIMER_A0->CCR[0]  = 37500-1;              // PWM Period (# cycles of clock)
     TIMER_A0->CCTL[1] = 0b11100000;     // CCR1 reset/set mode 7
     TIMER_A0->CCR[1]  = ((pwm/100.0)* 37500)-1; // CCR1 PWM duty cycle

     TIMER_A0->CTL = 0b1001010000;

     if (pwm==0)
         {
         TIMER_A0->CCR[0]  = 37500-1;              // PWM Period (# cycles of clock)
         TIMER_A0->CCTL[1] = 0b11100000;     // CCR1 reset/set mode 7
         TIMER_A0->CCR[1]  = 0;
         TIMER_A0->CTL = 0b1001010000;
         }

}
//----------------------------------------------------------------------------------------------------------------------------
//opens and closes door based on what option was selected
//1 is open and 0 is closed
void timerA_servo(int option)   // pin 2.5
{
    P2->SEL0 |= BIT5;
    P2->SEL1 &= ~(BIT5);
    P2->DIR |= BIT5;  // P2.5 set TA0.2

     if (option== 1)
     {
         TIMER_A0->CCR[0]  = 40000-1;              // PWM Period (# cycles of clock)
         TIMER_A0->CCTL[2] = 0b11000000;     // CCR1 reset/set mode 7
         TIMER_A0->CCR[2]  = 4000-1; // CCR1 PWM for 90 degrees 10 percent

         TIMER_A0->CTL = 0b1001010000;  //smclk, no divider, upmode
     }
     if (option== 2)
     {
         TIMER_A0->CCR[0]  = 40000-1;              // PWM Period (# cycles of clock)
         TIMER_A0->CCTL[2] = 0b11000000;     // CCR1 reset/set mode 7
         TIMER_A0->CCR[2]  = 2000-1; // CCR1 PWM for 0 degrees 5 percent

         TIMER_A0->CTL = 0b1001010000;  //smclk, no divider, upmode
     }



}
//----------------------------------------------------------------------------------------------------------------------------
// takes in PWM, and Color
// Color 1= Orange
// Color 2= Blue
// Color 3= Yellow
void timerA_lights(int pwm,int color)   // P7.7,6,5
{

     TIMER_A1->CCR[0]  = 24000-1;              // PWM Period (# cycles of clock)
     if (color==1) //COLOR 1= Orange and TA1.3
     {
         TIMER_A1->CCTL[3] = 0b11100000;     // CCR1 reset/set mode 7
         TIMER_A1->CCR[3]  =((pwm/100.0)* 24000)-1; // CCR1 PWM duty cycle

         if(pwm==0)
             TIMER_A1->CCR[3]  =0;
     }
     if (color==2)  //COLOR 2= Blue and TA1.2
     {
         TIMER_A1->CCTL[2] = 0b11100000;     // CCR1 reset/set mode 7
         TIMER_A1->CCR[2]  =((pwm/100.0)* 24000)-1; // CCR1 PWM duty cycle

         if(pwm==0)
             TIMER_A1->CCR[2]  =0;
     }
     if (color==3) //COLOR 3= Yellow and TA1.1
     {
         TIMER_A1->CCTL[1] = 0b11100000;     // CCR1 reset/set mode 7
         TIMER_A1->CCR[1]  =((pwm/100.0)* 24000)-1; // CCR1 PWM duty cycle

         if(pwm==0)
             TIMER_A1->CCR[1]  =0;
     }

     TIMER_A1->CTL = 0b1001010000;
}
//----------------------------------------------------------------------------------------------------------------------------
void systick_start() //initialize timer
{
    SysTick-> CTRL = 0; //off
    SysTick-> LOAD = 0xBB8; //1ms+clock
    SysTick-> VAL = 0; //reset count
    SysTick-> CTRL = 5; //enable bit 0 and 2

}
//-----------------------------------------------------------------------------------------------------------------------------
void delay_ms(unsigned ms) //function for delaying for x ms. takes in ms
{
    SysTick-> LOAD = (3000* ms)-1; //1ms+clock
    SysTick-> VAL = 0; //reset count
    while((SysTick-> CTRL & 0x10000) == 0) {} ;  //function given in lab

}
//------------------------------------------------------------------------------------------------------------------------------
void delay_microsec(unsigned microsec) //function for delaying for x ms. takes in ms
{
    SysTick-> LOAD = (300* microsec)-1; //1micros+clock
    SysTick-> VAL = 0; //reset count
    while((SysTick-> CTRL & 0x10000) == 0) {} ;  //function given in lab

}
//--------------------------------------------------------------------------------------------------------------------------------
void LCD_init(void)  //initializes the LCD
{
    P6-> SEL0 &= ~0b11110011;  //BITS 0,1,4,5,6,7
    P6-> SEL1 &= ~0b11110011;
    P6-> DIR |= 0b11110011; //output
    P6-> OUT &= ~0b11110011;  //set output 0

    write_command(3);   //from lab appendix
    delay_ms(100);
    write_command(3);
    delay_microsec(200);
    write_command(3);
    delay_ms(100);

    write_command(2);
    delay_microsec(100);
    write_command(2);
    delay_microsec(100);

    write_command(8);
    delay_microsec(100);
    write_command(0x0F);
    delay_microsec(100);
    write_command(1);
    delay_microsec(100);
    write_command(6);
    delay_ms(10);
}

//----------------------------------------------------------------------------------------------------------------------------------
void PulseEnablePin(void) //sequences the enable pin- kandalaft code
{
    P6-> OUT &= ~BIT1; //set pulse to 0V
    delay_microsec(10);
    P6-> OUT |= BIT1;
    delay_microsec(10);
    P6-> OUT &= ~BIT1; //set pulse to 0V
    delay_microsec(10);

}

//---------------------------------------------------------------------------------------------------------------------------------
void pushNibble(uint8_t nibble)  //puts one nibble onto data pins - kandalaft code
{
    P6-> OUT &= ~0xF0;  //clear p4.4-4.7
    P6-> OUT |= (nibble & 0x0F)<<4; //sets data pins  p4.4-4.7

    PulseEnablePin();

}

//----------------------------------------------------------------------------------------------------------------------------------
void pushByte(uint8_t byte)    //pushes most significant 4 bits to data pins with the push nibble function - kandalaft code
{
    uint8_t nibble;

    nibble=(byte & 0xF0)>>4;
    pushNibble(nibble);

    nibble=(byte & 0x0F);
    pushNibble(nibble);
    delay_microsec(100);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void write_command( uint8_t command) //writes one bit of command by calling pushByte() with the command parameter
{
    P6-> OUT &= ~BIT0;
    pushByte(command);

}
//--------------------------------------------------------------------------------------------------------------------------------------
void dataWrite(uint8_t data) //will write one bit of data by calling pushByte()
{
    P6-> OUT |= BIT0;
    pushByte(data);
}
//----------------------------------------------------------------------------------------------------------------------------------------
//this funciton displays the overall menu
void main_menu() //for the display menu
{
    write_command(0b00000001); //reset display
    char line1[]= "      Menu";
     char line2[]= "1.Door";
     char line3[]= "2.Motor";
     char line4[]= "3.Lights         ";

     int i=0;
     while(line1[i] != '\0')
     {
         if (line1[i] != '\0')
             dataWrite(line1[i]);
         i++;
     }

     write_command(0b11000000); //moves cursor to second line
     i=0;
     while(line2[i] != '\0')
     {
         if (line2[i] != '\0')
             dataWrite(line2[i]);
         i++;
     }

     write_command(0b10010000); //moves cursor to third line
     i=0;
     while(line3[i] != '\0')
     {
         if (line3[i] != '\0')
             dataWrite(line3[i]);
         i++;
     }

     write_command(0b11010000); //moves cursor to fourth line
     i=0;
     while(line4[i] != '\0')
     {
         if (line4[i] != '\0')
             dataWrite(line4[i]);
         i++;
     }
}
//-----------------------------------------------------------------------------------------------------------------------------
//once the door is selected the door menu appears
void door_menu() //for the display menu
{
    write_command(0b00000001); //reset display
    char line1[]= "   Door Menu";
     char line2[]= "1.Open";
     char line3[]= "2.Closed";
     char line4[]= "                ";

     int i=0;
     while(line1[i] != '\0')
     {
         if (line1[i] != '\0')
             dataWrite(line1[i]);
         i++;
     }

     write_command(0b11000000); //moves cursor to second line
     i=0;
     while(line2[i] != '\0')
     {
         if (line2[i] != '\0')
             dataWrite(line2[i]);
         i++;
     }

     write_command(0b10010000); //moves cursor to third line
     i=0;
     while(line3[i] != '\0')
     {
         if (line3[i] != '\0')
             dataWrite(line3[i]);
         i++;
     }

     write_command(0b11010000); //moves cursor to fourth line
     i=0;
     while(line4[i] != '\0')
     {
         if (line4[i] != '\0')
             dataWrite(line4[i]);
         i++;
     }
}
//----------------------------------------------------------------
//Displays the menu for the motor once the motor is selected 
void motor_menu() //for the display menu
{
    write_command(0b00000001); //reset display
    char line1[]= "   Motor Menu";
     char line2[]= "Enter PWM";
     char line3[]= "Followed by a #";
     char line4[]= "                ";

     int i=0;
     while(line1[i] != '\0')
     {
         if (line1[i] != '\0')
             dataWrite(line1[i]);
         i++;
     }

     write_command(0b11000000); //moves cursor to second line
     i=0;
     while(line2[i] != '\0')
     {
         if (line2[i] != '\0')
             dataWrite(line2[i]);
         i++;
     }

     write_command(0b10010000); //moves cursor to third line
     i=0;
     while(line3[i] != '\0')
     {
         if (line3[i] != '\0')
             dataWrite(line3[i]);
         i++;
     }

     write_command(0b11010000); //moves cursor to fourth line
     i=0;
     while(line4[i] != '\0')
     {
         if (line4[i] != '\0')
             dataWrite(line4[i]);
         i++;
     }
}
//--------------------------------------------------------------------------------------------------------------------------------
// Color 1= Orange
// Color 2= Blue
// Color 3= Yellow
//displays the menu for the Lights it switches it prints the lines
void lights_menu() //for the display menu
{
      write_command(0b00000001); //reset display
      char line1[]= "   Light Menu";
     char line2[]= "1.Orange ";
     char line3[]= "2.Blue";
     char line4[]= "3.Yellow        ";

     int i=0;
     while(line1[i] != '\0')
     {
         if (line1[i] != '\0')
             dataWrite(line1[i]);
         i++;
     }

     write_command(0b11000000); //moves cursor to second line
     i=0;
     while(line2[i] != '\0')
     {
         if (line2[i] != '\0')
             dataWrite(line2[i]);
         i++;
     }

     write_command(0b10010000); //moves cursor to third line
     i=0;
     while(line3[i] != '\0')
     {
         if (line3[i] != '\0')
             dataWrite(line3[i]);
         i++;
     }

     write_command(0b11010000); //moves cursor to fourth line
     i=0;
     while(line4[i] != '\0')
     {
         if (line4[i] != '\0')
             dataWrite(line4[i]);
         i++;
     }
}
//--------------------------------------------------------------------------------------------------------------------------------
//once the light menu is selected and a light is selected the brightness level is then selected 
void lights_menu2() //for the display menu to get pwm
{
      write_command(0b00000001); //reset display
      char line1[]= "   Light Menu";
      char line2[]= "Enter PWM";
      char line3[]= "Followed by a #";
      char line4[]= "                ";

     int i=0;
     while(line1[i] != '\0')
     {
         if (line1[i] != '\0')
             dataWrite(line1[i]);
         i++;
     }

     write_command(0b11000000); //moves cursor to second line
     i=0;
     while(line2[i] != '\0')
     {
         if (line2[i] != '\0')
             dataWrite(line2[i]);
         i++;
     }

     write_command(0b10010000); //moves cursor to third line
     i=0;
     while(line3[i] != '\0')
     {
         if (line3[i] != '\0')
             dataWrite(line3[i]);
         i++;
     }

     write_command(0b11010000); //moves cursor to fourth line
     i=0;
     while(line4[i] != '\0')
     {
         if (line4[i] != '\0')
             dataWrite(line4[i]);
         i++;
     }
}
//-----------------------------------------------------------------------------------------------------------------------------------
//handles the two interrupt buttons.
//This will turn the lights on and off and then an emeragency stop for the motot
//3.6 is the lights and 3.7 is the motor stop
void PORT3_IRQHandler(void)
{
    int status = P3->IFG;                           //Record all flags of all interrupts on this port
    P3->IFG = 0;                                    //Clear all flags
    if(status & BIT6)
    {                                             //If P3.6 had an interrupt (comparing the status with the BIT)
        TIMER_A1->CCTL[3] ^= BIT6 ;                // toggle between reset/set mode and reset
        TIMER_A1->CCTL[2] ^= BIT6 ;
        TIMER_A1->CCTL[1] ^= BIT6 ;
      //  P3 -> IE &= ~BIT6;                          //turn interrupt off

    }
    if(status & BIT7) {                             //If P3.7 had an interrupt (comparing the status with the BIT)
        timerA_Motor(0);                           //Turn motor off
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------

