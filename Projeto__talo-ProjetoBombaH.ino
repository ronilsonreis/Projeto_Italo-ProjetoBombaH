const int pinoRele = 8; //PINO DIGITAL UTILIZADO PELO MÓDULO RELÉ
const int sensorTensao = A2; //PINO ANALÓGICO UTILIZADO PELO SENSOR DE TENSÃO AC

int status = 0; //VARIÁVEL QUE CONTROLA O STATUS DO MÓDULO RELÉ (LIGADO / DESLIGADO)
String statusLamp = "DESLIGADA"; //VARIÁVEL PARA CONTROLE DE STATUS MOSTRADO NO MONITOR SERIAL

void setup(){
  pinMode(pinoRele, OUTPUT); //DEFINE A PORTA COMO SAÍDA
  pinMode(sensorTensao, INPUT); //DEFINE A PORTA COMO ENTRADA
  digitalWrite(pinoRele, LOW); //MÓDULO RELÉ INICIA DESLIGADO
  Serial.begin(9600); //INICIALIZA A SERIAL
}
   
void loop(){

verificaStatusLamp(); //CHAMA A FUNÇÃO RESPONSÁVEL POR VERIFICAR SE A LÂMPADA ESTÁ ACESA OU APAGADA

char c = Serial.read(); //VARIÁVEL RESPONSÁVEL POR RECEBER O CARACTERE DIGITADO NA JANELA SERIAL

  if (c == 'a'){ //SE CARACTER DIGITADO FOR IGUAL A "a", FAZ
    if (status == 0){ //SE VARIÁVEL FOR IGUAL A 0, FAZ
      digitalWrite(pinoRele, HIGH); //RELÉ ACIONADO (LIGADO)
      status = 1; //VARIÁVEL RECEBE O VALOR 1
    }else{ //SENÃO, FAZ
      digitalWrite(pinoRele, LOW); //RELÉ ACIONADO (DESLIGADO)
      status = 0; //VARIÁVEL RECEBE O VALOR 0
    }
  }
  Serial.print("LAMPADA: "); //IMPRIME O TEXTO NO MONITOR SERIAL
  Serial.println(statusLamp); //IMPRIME NO MONITOR SERIAL O ESTADO ATUAL DA LÂMPADA
}
//FUNÇÃO RESPONSÁVEL POR VERIFICAR SE A LÂMPADA ESTÁ ACESA OU APAGADA
void verificaStatusLamp(){
  for (int i = 0; i < 100; i++){ //PARA "i" IGUAL A 0, ENQUANTO "i" MENOR QUE 100, INCREMENTA "i"
    if(analogRead(sensorTensao) > 70){ //SE LEITURA FOR MAIOR QUE 70, FAZ
        statusLamp = "LIGADA"; //VARIÁVEL RECEBE O TEXTO
    }else{ //SENÃO
      statusLamp = "DESLIGADA"; //VARIÁVEL RECEBE O TEXTO
    }
  }
}
