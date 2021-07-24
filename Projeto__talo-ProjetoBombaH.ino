/* Este código funciona com o sensor de tensão CA ESP8266 12E e ZMPT101B
 * Pode medir o TRMS de qualquer tensão até 250 VAC 50 / 60Hz e enviar os valores para Adafruit MQTT
 */
#include <ESP8266WiFi.h>                         // Bibliotecas necessárias
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Filters.h>

#define WLAN_SSID       "AP-TI"               // Seu SSID WiFi e senha
#define WLAN_PASS       "w!fi@16#"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  

#define AIO_USERNAME  "informatica_italo"
#define AIO_KEY       "aio_ZlGs23V4QB7iQJa13DF2lyu5mi0l"

#define ZMPT101B A0                          //ZMPT101B analog pin
 
int Current_Time=0, Previous_Time=0, Period=10000;  //Enviamos um valor a cada 10s, o máximo é 30 valores por minuto

float testFrequency = 50;                    // frequência do sinal de teste (Hz)
float windowLength = 100/testFrequency;      // quanto tempo para calcular a média do sinal, para estatísticas

int RawValue = 0;


float intercept = 0; // a ser ajustado com base no teste de calibração
float slope = 1;     // a ser ajustado com base no teste de calibração
float Volts_TRMS;    // tensão real estimada em Volts

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Test = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Bomba1");        //My feed name is "Bomba1" so pay attention to yours, remplace it at the name and "/feeds/Test"
RunningStatistics inputStats;

void setup() {
  
  Serial.begin(115200);
  delay(10);
  inputStats.setWindowSecs( windowLength );
  Serial.println();
  Serial.print("Conectando à ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("Wi-Fi conectado");
  Serial.println("Endereço de IP: "); 
  Serial.println(WiFi.localIP());

}

void loop() {
  
  Volts_TRMS=ReadVoltage(); // Lemos a tensão, normalmente a função retorna o valor Volts_TRMS, você pode fazer isso diretamente no Test.publish
                            // Mas esta é a sintaxe que funcionou para mim, retorne um valor para ela mesma: D
  MQTT_connect();           // Mantenha o MQTT conectado
  Current_Time=millis();

  if(Current_Time - Previous_Time >= Period){  // A cada período enviamos o valor para o prestador de serviço
  Serial.print(F("\nEnvio de valor "));         // O valor também é mostrado no monitor serial
  Serial.print(Volts_TRMS);
  Serial.print("...");
  if (! Test.publish(Volts_TRMS)) {
    Serial.println(F("Fracassado"));
  } else {
    Serial.println(F("OK!"));
  }
  Previous_Time = Current_Time;
  
  }

}

void MQTT_connect() {                  // Reconectar automaticamente ao MQTT, caso contrário, ele acionará uma reinicialização do watchdog
  int8_t ret;

  // Pare se já estiver conectado.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Conectando ao MQTT ...");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // conectar retornará 0 para conectado
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Tentando novamente a conexão MQTT em 1 segundo ...");
       mqtt.disconnect();
       delay(1000);  // wait 1 seconds
       retries--;
       if (retries == 0) {
         // basicamente morrer e esperar que o WDT me reinicie
         while (1);
       }
  }
  Serial.println("MQTT conectado!");
}

float ReadVoltage(){
  
      RawValue = analogRead(ZMPT101B);  // leia o valor analógico:
      inputStats.input(RawValue);  // logar na função Stats
     
      Volts_TRMS = inputStats.sigma()* slope + intercept;
     // Volts_TRMS = Volts_TRMS*0.979;

      return Volts_TRMS;
       
}
