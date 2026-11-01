# 🚨 RodRobot – Alarme Zigbee com ESP32-H2 e ESP32-C6

<p align="center">
  <strong>Sistema de monitoramento de portas utilizando Zigbee, sensor magnético e alarme sonoro.</strong>
</p>

## 📌 Sobre o projeto

Este projeto desenvolve um sistema de alarme sem fio para monitoramento de portas utilizando dois microcontroladores da família ESP32: **ESP32-H2 e ESP32-C6**.

O sensor magnético MC-38A identifica a abertura e o fechamento da porta. O ESP32-H2 monitora o sensor e coordena a rede Zigbee, enviando comandos para o ESP32-C6.

Ao receber o comando de abertura, o ESP32-C6 aciona um buzzer e um LED de sinalização. Quando a porta é fechada, o alarme é desligado.

O objetivo é demonstrar, na prática, como utilizar a comunicação Zigbee em projetos de automação residencial e Internet das Coisas (IoT).

## 🎯 Objetivos

- Desenvolver um sistema de alarme sem fio.
- Aprender a configurar uma rede Zigbee com ESP32.
- Utilizar o ESP32-H2 como coordenador.
- Configurar o ESP32-C6 como dispositivo final Zigbee.
- Monitorar uma porta utilizando o sensor magnético MC-38A.
- Controlar buzzer e LED remotamente.
- Explorar possibilidades de expansão para projetos IoT.

## 🧰 Componentes utilizados

| Componente | Função |
|---|---|
| ESP32-H2 SuperMini | Coordenador Zigbee |
| ESP32-C6 DevKit | Dispositivo final Zigbee |
| Sensor MC-38A NA | Detecção da porta |
| Buzzer ativo | Alarme sonoro |
| LED | Sinalização visual |
| Resistores | Limitação de corrente dos LEDs |
| Transistor BC337 | Para acionar o buzzer |
| Protoboard e jumpers | Montagem do circuito |

## 🔌 Ligações dos componentes

### ESP32-H2 – Sensor e indicador

| Componente | GPIO |
|---|---|
| Sensor MC-38A | GPIO 4 |
| LED indicador | GPIO 13 |
| GND do sensor | GND |

O sensor magnético utiliza o resistor de pull-up interno, configurado através de `INPUT_PULLUP`.

**Lógica de funcionamento do sensor:**

| Estado da porta | Leitura GPIO 4 |
|---|---|
| Fechada | LOW |
| Aberta | HIGH |

Quando a porta está fechada, o contato conecta a entrada ao GND. Ao abrir a porta, o contato é interrompido e a entrada passa para HIGH.

### ESP32-C6 – Alarme

| Componente | GPIO |
|---|---|
| Base transitor BC337 | GPIO 13 |
| LED indicador | GPIO 12 |

O buzzer funciona de maneira intermitente, alternando entre ligado e desligado para produzir um sinal sonoro semelhante a uma sirene.

**Atenção:** utilize um transistor ou MOSFET para acionar o buzzer caso sua corrente ultrapasse a capacidade de saída do GPIO.

## 📡 Arquitetura da rede Zigbee

O projeto utiliza dois dispositivos com funções diferentes na rede:

| Dispositivo | Função |
|---|---|
| ESP32-H2 | Coordenador Zigbee |
| ESP32-C6 | End Device Zigbee |

O ESP32-H2 monitora o sensor e envia comandos pela rede Zigbee.

O ESP32-C6 recebe esses comandos e controla as saídas responsáveis pelo alarme.

A comunicação utiliza o mecanismo de binding para associar o dispositivo transmissor ao endpoint receptor.

### Fluxo de funcionamento

```text
       SENSOR MAGNÉTICO MC-38A
                  |
                  |
             ESP32-H2
          COORDENADOR ZIGBEE
                  |
                  |
           REDE ZIGBEE
                  |
                  |
              ESP32-C6
           END DEVICE
                  |
             +----+----+
             |         |
           BUZZER     LED
```

## ⚙️ Configuração do ambiente

O projeto foi desenvolvido utilizando:

- Arduino IDE.
- Arduino-ESP32 versão 3.3.11.
- Pacote de placas ESP32 da Espressif Systems.
- Biblioteca Zigbee integrada ao pacote Arduino-ESP32.

### Configuração do ESP32-H2

No Arduino IDE, selecione a placa ESP32-H2 e configure o modo Zigbee:

- Zigbee Mode: `ZCZR`
- Utilize a configuração de partição compatível com Zigbee.
- Configure a porta COM correspondente à placa.

### Configuração do ESP32-C6

Selecione a placa ESP32-C6 e configure:

- Zigbee Mode: `ED`
- Configure a porta COM correspondente à placa.

**Importante:** os dois dispositivos precisam utilizar modos Zigbee compatíveis com suas funções na rede.

## 🔄 Funcionamento do sistema

O funcionamento acontece da seguinte maneira:

1. O ESP32-H2 inicia a rede Zigbee como coordenador.
2. O ESP32-C6 entra na rede como dispositivo final.
3. O vínculo entre os dispositivos é estabelecido.
4. O ESP32-H2 monitora continuamente o sensor magnético.
5. Quando a porta abre, o H2 envia o comando de acionamento.
6. O ESP32-C6 ativa o buzzer e faz o LED piscar.
7. Quando a porta fecha, o H2 envia o comando de desligamento.
8. O C6 desativa o buzzer e o LED.

## 🧪 Demonstração

O projeto permite observar a comunicação entre dois microcontroladores sem a necessidade de um fio de sinal entre eles.

**Teste 1 – Porta fechada:**
- Sensor em LOW.
- LED do ESP32-H2 apagado.
- Buzzer do ESP32-C6 desligado.
- LED do ESP32-C6 apagado.

**Teste 2 – Porta aberta:**
- Sensor em HIGH.
- LED do ESP32-H2 aceso.
- Comando enviado pela rede Zigbee.
- Buzzer do ESP32-C6 acionado.
- LED do ESP32-C6 piscando.

### 💡 ESP32-C6: Zigbee e Wi-Fi

Uma característica interessante do ESP32-C6 é possuir conectividade IEEE 802.15.4, utilizada pelo Zigbee, além de Wi-Fi 6 e Bluetooth Low Energy.

Neste projeto, o ESP32-C6 está sendo utilizado para receber comandos Zigbee e controlar o alarme.

A utilização do Wi-Fi para encaminhar informações à internet é uma possibilidade de evolução, que ainda precisa ser implementada e testada.

## 📚 Tecnologias utilizadas

- C++ 
- Arduino IDE
- ESP32-H2
- ESP32-C6
- Zigbee
- IEEE 802.15.4
- Bluetooth Low Energy
- Wi-Fi 6
- Sensores digitais
- Automação residencial
- Internet das Coisas (IoT)

## 👨‍🔧 Sobre o RodRobot

O **RodRobot** é um projeto dedicado ao aprendizado prático de eletrônica, programação, microcontroladores, robótica e Internet das Coisas.

A proposta é desenvolver projetos passo a passo, compartilhando experiências, códigos e conhecimentos para makers, estudantes e entusiastas da tecnologia.

## 🎬 Vídeo demonstrativo do projeto

Clique na imagem para assistir ao vídeo completo no YouTube.

<p align="center">
  <a href="https://youtu.be/QshWmU9XEK8" target="_blank">
    <img src="imagens/alarme-zigbee-principal.jpg"
         alt="Assistir ao projeto Alarme Zigbee RodRobot no YouTube"
         width="800">
  </a>
</p>

<p align="center">
  ▶️ <a href="https://youtu.be/QshWmU9XEK8">
    <strong>ASSISTA AO VÍDEO NO YOUTUBE</strong>
  </a>
</p>

🌐 Blog: https://rodrobot.blogspot.com/

📂 Projetos: https://rodrobot.blogspot.com/p/projetos.html

## 📄 Licença

Este projeto é disponibilizado para fins educacionais e experimentais.

Você pode estudar, modificar e adaptar o código, mantendo os créditos de autoria ao RodRobot.

---

<p align="center">
  <strong>RodRobot – Eletrônica, Robótica e IoT na prática!</strong>
</p>
