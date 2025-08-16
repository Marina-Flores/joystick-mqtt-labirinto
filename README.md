# 🎮 Projeto ESP32 com Joystick e Jogo de Labirinto

## 📖 Descrição do Projeto

Este projeto integra um **ESP32 com joystick analógico** e um **jogo de labirinto em HTML/JS**, utilizando o protocolo **MQTT** para comunicação em tempo real. O joystick conectado ao ESP32 envia comandos (cima, baixo, esquerda, direita) para o broker **Mosquitto**, e o frontend exibe os movimentos em um jogo de labirinto, proporcionando uma experiência interativa de controle remoto.

O objetivo é demonstrar a aplicação de **sistemas embarcados**, **protocolos IoT** e **integração com frontend web** em um ambiente acadêmico.

## 🛠️ Ferramentas Utilizadas

* **ESP32 DevKit V1** (módulo ESP32-WROOM-32D)
* **ESP-IDF** (framework oficial da Espressif)
* **Mosquitto** (broker MQTT)
* **Frontend em HTML + JavaScript** (com MQTT.js)

---

## ⚙️ Sistema Embarcado (ESP32 + Joystick)

O ESP32 lê os valores analógicos do joystick e converte em comandos de movimento (`up`, `down`, `left`, `right`). Esses comandos são enviados via **MQTT** para o tópico configurado.

### Como executar

1. Instale o **ESP-IDF** seguindo a [documentação oficial](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/).
2. Clone este repositório e abra o diretório `esp32_with_joystick/`.
3. Configure o Wi-Fi e o broker MQTT no código fonte (`app_main.c`).
4. Compile e envie o firmware para o ESP32:

   ```bash
   idf.py build
   idf.py -p PORTA_FLASH flash monitor
   ```

O ESP32 começará a publicar mensagens no broker MQTT.

---

## 🌐 Broker MQTT (Mosquitto)

O **Mosquitto** atua como intermediário entre o ESP32 e o frontend.

### Instalação (Windows)

1. Baixe e instale o Mosquitto: [Mosquitto Downloads](https://mosquitto.org/download/)
2. Configure para aceitar conexões locais (porta padrão `1883`) e **WebSocket** (porta `9001`).
   Exemplo de configuração no `mosquitto.conf`:

   ```
   listener 1883
   allow_anonymous true

   listener 9001
   protocol websockets
   ```
3. Inicie o broker:

   ```bash
   mosquitto -c mosquitto.conf
   ```

---

## 🎮 Frontend - Jogo do Labirinto

O frontend consiste em um jogo de labirinto desenvolvido em **HTML + JavaScript**, que se conecta ao broker **MQTT** para receber os comandos enviados pelo joystick no **ESP32**.
Cada movimento publicado no tópico `/ifpe/ads/joystick` faz o personagem se mover no tabuleiro.

### Como executar o frontend

1. Abra o arquivo `index.html` (que está na pasta `frontend/`) em um navegador moderno (Chrome, Edge ou Firefox).
2. No código, edite a variável `brokerUrl` com o endereço do broker MQTT (exemplo: `ws://192.168.1.100:9001`).

   ```javascript
   const brokerUrl = "ws://SEU_IP:9001";
   ```

   ⚠️ É importante que o **Mosquitto** esteja configurado para aceitar conexões **WebSocket** (porta 9001).
3. Assim que o ESP32 começar a publicar mensagens (`up`, `down`, `left`, `right`), o personagem se moverá no labirinto.
4. O jogo possui múltiplos níveis que avançam automaticamente até a vitória final.

### Dependências

O frontend utiliza apenas uma dependência externa:

* [MQTT.js via CDN](https://unpkg.com/mqtt/dist/mqtt.min.js)

Não é necessário instalar nada além do navegador.

---

## 🚀 Estrutura do Repositório

```
📂 joystick-mqtt-labirinto
 ┣ 📂 esp32_with_joystick   → Código fonte do ESP32
 ┣ 📂 frontend              → Código do jogo em HTML/JS
 ┗ 📜 README.md             → Documentação do projeto
```

---

## ✅ Conclusão

Este projeto demonstra com sucesso a integração entre **sistemas embarcados (ESP32)**, **protocolo de comunicação (MQTT)** e **interfaces web interativas (HTML/JS)**, possibilitando o controle de um jogo em tempo real via joystick físico.
