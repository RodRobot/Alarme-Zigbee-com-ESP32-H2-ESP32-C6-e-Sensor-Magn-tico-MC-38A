
/*
  ============================================================
       ESP32-C6 + ZIGBEE + BUZZER + LED
       RODROBOT - ALARME DE PORTA
  ============================================================

  FUNCAO DO ESP32-C6:
  - Atua como END DEVICE Zigbee.                 // Receptor
  - Entra na rede criada pelo ESP32-H2.          // Coordenador
  - Utiliza o endpoint ZigbeeLight.              // Binding
  - Recebe comandos ON/OFF pela rede Zigbee.     // Controle
  - Controla um buzzer ativo no GPIO 13.         // Sirene
  - Controla um LED externo no GPIO 12.          // Sinalizacao

  FUNCIONAMENTO:

  PORTA ABERTA:
  ESP32-H2 envia ON
        |
        v
  ESP32-C6 recebe ON
        |
        v
  BUZZER alterna HIGH / LOW
  LED alterna HIGH / LOW
        |
        v
  SIRENE E LED ATIVADOS


  PORTA FECHADA:
  ESP32-H2 envia OFF
        |
        v
  ESP32-C6 recebe OFF
        |
        v
  BUZZER = LOW
  LED = LOW
        |
        v
  ALARME DESLIGADO

  ============================================================
*/

#include <Arduino.h>

#ifndef ZIGBEE_MODE_ED
#error "Selecione Zigbee ED em Tools -> Zigbee mode"
#endif

#include "Zigbee.h"


/*
  ============================================================
                    CONFIGURACAO
  ============================================================
*/

#define BUZZER_PIN 13                     // GPIO do buzzer ativo
#define LED_PIN 12                        // GPIO do LED externo

#define LIGHT_ENDPOINT 10                 // Endpoint Zigbee


/*
  ============================================================
                TEMPORIZACAO DA SIRENE
  ============================================================
*/

const unsigned long SIREN_INTERVAL = 250;  // Intervalo em ms


/*
  Cria o endpoint ZigbeeLight.
  Mantem compatibilidade com o coordenador.
*/

ZigbeeLight zbLight(LIGHT_ENDPOINT);


/*
  ============================================================
                    VARIAVEIS
  ============================================================
*/

/*
  Estado do alarme recebido por Zigbee.

  false = alarme desligado
  true  = alarme ativado
*/

volatile bool alarmActive = false;


/*
  Estado atual do buzzer e do LED.

  LOW  = desligado
  HIGH = ligado
*/

bool buzzerState = LOW;


/*
  Armazena o instante da ultima alternancia.
*/

unsigned long previousMillis = 0;


/*
  ============================================================
                 FUNCAO DE CONTROLE ZIGBEE
  ============================================================
*/

void setBuzzer(bool state)
{
  /*
    Esta funcao e chamada quando o ESP32-C6 recebe
    um comando de alteracao do endpoint ZigbeeLight.
  */

  if (state)
  {
    /*
      ========================================================
                       ALARME ATIVADO
      ========================================================
    */

    alarmActive = true;                   // Ativa a sirene

    Serial.println("Zigbee recebido: ON");
    Serial.println("ALARME ATIVADO - BUZZER E LED");
  }
  else
  {
    /*
      ========================================================
                       ALARME DESATIVADO
      ========================================================
    */

    alarmActive = false;                  // Desativa a sirene
    buzzerState = LOW;                    // Define estado desligado

    digitalWrite(BUZZER_PIN, LOW);        // Silencia o buzzer
    digitalWrite(LED_PIN, LOW);           // Apaga o LED

    Serial.println("Zigbee recebido: OFF");
    Serial.println("ALARME DESATIVADO");
  }
}


/*
  ============================================================
                         SETUP
  ============================================================
*/

void setup()
{
  Serial.begin(115200);


  /*
    Configura o buzzer e o LED como saidas.
  */

  pinMode(BUZZER_PIN, OUTPUT);            // Saida do buzzer
  pinMode(LED_PIN, OUTPUT);               // Saida do LED


  /*
    Inicia o alarme em silencio e com o LED apagado.
  */

  digitalWrite(BUZZER_PIN, LOW);          // Buzzer desligado
  digitalWrite(LED_PIN, LOW);             // LED apagado


  /*
    Define fabricante e modelo.
  */

  zbLight.setManufacturerAndModel(
    "RodRobot",
    "ESP32-C6-Zigbee-Alarm"
  );


  /*
    Registra a funcao que recebe comandos ON/OFF.
  */

  zbLight.onLightChange(setBuzzer);


  /*
    Adiciona o endpoint Zigbee.
  */

  Serial.println("Adicionando endpoint ZigbeeLight...");

  Zigbee.addEndpoint(&zbLight);


  /*
    ==========================================================
                    INICIA O END DEVICE
    ==========================================================
  */

  Serial.println("Iniciando ESP32-C6 como End Device...");


  if (!Zigbee.begin())
  {
    Serial.println("Falha ao iniciar Zigbee!");
    Serial.println("Reiniciando...");

    ESP.restart();
  }


  /*
    ==========================================================
                    AGUARDA CONEXAO
    ==========================================================
  */

  Serial.println("Aguardando entrada na rede Zigbee...");

  while (!Zigbee.connected())
  {
    Serial.print(".");
    delay(500);
  }


  /*
    ==========================================================
                    REDE CONECTADA
    ==========================================================
  */

  Serial.println();
  Serial.println();
  Serial.println("======================================");
  Serial.println(" ESP32-C6 CONECTADO A REDE ZIGBEE");
  Serial.println("======================================");

  Serial.println("Buzzer: GPIO 13");
  Serial.println("LED: GPIO 12");
  Serial.println("Aguardando comandos do ESP32-H2...");
}


/*
  ============================================================
                         LOOP
  ============================================================
*/

void loop()
{
  /*
    Obtem o tempo atual sem bloquear o processamento Zigbee.
  */

  unsigned long currentMillis = millis();


  /*
    ==========================================================
                    SIRENE ATIVADA
    ==========================================================

    Enquanto o alarme estiver ativo, alterna o buzzer
    e o LED simultaneamente a cada 250 milissegundos.
  */

  if (alarmActive)
  {
    if (currentMillis - previousMillis >= SIREN_INTERVAL)
    {
      previousMillis = currentMillis;      // Atualiza temporizacao

      buzzerState = !buzzerState;          // Inverte HIGH / LOW

      digitalWrite(BUZZER_PIN, buzzerState); // Alterna buzzer
      digitalWrite(LED_PIN, buzzerState);    // LED acompanha buzzer
    }
  }
  else
  {
    /*
      ========================================================
                    SIRENE DESLIGADA
      ========================================================
    */

    buzzerState = LOW;                     // Define estado desligado

    digitalWrite(BUZZER_PIN, LOW);         // Garante silencio
    digitalWrite(LED_PIN, LOW);            // Garante LED apagado
  }
}
