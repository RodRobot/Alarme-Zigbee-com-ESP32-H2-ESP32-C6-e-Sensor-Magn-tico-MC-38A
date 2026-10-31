
/*
  ============================================================
       ESP32-H2 + SENSOR MAGNETICO + LED + ZIGBEE
       RODROBOT - ALARME DE PORTA
  ============================================================

  FUNCAO DO ESP32-H2:
  - Atua como coordenador Zigbee.                // Coordenador
  - Le o sensor magnetico MC-38A no GPIO 4.      // Entrada
  - Utiliza INPUT_PULLUP.                         // Pull-up interno
  - Controla um LED indicador no GPIO 13.        // Indicacao local
  - Possui um endpoint ZigbeeSwitch.              // Binding
  - Envia comandos ON/OFF ao ESP32-C6.            // Alarme remoto

  PORTA FECHADA:
  - Sensor conectado ao GND.
  - GPIO 4 = LOW.
  - LED GPIO 13 apagado.
  - Envia OFF ao ESP32-C6.

  PORTA ABERTA:
  - Contato do sensor aberto.
  - GPIO 4 = HIGH.
  - LED GPIO 13 aceso.
  - Envia ON ao ESP32-C6.

  ============================================================
*/

#include <Arduino.h>

#ifndef ZIGBEE_MODE_ZCZR
#error "Selecione Zigbee ZCZR em Tools -> Zigbee mode"
#endif

#include "Zigbee.h"


/*
  ============================================================
                    CONFIGURACAO
  ============================================================
*/

#define SENSOR_PIN 4                       // GPIO do MC-38A
#define LED_PIN 13                          // LED indicador local

#define SWITCH_ENDPOINT 1                  // Endpoint Zigbee


/*
  Cria o endpoint ZigbeeSwitch.
*/

ZigbeeSwitch zbSwitch(SWITCH_ENDPOINT);


/*
  ============================================================
                    VARIAVEIS
  ============================================================
*/

/*
  Estado anterior do sensor.

  LOW  = porta fechada
  HIGH = porta aberta
*/

bool lastSensorState = LOW;


/*
  ============================================================
                         SETUP
  ============================================================
*/

void setup()
{
  Serial.begin(115200);


  /*
    Configura o sensor com pull-up interno.

    Porta fechada:
    GPIO 4 = LOW

    Porta aberta:
    GPIO 4 = HIGH
  */

  pinMode(SENSOR_PIN, INPUT_PULLUP);


  /*
    Configura o LED indicador como saida.
  */

  pinMode(LED_PIN, OUTPUT);


  /*
    Inicia o LED apagado.
  */

  digitalWrite(LED_PIN, LOW);


  /*
    Configura o binding para um dispositivo receptor.
  */

  zbSwitch.allowMultipleBinding(false);


  /*
    Define fabricante e modelo.
  */

  zbSwitch.setManufacturerAndModel(
    "RodRobot",
    "ESP32-H2-Alarm-Coordinator"
  );


  /*
    Adiciona o endpoint Switch a rede Zigbee.
  */

  Zigbee.addEndpoint(&zbSwitch);


  /*
    Abre a rede durante 180 segundos para permitir
    a entrada e o binding do ESP32-C6.
  */

  Zigbee.setRebootOpenNetwork(180);


  /*
    ==========================================================
                    INICIA O COORDENADOR
    ==========================================================
  */

  Serial.println();
  Serial.println("Iniciando coordenador Zigbee...");


  if (!Zigbee.begin(ZIGBEE_COORDINATOR))
  {
    Serial.println("Falha ao iniciar Zigbee!");

    ESP.restart();
  }


  Serial.println();
  Serial.println("======================================");
  Serial.println(" ESP32-H2 - COORDENADOR ZIGBEE");
  Serial.println("======================================");

  Serial.println("Aguardando ESP32-C6...");
  Serial.println("Aguardando Binding Zigbee...");


  /*
    Aguarda ate existir um dispositivo vinculado.
  */

  while (!zbSwitch.bound())
  {
    Serial.print(".");
    delay(500);
  }


  /*
    ==========================================================
                    BINDING CONCLUIDO
    ==========================================================
  */

  Serial.println();
  Serial.println();
  Serial.println("======================================");
  Serial.println(" BINDING REALIZADO COM SUCESSO!");
  Serial.println("======================================");

  zbSwitch.printBoundDevices(Serial);


  /*
    ==========================================================
                 LEITURA INICIAL DA PORTA
    ==========================================================

    Le o estado real do sensor.

    HIGH = porta aberta: LED aceso e comando ON.
    LOW  = porta fechada: LED apagado e comando OFF.
  */

  lastSensorState = digitalRead(SENSOR_PIN);


  if (lastSensorState == HIGH)
  {
    digitalWrite(LED_PIN, HIGH);           // Acende LED local

    Serial.println("Porta aberta na inicializacao");
    Serial.println("LED indicador: ON");
    Serial.println("Enviando comando ON");

    zbSwitch.lightOn();
  }
  else
  {
    digitalWrite(LED_PIN, LOW);            // Apaga LED local

    Serial.println("Porta fechada na inicializacao");
    Serial.println("LED indicador: OFF");
    Serial.println("Enviando comando OFF");

    zbSwitch.lightOff();
  }


  Serial.println();
  Serial.println("Sistema de alarme pronto.");
}


/*
  ============================================================
                         LOOP
  ============================================================
*/

void loop()
{
  /*
    Le o estado atual do sensor magnetico.
  */

  bool sensorState = digitalRead(SENSOR_PIN);


  /*
    ==========================================================
                 DETECCAO DE MUDANCA NO SENSOR
    ==========================================================

    LOW -> HIGH = porta abriu.
    HIGH -> LOW = porta fechou.
  */

  if (sensorState != lastSensorState)
  {
    /*
      Debounce para evitar leituras instaveis.
    */

    delay(50);


    /*
      Confirma o estado do sensor apos o debounce.
    */

    sensorState = digitalRead(SENSOR_PIN);


    if (sensorState != lastSensorState)
    {
      /*
        Atualiza o estado anterior.
      */

      lastSensorState = sensorState;


      /*
        ======================================================
                       PORTA ABERTA
        ======================================================
      */

      if (sensorState == HIGH)
      {
        digitalWrite(LED_PIN, HIGH);       // Acende LED local

        Serial.println();
        Serial.println("PORTA ABERTA!");
        Serial.println("Sensor: HIGH");
        Serial.println("LED indicador: ON");
        Serial.println("Zigbee -> enviando comando ON");

        zbSwitch.lightOn();
      }


      /*
        ======================================================
                       PORTA FECHADA
        ======================================================
      */

      else
      {
        digitalWrite(LED_PIN, LOW);        // Apaga LED local

        Serial.println();
        Serial.println("PORTA FECHADA!");
        Serial.println("Sensor: LOW");
        Serial.println("LED indicador: OFF");
        Serial.println("Zigbee -> enviando comando OFF");

        zbSwitch.lightOff();
      }
    }
  }


  /*
    Pequeno intervalo entre as leituras.
  */

  delay(10);
}
