 #include "EmonLib.h" //INCLUSÃO DE BIBLIOTECA

#define VOLT_CAL 211.6 //VALOR DE CALIBRAÇÃO (DEVE SER AJUSTADO EM PARALELO COM UM MULTÍMETRO)

/*
     CÓDIGO:    R0001
     AUTOR:     Ronilson de Lima Reis
     PROJETO:   BOMBA AUTOMATIZADA 
     DATA:      11/07/2021
*/


// INCLUSÃO DE BIBLIOTECAS
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// DEFINIÇÕES DE PINOS
#define OLED_RESET   -1 // Reset pin # (or -1 if sharing Arduino reset pin)

// DEFINIÇÕES
#define mtIDENTIFICAR 0
#define mtGET         1
#define mtPOST        2
#define mtOUTRO       3

#define SCREEN_WIDTH  128 // OLED display, largura em pixels
#define SCREEN_HEIGHT 32  // OLED display, altura em pixels

// INSTANCIANDO OBJETOS
IPAddress ip(10, 0, 0, 28);
EthernetServer server(80);
EthernetClient client;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DECLARAÇÃO DE VARIÁVEIS
byte mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};     // MAC ADDRES
byte pinCanais[1] = {A0};

EnergyMonitor emon1; //CRIA UMA INSTÂNCIA
String statusTensao = "DESLIGADA"; //VARIÁVEL PARA CONTROLE DE STATUS MOSTRADO NO MONITOR SERIAL

//DECLARAÇÂO DE FUNÇÔES
void setup_Ethernet();
void setup_Voltagem();

void setup_Voltagem(){  
  Serial.begin(9600); //INICIALIZA A SERIAL
  emon1.voltage(2, VOLT_CAL, 1.7); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE)
}


void setup_Ethernet() {
  Serial.begin(115200);

  for (byte nL = 0; nL < 10; nL++) {
    pinMode(pinCanais[nL], OUTPUT);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Endereço 0x3C para o 128x32
    Serial.println(F("SSD1306 falha no modulo"));
    while (true);
  }

  // LIMPA O BUFFER E CONFIGURA O DISPLAY
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.print(F("Configurando a Shield ..."));
  display.display();
  delay(1000); // DELAY PARA EXIBIR MENSAGEM

  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield não encontrada. verifique o hardware :( ");
    display.clearDisplay();
    display.print("Shield não encontrada");
    display.display();

    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Cabo desconectado.");
    display.clearDisplay();
    display.print("Cabo desconectado.");
    display.display();
  }

  server.begin();
  Serial.print("IP do servidor: ");
  Serial.println(Ethernet.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("IP: "));
  display.print(Ethernet.localIP());
  display.display();

}

void loop(){
  emon1.calcVI(17,2000); //FUNÇÃO DE CÁLCULO (17 SEMICICLOS, TEMPO LIMITE PARA FAZER A MEDIÇÃO)    
  
  float supplyVoltage   = emon1.Vrms; //VARIÁVEL RECEBE O VALOR DE TENSÃO RMS OBTIDO

  Serial.print("TENSÃO"); //IMPRIME O TEXTO NO MONITOR SERIAL
  Serial.println(statusTensao); //IMPRIME NO MONITOR SERIAL O ESTADO ATUAL DA LÂMPADA
  Serial.print("  "); //IMPRIME O TEXTO NO MONITOR SERIAL

  Serial.print("Tensão medida na rede AC: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(supplyVoltage, 0); //IMPRIME NA SERIAL O VALOR DE TENSÃO MEDIDO E REMOVE A PARTE DECIMAL
  Serial.println("V"); //IMPRIME O TEXTO NA SERIAL
  delay(1000);
  
  if((supplyVoltage) > 70){
    
      statusTensao = "LIGADA"; //VARIÁVEL RECEBE O TEXTO
      
    }else{
      
      statusTensao = "DESLIGADA"; //VARIÁVEL RECEBE O TEXTO
      
     }  
   }
