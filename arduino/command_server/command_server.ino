#include <EtherCard.h>
#include "pitches.h"

#define LED_RED_PIN 4
#define LED_GREEN_PIN 5
#define SPEAKER_PIN 6
#define MOTOR_1_PWM 7
#define MOTOR_1_PIN_1 8
#define MOTOR_1_PIN_2 9

uint8_t Ethernet::buffer[700]; // configure buffer size to 700 octets
static uint8_t mymac[] = { 0xBE,0xC2,0x43,0xC0,0xEC,0x04 }; // define (unique on LAN) hardware (MAC) address

static byte server_ip[] = { 192,168,3,2 };
static BufferFiller bfill;  // used as cursor while filling the buffer
const char ok_header[] PROGMEM =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n";


void m1_backward(int time) {
  analogWrite(MOTOR_1_PWM, 150);
  digitalWrite(MOTOR_1_PIN_1, HIGH);
  digitalWrite(MOTOR_1_PIN_2, LOW);
  delay(time);
  analogWrite(MOTOR_1_PWM, 0);
  digitalWrite(MOTOR_1_PIN_1, LOW);
  digitalWrite(MOTOR_1_PIN_2, LOW);
}

void m1_forward(int time) {
  analogWrite(MOTOR_1_PWM, 150);
  digitalWrite(MOTOR_1_PIN_1, LOW);
  digitalWrite(MOTOR_1_PIN_2, HIGH);
  delay(time);
  analogWrite(MOTOR_1_PWM, 0);
  digitalWrite(MOTOR_1_PIN_1, LOW);
  digitalWrite(MOTOR_1_PIN_2, LOW);
}

void m1_stop() {
  analogWrite(MOTOR_1_PWM, 0);
  digitalWrite(MOTOR_1_PIN_1, LOW);
  digitalWrite(MOTOR_1_PIN_2, LOW);
}

void setup() {
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(MOTOR_1_PWM, OUTPUT);
  analogWrite(MOTOR_1_PWM, 0);
  pinMode(MOTOR_1_PIN_1, OUTPUT);
  digitalWrite(MOTOR_1_PIN_1, LOW);
  pinMode(MOTOR_1_PIN_2, OUTPUT);
  digitalWrite(MOTOR_1_PIN_2, LOW);


  Serial.begin(9600);
  Serial.println("Network setup BEGIN");
  uint8_t vers = ether.begin(sizeof Ethernet::buffer, mymac);
  if(vers == 0)
  {
      // handle failure to initiate network interface
      Serial.println("Failed to access Ethernet controller");
  }
  Serial.println("Setting up DHCP");
  if(!ether.dhcpSetup())
  {
      // handle failure to obtain IP address via DHCP
      Serial.println("DHCP failed");
  }
  ether.printIp("IP:   ", ether.myip); // output IP address to Serial
  ether.printIp("GW:   ", ether.gwip); // output gateway address to Serial
  ether.printIp("Mask: ", ether.netmask); // output netmask to Serial
  ether.printIp("DHCP server: ", ether.dhcpip); // output IP address of the DHCP server
  Serial.println("Network setup DONE");

  memcpy(ether.hisip, server_ip, sizeof(server_ip));
  ether.printIp("Operation server: ", ether.hisip);
}

void loop() {
  word pos = ether.packetLoop(ether.packetReceive());
  char console_buf[81];

  if (pos) {
    bfill = ether.tcpOffset();
    char* data = (char *) Ethernet::buffer + pos;
    console_buf[0] = '\0';
    strncat(console_buf, data, min(strpbrk(data, "\r\n") - data, 80));
    Serial.println(console_buf);

    if (strncmp("GET / ", data, 6) == 0) {
      Serial.println(data);
      bfill.emit_p(PSTR("$F\r\n"), ok_header);
    } else if (strncmp("POST /operator/led_on", data, 21) == 0) {
      digitalWrite(LED_GREEN_PIN, HIGH);
      digitalWrite(LED_RED_PIN, HIGH);
      bfill.emit_p(PSTR("$F\r\n"), ok_header);
    } else if (strncmp("POST /operator/led_off", data, 22) == 0) {
      digitalWrite(LED_GREEN_PIN, LOW);
      digitalWrite(LED_RED_PIN, LOW);
      bfill.emit_p(PSTR("$F\r\n"), ok_header);
    } else if (strncmp("POST /operator/vtykatel_in", data, 26) == 0) {
      tone(SPEAKER_PIN, 2000, 1000);
      m1_forward(100);
      bfill.emit_p(PSTR("$F\r\n"), ok_header);
    } else if (strncmp("POST /operator/vtykatel_out", data, 27) == 0) {
      tone(SPEAKER_PIN, 1000, 1000);
      m1_backward(100);
      bfill.emit_p(PSTR("$F\r\n"), ok_header);
    } else if (strncmp("POST /operator/play", data, 19) == 0) {
      char number_str[21];
      char* p1 = strstr(data, "play") + 5;
      char* p2 = strchr(p1, '/');
      char* p3 = strchr(p2, ' ');
      number_str[0] = '\0';
      int note = atoi(strncat(number_str, p1, min(p2 - p1, 20)));
      number_str[0] = '\0';
      int duration = atoi(strncat(number_str, p2+1, min(p3 - p2, 20)));
      tone(SPEAKER_PIN, note, duration);
      bfill.emit_p(PSTR("$F\r\n"), ok_header);
    } else {
      Serial.println(data);
      bfill.emit_p(PSTR(
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>400 Bad Request</h1>"));
    }
    ether.httpServerReply(bfill.position()); // send web page data
  }
  delay(10);
}
