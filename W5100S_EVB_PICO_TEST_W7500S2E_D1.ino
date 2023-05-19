#include <SPI.h>
#include <Ethernet.h>
//#include <TimeLib.h>  

byte mac[] = { 0x00, 0x08, 0xdc, 0x11, 0x22, 0x33 };  // Your MAC Address
IPAddress localIP(192, 168, 1, 254);                  // Your IP Address
IPAddress broadcastIp(255, 255, 255, 255);            // Broadcast address

EthernetUDP udp;
EthernetServer server(80);

uint8_t count = 0;
unsigned long startTime;

#pragma pack(1) 

struct S2E_DATA {
  char      SERH[4];
  uint8_t   MAC[6];
  uint8_t   DEVICE_ID[16];
  uint8_t   STATE;
  uint8_t   SW_VER[2];
  char      DEVICE_TYPE[16];
  char      DEVICE_NAME[16];
  char      DEVICE_PWD[16];
  uint16_t  HTTP_PORT;
  uint32_t  FW_LEN;
  uint32_t  FW_CHECKSUM;
  uint8_t   DEBUG_FLAG;
  uint8_t   ECHO_FLAG;
  uint8_t   NETBIOS;
  uint8_t   DHCP;
  uint8_t   LOCAL_IP[4];
  uint8_t   SUBNET[4];
  uint8_t   GATEWAY[4];
  uint8_t   DNS[4];
  uint8_t   BAUDRATE;
  uint8_t   DATABIT;
  uint8_t   PARITY;
  uint8_t   STOPBIT;
  uint8_t   FLOWCONTROL;
  uint8_t   MODE;
  uint16_t  LOCAL_PORT;
  uint8_t   REMOTE_IP[4];
  uint16_t  REMOTE_PORT;
  uint8_t   DNS_FLAG;
  char      DOMAIN[32];
  uint16_t  INACTIVITY;
  uint16_t  RECONNECT;
  uint16_t  NAGLE_TIME;
  uint16_t  PACK_LEN;
  uint16_t  KEEP_ALIVE;
  uint8_t   BIND_PORT;
  uint8_t   LINK_CLEAR_BUF;
  uint8_t   LINK_PWD;
  uint8_t   LINK_MSG;
  uint8_t   LINK_CONDITION;
  uint8_t   START_MODE;
  uint16_t  CRC16;
  uint32_t  NET_SEND_NUM;
  uint32_t  NET_RCV_NUM;
  uint32_t  SEC_SEND_NUM;
  uint32_t  SEC_RCV_NUM;
  uint32_t  ON_TIME;
  uint8_t  RUN_MODE;
  uint8_t  TEMPERATURE;
};

S2E_DATA s2eData;
S2E_DATA s2eDataArray[16];

void setup() {
  Serial.begin(115200);
//  while (!Serial) {
//    delay(100);
//  }
  Ethernet.init(17);  // WIZnet W5100S-EVB-Pico
  Ethernet.begin(mac, localIP);
  
  server.begin(); // start the server
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  udp.begin(8888); 
  Serial.println("Broadcast test");  
}

void loop() {
  count = 0;
  sendBroadcastPacket();
  printReceivedPackets();
  startTime = millis();
  while (millis() - startTime < 3000) 
  {
    EthernetClient client = server.available();
    if (client) {
    processRequest(client);
    client.stop();
    }  
  }
}

void sendBroadcastPacket() {
  char data[] = "SERH";                 // Discovery String
  udp.beginPacket(broadcastIp, 1460);   // open udp socket
  udp.write(data);                      // Write your data to the packet
  udp.endPacket();                      // end 
  Serial.println("Discovery packet sent.");
}

void printReceivedPackets() {
  startTime = millis();
  while (millis() - startTime < 500) {
    int packetSize = udp.parsePacket();
    if (packetSize) {
      int len = udp.read((byte*)&s2eData, sizeof(s2eData) - 1);
      if (len > 0) {        
        for(uint8_t m=0; m<16; m++)
        {
          if((s2eDataArray[m].MAC[3] != s2eData.MAC[3])||(s2eDataArray[m].MAC[4] != s2eData.MAC[4])||(s2eDataArray[m].MAC[5] != s2eData.MAC[5]))
          {  
            memcpy(&s2eDataArray[count], &s2eData, sizeof(s2eData));             
            Serial.print("Module NO.");
            Serial.print(count+1);
            Serial.print(" [");
            Serial.print(s2eDataArray[count].DEVICE_TYPE);
            Serial.print("] ");
            Serial.print("[MAC]:");
            for (int i = 0; i < 6; i++) {
              Serial.printf("%02X",s2eDataArray[count].MAC[i]);
              if(i<5)
              {
                Serial.print(":");
              }else{
                Serial.print(" ");
                break;
              }
            }
            Serial.print("[IP]:");
            for (int n = 0; n < 4; n++) {
              Serial.print(s2eDataArray[count].LOCAL_IP[n]);
              if(n<3)
              {
                Serial.print(".");
              }else{
                break;
              }
            }
            Serial.print(" [Run Time]: ");
            Serial.print(s2eDataArray[count].ON_TIME/(60*60*24));
            Serial.print("d/");
            Serial.print((s2eDataArray[count].ON_TIME/(60*60))%24);
            Serial.print("h/");
            Serial.print((s2eDataArray[count].ON_TIME/(60))%60);
            Serial.print("m/");
            Serial.print((s2eDataArray[count].ON_TIME)%60);
            Serial.println("s");
            count ++;
            break;
          }          
        }
      }
    }
  }
}

void processRequest(EthernetClient client) {
  String request = client.readStringUntil('\r');
  client.readStringUntil('\n');
  if (request.indexOf("GET /") != -1) {
    sendResponse(client);
  }
}

void sendResponse(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>WIZS2E ConfigTool Webpage</title>");
  client.println("<meta http-equiv=\"refresh\" content=\"3\">"); //Automatic refresh, refresh every 5 seconds
  client.println("<style>");
  client.println(".no-style{ color: black; font-size: 18px; }");
  client.println(".device-style{ color: green; font-size: 14px; }");
  client.println(".mac-style{ color: red; font-size: 14px; }");
  client.println(".ip-style{ color: orange; font-size: 14px; }");
  client.println(".time-style{ color: blue; font-size: 14px; }");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h1>WIZS2E ConfigTool Webpage</h1>");
  for (int i = 0; i < 16; i++) {
    if (s2eDataArray[i].DEVICE_TYPE[0] == '\0') {
      continue;  //skip invalid data
    }    
    client.print("<span class=\"no-style\">Module NO. </span>");    
    client.print(i + 1);
    client.print("<span class=\"device-style\">    [DEVICE TYPE]: </span>");
    client.print(s2eDataArray[i].DEVICE_TYPE);
    client.print("<span class=\"mac-style\">   [MAC]: </span>");
    for (int j = 0; j < 6; j++) {
      client.printf("%02X",s2eDataArray[i].MAC[j]);
      if (j < 5) {
        client.print(":");
      }
    }
    client.print("<span class=\"ip-style\">   [IP]: </span>");
    client.print(s2eDataArray[i].LOCAL_IP[0]);
    client.print(".");
    client.print(s2eDataArray[i].LOCAL_IP[1]);
    client.print(".");
    client.print(s2eDataArray[i].LOCAL_IP[2]);
    client.print(".");
    client.print(s2eDataArray[i].LOCAL_IP[3]);
    client.print("<span class=\"time-style\">   [Run Time]: </span>");
    client.print(s2eDataArray[i].ON_TIME/(60*60*24));
    client.print("d/");
    client.print((s2eDataArray[i].ON_TIME/(60*60))%24);
    client.print("h/");
    client.print((s2eDataArray[i].ON_TIME/(60))%60);
    client.print("m/");
    client.print((s2eDataArray[i].ON_TIME)%60);
    client.print("s");    
    client.print("</p>");
    client.println("<hr>");
  }
  client.println("</body>");
  client.println("</html>");
}
