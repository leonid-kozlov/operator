#include <EtherCard.h>

#define LED_RED_PIN 4
#define LED_GREEN_PIN 5

uint8_t Ethernet::buffer[700]; // configure buffer size to 700 octets
static uint8_t mymac[] = { 0xBE,0xC2,0x43,0xC0,0xEC,0x04 }; // define (unique on LAN) hardware (MAC) address

const char website[] PROGMEM = "192.168.3.2";
static byte server_ip[] = { 192,168,3,2 };
static uint32_t timer;
static BufferFiller bfill;  // used as cursor while filling the buffer
const char ok_header[] PROGMEM =
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n";

const char index[] PROGMEM =
    "<h1>Help:</h1>"
    "<p>echo: echo text</p>"
    "<p>led: change led state</p>"
    "<form action=\"/\" method=\"POST\"><input type=\"submit\" name=\"led\" value=\"sw\" /></form>";

void setup() {
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
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
  char echo_str[300];
  uint8_t echo_len;

  if (pos) {
    bfill = ether.tcpOffset();
    char* data = (char *) Ethernet::buffer + pos;
    if (strncmp("GET / ", data, 6) == 0) {
      bfill.emit_p(PSTR("$F\r\n"), index, ok_header);
    } else if (strncmp("GET /echo/", data, 10) == 0) {
      Serial.println(data);
      echo_len = strchr(data + 4, ' ') - data - 10;
      echo_str[0] = 0;
      strncat(echo_str, data+10, echo_len);
      // memcpy(echo_str+4, data + 10, echo_len);
      // memcpy(echo_str+4+echo_len, "</h1>", 5);
      bfill.emit_p(PSTR("$F\r\n<h1>ECHO $D bytes: $S</h1>"), ok_header, echo_len, echo_str);
    } else if (strncmp("POST /", data, 5) == 0) {
      Serial.println(data);
      if (digitalRead(LED_GREEN_PIN) == LOW) {
        digitalWrite(LED_GREEN_PIN, HIGH);
        digitalWrite(LED_RED_PIN, HIGH);
      } else {
        digitalWrite(LED_GREEN_PIN, LOW);
        digitalWrite(LED_RED_PIN, LOW);
      }
      bfill.emit_p(PSTR("$F\r\n$F\r\n"),ok_header, index);
    } else {
      bfill.emit_p(PSTR(
        "HTTP/1.0 400 Bad Request\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>400 Very Bad Request</h1>"));
    }
    ether.httpServerReply(bfill.position()); // send web page data
  }
}
