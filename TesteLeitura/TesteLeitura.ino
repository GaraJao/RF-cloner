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



void lerDados() {
  while (mySwitch.available()) {
    received_data = mySwitch.getReceivedValue();
    mySwitch.resetAvailable();
    
    switch (flag){
    case 0:
      a_code = received_data;
      flag = 1;
      break;
    default:
      if (a_code == received_data){
        continue; // Ignora o valor repetido e continua para a próxima iteração do loop
      }
      b_code = received_data;
      flag = 0;
      break;
    }
  }

  Serial.print("Código A = ");
  Serial.println(a_code);
  Serial.print("Código B = ");
  Serial.println(b_code);
}


void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();
  display.setTextColor(WHITE);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void loop() {
  display.clearDisplay();
  lerDados();
  display.setTextSize(2);
  display.setCursor(5, 1);
  display.print(a_code);
  display.setCursor(5, 30);
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
