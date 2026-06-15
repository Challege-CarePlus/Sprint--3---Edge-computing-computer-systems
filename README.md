# Care Plus - Health Tracker IoT

Este repositório contém a documentação, o código-fonte e os esquemas de circuitos do **Care Plus - Health Tracker**, um dispositivo vestível baseado em ESP32 projetado para monitorar métricas vitais de saúde (BPM, SpO2 e Temperatura). Os dados capturados são transmitidos em tempo real via protocolo MQTT para uma infraestrutura robusta baseada em FIWARE, onde são armazenados, contextualizados e disponibilizados para visualização analítica em um painel do Grafana.

Projeto desenvolvido como parte dos requisitos de avaliação da **Sprint 3 - FIAP**.

---

## 👥 Integrantes do Projeto

| Nome | RM |
|------|----|
| Nicolas Forcione de Oliveira e Souza | RM566998|
| Enrico Dellatorre da Fonseca | RM566824 |
| Alexandre Constantino Furtado Junior | RM567188 |
| Leonardo Batista de Souza | RM568558 |
| Matheus Freitas dos Santos | RM56737 |


---

## 🚀 Visão Geral da Arquitetura

A solução utiliza o paradigma de IoT de ponta a ponta, dividida em três camadas principais:

1.  **Edge / Hardware (Coleta e Alerta Local):** Um ESP32 lê sensores analógicos (simulados via potenciômetros de precisão para fins de calibração e teste), processa os limites clínicos locais, exibe as métricas de forma síncrona em uma tela OLED e aciona alertas visuais (LED) e sonoros (Buzzer) caso o paciente entre em estado crítico.
2.  **Broker & Context Server (Mensageria e Persistência):** Os dados são formatados no padrão *Ultralight 2.0* e publicados via MQTT na porta `1883` gerenciada por um broker **Eclipse Mosquitto**. O **FIWARE IoT Agent Ultralight** assina o tópico, traduz o payload bruto e atualiza os atributos de contexto diretamente no **FIWARE Orion Context Broker** através do MongoDB.
3.  **Visualization Layer (Monitoramento em Runtime):** O **Grafana** consome os dados em tempo real diretamente das APIs REST do Orion Context Broker, utilizando o plugin `marcusolsson-json-datasource` configurado com tempo de cache zerado (`0s`) para evitar latência e garantir a exibição instantânea das flutuações das métricas vitais.

---

## 🛠️ Especificações do Hardware e Pinagem

O circuito foi projetado sobre uma matriz de contatos (Protoboard) alimentada diretamente pelo regulador interno de `3.3V` do ESP32 para mitigar ruídos de leitura de conversão analógico-digital (ADC).

| Componente | Pino ESP32 | Tipo de Sinal | Função |
| :--- | :--- | :--- | :--- |
| **Potenciômetro BPM** | `GPIO 34` | Analógico (ADC6) | Simulação de Batimentos por Minuto (50 a 180 BPM) |
| **Potenciômetro SpO2** | `GPIO 35` | Analógico (ADC7) | Simulação de Saturação de Oxigênio (80% a 100%) |
| **Potenciômetro Temp** | `GPIO 32` | Analógico (ADC4) | Simulação de Temperatura Corporal (34.0°C a 41.0°C) |
| **Display OLED SSD1306** | `GPIO 21 (SDA)` / `GPIO 22 (SCL)` | Digital (I2C) | Interface visual de telemetria local |
| **LED Vermelho** | `GPIO 33` | Digital (Output) | Indicador visual de anomalia médica |
| **Buzzer Piezoelétrico** | `GPIO 25` | Digital (PWM / Output) | Indicador sonoro de emergência clínica (1000 Hz) |

---

## 🎥 Demonstração e Simulação

Abaixo você encontra o link para a simulação completa do circuito operando perfeitamente e o vídeo de apresentação do projeto em tempo real.

* **Simulação Interativa (Wokwi):** [Care Plus - Simulador Wokwi](https://wokwi.com/projects/462510376052402177)
* **Vídeo de Apresentação (YouTube):** [Insira o link do seu vídeo aqui]

---

## ⚙️ Estrutura do Repositório

* `sketch.ino`: Contém o firmware completo desenvolvido em C++ para o ESP32.
* `docker-compose.yml`: Contém a infraestrutura de nuvem (Orion, IoT Agent, Mosquitto, MongoDB) pronta para ser levantada via Docker.

