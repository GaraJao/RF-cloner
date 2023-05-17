#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RCSwitch.h>

#define OLED_ADDRESS 0x3C // Endereço I2C do display OLED

Adafruit_SSD1306 display(128, 64, &Wire, -1);
RCSwitch mySwitch = RCSwitch();

String received_data = "";
String a_code = "";
String b_code = "";
int flag = 0;

void ler_dados() {
  if (mySwitch.available()) {
    received_data = mySwitch.getReceivedValue();
    mySwitch.resetAvailable();
    
    if (flag == 0) {
      a_code = received_data;
      flag = 1;
    } else if (flag == 1) {
      b_code = received_data;
      flag = 2;
    }
  }
  
  Serial.print("Código A = ");
  Serial.println(a_code);
  Serial.print("Código B = ");
  Serial.println(b_code);
  
  if (flag == 2) {
    flag = 0;
  }
}



void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();
  display.setTextColor(WHITE);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void loop() {
  display.clearDisplay();

  ler_dados();
  
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("A: ");
  display.setTextSize(2);
  display.print(a_code);
  
  display.setCursor(0, 30);
  display.setTextSize(1);
  display.print("B: ");
  display.setTextSize(2);
  display.print(b_code);
  
  display.display();
}
/*
 * teste 1 Código A = 143799829
           Código B = 119195496

   teste 2 Código A = 28238280 
           Código B = 143799829

   teste 3 Código A = 224714664
           Código B = 143799845

   teste 4 Código A = 143799845
           Código B = 66417304

   teste 5 Código A = 143799845
           Código B = 255806712




    




 */
