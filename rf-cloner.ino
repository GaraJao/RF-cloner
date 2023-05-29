#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RCSwitch.h>

#define OLED_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define PRESS_INTERVAL 200

#define LEFT_BTN 11
#define ENTER_BTN 10
#define RIGHT_BTN 12

Adafruit_SSD1306 display(128, 64, &Wire, -1);
RCSwitch transmitter = RCSwitch();
RCSwitch receiver = RCSwitch();

float page = 0;
bool page_changed = false, check_receiver = true;
int menu_index = 0, protocol = 1, bit_length = 32, brute_force_digits = 2;
unsigned long a_code = 0, b_code = 0, previous_time = 0, brute_force_n = 0;

extern const unsigned char menu_navigation[];
extern const unsigned char menu_listen[];
extern const unsigned char menu_send[];
extern const unsigned char menu_brute_force[];

const unsigned char *menu[3] = { menu_listen, menu_send, menu_brute_force };

void setup() {
  Serial.begin(9600);

  pinMode(ENTER_BTN, INPUT_PULLUP);
  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(RIGHT_BTN, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.setTextColor(WHITE);

  receiver.enableReceive(0);
  transmitter.enableTransmit(3);
  // transmitter.setPulseLength(320);

  clearDisplay();
  setPage(0, 0);
}

void loop() {

  if (receiver.available()) {
    unsigned long received_data = receiver.getReceivedValue();

    if (page == 1 && received_data != 0) {

      protocol = receiver.getReceivedProtocol();
      bit_length = receiver.getReceivedBitlength();

      if (check_receiver)
        a_code = received_data;
      else
        b_code = received_data;

      check_receiver = !check_receiver;

      setPage(1, menu_index);
    }

    receiver.resetAvailable();
  }

  int on_state_change = onStateChange();

  if (on_state_change < 2 || page_changed) {
    page_changed = false;

    if (page == 0) {  // Home page
      menuPosition(3, on_state_change);

      clearDisplay();

      for (int i = 0; i < 3; i++) {
        if (menu_index == i) {
          display.drawBitmap(0, 0, menu[i], 128, 64, WHITE);

          if (on_state_change == 0)
            setPage(i + 1, 0);
        }
      }
    } else if (page == 1) {  // Listen signal page
      menuPosition(2, on_state_change);

      clearDisplay();

      display.setTextColor(WHITE);
      display.drawRect(5, 16, 118, 36, WHITE);
      display.setCursor(18, 55);
      display.print("Voltar");
      display.setCursor(74, 55);
      display.print("Resetar");

      if (a_code != 0) {
        display.setTextSize(2);
        display.setCursor(11, 18);
        display.print(a_code);
      }

      if (b_code != 0) {
        display.setTextSize(2);
        display.setCursor(11, 35);
        display.print(b_code);
      }

      switch (menu_index) {
        case 0:
          display.setTextSize(1);
          display.setTextColor(BLACK);
          display.fillRect(5, 53, 59, 16, WHITE);
          display.setCursor(18, 55);
          display.print("Voltar");

          if (on_state_change == 0)
            setPage(0, 0);
          break;
        case 1:
          display.setTextSize(1);
          display.setTextColor(BLACK);
          display.fillRect(64, 53, 59, 16, WHITE);
          display.setCursor(74, 55);
          display.print("Resetar");

          if (on_state_change == 0) {
            a_code = 0;
            b_code = 0;
            check_receiver = true;
            setPage(1, 1);
          }
          break;
      }
    } else if (page == 2) {  // Send signal page
      menuPosition(4, on_state_change);

      clearDisplay();

      display.setTextSize(1);
      display.setTextColor(WHITE);

      display.drawRect(5, 16, 118, 36, WHITE);

      display.setTextSize(2);

      if (a_code != 0) {
        display.setCursor(12, 18);
        display.print(a_code);
      }

      display.setTextColor(WHITE);

      if (b_code != 0) {
        display.setCursor(12, 35);
        display.print(b_code);
      }

      display.setTextSize(1);

      display.setCursor(18, 55);
      display.print("Voltar");

      display.setCursor(74, 55);
      display.print("Rolante");

      switch (menu_index) {
        case 0:
          display.setTextSize(2);
          display.setTextColor(BLACK);

          display.fillRect(5, 16, 118, 18, WHITE);
          if (a_code != 0) {
            display.setCursor(12, 18);
            display.print(a_code);

            if (on_state_change == 0)
              setPage(2.1, 0);
          }

          break;
        case 1:
          display.setTextSize(2);
          display.setTextColor(BLACK);

          display.fillRect(5, 35, 118, 18, WHITE);
          if (b_code != 0) {
            display.setCursor(12, 37);
            display.print(b_code);

            if (on_state_change == 0)
              setPage(2.1, 1);
          }

          break;
        case 2:
          display.setTextSize(1);
          display.setTextColor(BLACK);

          display.fillRect(5, 53, 59, 16, WHITE);
          display.setCursor(18, 55);
          display.print("Voltar");

          if (on_state_change == 0)
            setPage(0, 1);

          break;
        case 3:
          display.setTextSize(1);
          display.setTextColor(BLACK);

          display.fillRect(64, 53, 59, 16, WHITE);
          display.setCursor(74, 55);
          display.print("Rolante");

          if (a_code != 0 && b_code != 0) {
            if (on_state_change == 0)
              setPage(2.1, 3);
          }

          break;
      }

    } else if (page == 2.1) {  // "Sending" page
      clearDisplay();

      menu_index += 2;

      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(15, 32);
      display.print("Enviando");
      display.display();

      transmitter.setProtocol(protocol);

      switch (menu_index) {
        case 0:
          transmitter.send(a_code, bit_length);
          delay(1000);
          break;
        case 1:
          transmitter.send(b_code, bit_length);
          delay(1000);
          break;
        case 3:
          transmitter.send(a_code, bit_length);
          delay(1000);
          transmitter.send(b_code, bit_length);
          delay(1000);
          break;
      }

      setPage(2, menu_index);
    } else if (page == 3) {  // Brute force page
      menuPosition(3, on_state_change);

      clearDisplay();

      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(21, 24);
      display.print("INICIAR");

      display.setTextSize(1);
      display.setCursor(18, 55);
      display.print("Voltar");
      display.setCursor(74, 55);
      display.print("Config");

      switch (menu_index) {
        case 0:
          display.setTextSize(2);
          display.fillRect(10, 17, 108, 28, WHITE);
          display.setTextColor(BLACK);
          display.setCursor(21, 24);
          display.print("INICIAR");
          // brute_force_n = pow(10, brute_force_digits);

          if (on_state_change == 0) {
            brute_force_n = 1;

            for (int i = 0; i < brute_force_digits; i++) 
              brute_force_n *= 10;

            setPage(3.2, 0);
          }

          break;
        case 1:
          display.setTextSize(1);
          display.drawRect(10, 17, 108, 28, WHITE);
          display.fillRect(5, 53, 59, 16, WHITE);
          display.setTextColor(BLACK);
          display.setCursor(18, 55);
          display.print("Voltar");

          if (on_state_change == 0)
            setPage(0, 2);
          break;
        case 2:
          display.setTextSize(1);
          display.drawRect(10, 17, 108, 28, WHITE);
          display.fillRect(64, 53, 59, 16, WHITE);
          display.setTextColor(BLACK);
          display.setCursor(74, 55);
          display.print("Config");

          if (on_state_change == 0)
            setPage(3.1, brute_force_digits);

          break;
      }
    } else if (page == 3.1) {  // Brute force config page
      menuPosition(9, on_state_change);

      clearDisplay();

      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(34, 27);
      display.print("<   >");

      display.setTextSize(1);
      display.setCursor(13, 51);
      display.print("NUMERO DE DIGITOS");

      display.setTextSize(2);

      for (int i = 0; i < 10; i++) {
        if (menu_index == i) {
          display.setCursor(58, 27);
          display.print(i + 1);

          if (on_state_change == 0)
            setPage(3, 2);

          brute_force_digits = i;
          break;
        }
      }
    } else if (page == 3.2) {  // Brute force working page
      if (brute_force_n < pow(10, brute_force_digits) * 10) {
        clearDisplay();

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(2, 24);
        display.print(brute_force_n);

        display.setTextSize(1);
        display.setTextColor(BLACK);

        display.fillRect(34, 53, 60, 28, WHITE);
        display.setCursor(46, 55);
        display.print("Voltar");

        transmitter.send(brute_force_n, bit_length);
        brute_force_n++;
        setPage(3.2, 0);
      }

      if (on_state_change == 0)
        setPage(3, 0);
    }

    display.display();
  }
}

// Call page routing
void setPage(float landing_page, int index) {
  page = landing_page;
  menu_index = index - 2;
  page_changed = true;
}

// It fires when a button is pressed and returns a value right_btn(1), right_btn(-1) and enter_btn(0)
int onStateChange() {
  unsigned long current_time = millis();

  if (digitalRead(RIGHT_BTN) == LOW && current_time - previous_time > PRESS_INTERVAL) {
    previous_time = current_time;
    return 1;
  }

  if (digitalRead(LEFT_BTN) == LOW && current_time - previous_time > PRESS_INTERVAL) {
    previous_time = current_time;
    return -1;
  }

  if (digitalRead(ENTER_BTN) == LOW && current_time - previous_time > PRESS_INTERVAL) {
    previous_time = current_time;
    return 0;
  }

  return 2;
}

// Manage menu position
void menuPosition(int len, int operation) {
  len = len - 1;

  menu_index += operation;

  if (menu_index > len)
    menu_index = 0;
  else if (menu_index < 0)
    menu_index = len;
}

// Clears the display and adds tab navigation
void clearDisplay() {
  display.clearDisplay();
  display.drawBitmap(0, 0, menu_navigation, 128, 64, WHITE);
}
