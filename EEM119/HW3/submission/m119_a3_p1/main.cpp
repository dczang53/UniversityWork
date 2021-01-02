#include "mbed.h"
#include "mbed_events.h"
#include "Hexi_KW40Z.h"
#include "Hexi_OLED_SSD1351.h"
#include "OLED_types.h"
#include "OpenSans_Font.h"
#include "string.h"

#define LED_ON      0
#define LED_OFF     1

DigitalOut pwm(PTA10);
//PwmOut wave(PTA10);    
DigitalIn rx(PTD2);
Serial pc(USBTX, USBRX);

Ticker flipper1;
Ticker flipper2;

void UpdateSensorData(void);
void StartHaptic(void);
void StopHaptic(void const *n);
void txTask(void);
void displayString();
void clearScreen();

//DigitalOut redLed(LED1,1);
//DigitalOut greenLed(LED2,1);
DigitalOut blueLed(LED3,1);
DigitalOut haptic(PTB9);

/* Define timer for haptic feedback */
RtosTimer hapticTimer(StopHaptic, osTimerOnce);

/* Instantiate the Hexi KW40Z Driver (UART TX, UART RX) */ 
KW40Z kw40z_device(PTE24, PTE25);

/* Instantiate the SSD1351 OLED Driver */ 
SSD1351 oled(PTB22,PTB21,PTC13,PTB20,PTE6, PTD15); /* (MOSI,SCLK,POWER,CS,RST,DC) */

/*Create a Thread to handle sending BLE Sensor Data */ 
Thread txThread;

Thread displayThread;
EventQueue displayEventQueue;
 /* Text Buffer */ 
char text[20]; 

uint8_t hour = 0;
uint8_t min = 0;
uint8_t second = 0;
uint8_t millisecond = 0;

uint16_t last_second = 100; 
bool processedReceivedData = true;
bool sendACK = true;

/****************************Call Back Functions*******************************/
void flip1(){
    pwm.write(1);
}

void flip2(){
    pwm.write(0);
}

void ButtonRight(void)
{
    StartHaptic();
    kw40z_device.ToggleAdvertisementMode();
}

void ButtonLeft(void)
{
    StartHaptic();
    kw40z_device.ToggleAdvertisementMode();
}

void PassKey(void)
{
    StartHaptic();
    strcpy((char *) text,"PAIR CODE");
    oled.TextBox((uint8_t *)text,0,25,95,18);
  
    /* Display Bond Pass Key in a 95px by 18px textbox at x=0,y=40 */
    sprintf(text,"%d", kw40z_device.GetPassKey());
    oled.TextBox((uint8_t *)text,0,40,95,18);
}

void AlertReceived(uint8_t *data, uint8_t length)
{
    StartHaptic();
    processedReceivedData = false;
    sendACK = false;
    
    hour = data[2];
    min = data[3];
    second = data[4];
    millisecond = data[5];
    
    if(second%2 == 0){
        flipper1.attach(&flip2, 2);
        wait(1);
        flipper2.attach(&flip1, 2);
    }
    else{
        flipper1.attach(&flip1, 2);
        wait(1);
        flipper2.attach(&flip2, 2);
    }
    
    double temp_time = hour*3600+min*60+second;
    set_time(temp_time);
    //printf("%s\n\r", data);
    displayEventQueue.call(&displayString);
}
/***********************End of Call Back Functions*****************************/

/********************************Main******************************************/

int main()
{    
    /* Register callbacks to application functions */
    kw40z_device.attach_buttonLeft(&ButtonLeft);
    kw40z_device.attach_buttonRight(&ButtonRight);
    kw40z_device.attach_passkey(&PassKey);
    kw40z_device.attach_alert(&AlertReceived);
    flipper1.attach(&flip1, 2);
    wait(1);
    flipper2.attach(&flip2, 2);
    /* Turn on the backlight of the OLED Display */
    oled.DimScreenON();
    
    /* Fills the screen with solid black */         
    oled.FillScreen(COLOR_BLACK);

    /* Get OLED Class Default Text Properties */
    oled_text_properties_t textProperties = {0};
    oled.GetTextProperties(&textProperties);    
        
    /* Change font color to Blue */ 
    textProperties.fontColor   = COLOR_BLUE;
    oled.SetTextProperties(&textProperties);
    
    /* Display Bluetooth Label at x=17,y=65 */ 
    strcpy((char *) text,"BLUETOOTH");
    oled.Label((uint8_t *)text,17,65);
    
    /* Change font color to white */ 
    textProperties.fontColor   = COLOR_WHITE;
    textProperties.alignParam = OLED_TEXT_ALIGN_CENTER;
    oled.SetTextProperties(&textProperties);
    
    /* Display Label at x=22,y=80 */ 
    strcpy((char *) text,"Tap Below");
    oled.Label((uint8_t *)text,22,80);
         
    //uint8_t prevLinkState = 0; 
    //uint8_t currLinkState = 0;
    displayThread.start(callback(&displayEventQueue, &EventQueue::dispatch_forever));
    displayEventQueue.call(&clearScreen);
    
    txThread.start(txTask); /*Start transmitting Sensor Tag Data */
    
    while (true) 
    {
        blueLed = !kw40z_device.GetAdvertisementMode(); /*Indicate BLE Advertisment Mode*/   
        Thread::wait(50);
        time_t seconds = time(NULL);
        if(seconds != last_second || seconds == 100){
            last_second = seconds;
            pc.printf("Second is %d, PwmOut is %d\n", seconds, pwm.read());
        }
    }
    // specify period first, then everything else
}

/******************************End of Main*************************************/


/* txTask() transmits the sensor data */
void txTask(void){  
   while (!sendACK) 
   {
        sendACK = true;
        /*Notify Hexiwear App that it is running Sensor Tag mode*/
        kw40z_device.SendSetApplicationMode(GUI_CURRENT_APP_SENSOR_TAG);
                
        /*The following is sending dummy data over BLE. Replace with real data*/
        
        /*Send Msg*/
        uint8_t buf[9] = "Received";
        kw40z_device.SendAlert(buf, 8);
        Thread::wait(1);                 
    }
}

void displayString() {
    if (!processedReceivedData) {
        clearScreen();
        
        processedReceivedData = true;
        oled_text_properties_t textProperties = {0};
        oled.GetTextProperties(&textProperties);
        
        textProperties.fontColor   = COLOR_BLUE;
        oled.SetTextProperties(&textProperties);
        
        sprintf(text, "%d:%d:%d.%d",hour,min,second,millisecond);
        //sprintf(text, "Time is: ");
        oled.Label((uint8_t*)text,20,45);
    }
}

void StartHaptic(void)  {
    hapticTimer.start(50);
    haptic = 1;
}

void StopHaptic(void const *n) {
    haptic = 0;
    hapticTimer.stop();
}

void clearScreen() {    
    oled.FillScreen(COLOR_BLACK);
}