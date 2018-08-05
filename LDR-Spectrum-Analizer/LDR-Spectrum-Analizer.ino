
#include <Gamer.h>
Gamer gamer;
/*
 * This sketch needs the fht library from open music labs, and the gamer kit library from tech will save us.  
 * the sketch samples the ldr value and calculates 
 * the FHT and outputs the spectrum to the display
 */

#define LOG_OUT 1 // use the log output function
#define FHT_N 16 // set to 16 point fht
#include <FHT.h>
void setup() {
   gamer.begin();
  Serial.begin(115200); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter
}

void loop() {
  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      int k = gamer.ldrValue(); // get signal from ldr
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();
    Serial.write(255); // send a start byte
    //Serial.write(fht_log_out, FHT_N/2); // send out the data
    /*Bucle que te saca todo por serial en decimal
     * 
    for (int i=0 ; i<FHT_N/2 ; i++)
    {
      Serial.print(fht_log_out[i],DEC);
      Serial.print('#');
    }
    Serial.print('\n');
    */
    //Show graphics on display
    updateScreen();
  }
}

void updateScreen(){
  int display[8][8]={ //create a blank display
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  };
  getSpectrum(display);//pass the display to print it
  //copy the display to the gamer's
  for (int column=0;column<8;column++){
    int invRow = 7;//invert rownumber on display so it goes from bottom to top
    for (int row=0;row<8;row++){
        gamer.display[column][row] = display[column][invRow];
        invRow--;
    }
  }
  gamer.updateDisplay();//Update
}
/*
*outputs the fht results into an 8*8 display
*/
int getSpectrum( int display[8][8] ){
  int spectro[8];
  for (int i =0; i < FHT_N/2;i++){
    float value = float(fht_log_out[i])/256;//Get a float value between 0 and 1  
    value *=10;//Multiply it by ten
    spectro[i] = int(value); //transform it to an int and store it
    Serial.println(spectro[i]);
  }
  for (int col=0;col < 8;col++){
    for(int row=0;row<8;row++){
      if (spectro[col] > row){ //if the number on that column of the spectrum is higher than the row
        display[col][row] = 1; //turn on the led on that column and row
      }
    }
  }
  return display;
}
