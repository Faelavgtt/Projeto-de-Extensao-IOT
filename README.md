# ❄️ Smart AC Controller — Automação IoT

![Arduino](https://img.shields.io/badge/Arduino-00979D?style=flat-square&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)
![Status](https://img.shields.io/badge/Status-Conclu%C3%ADdo-6D1D3A?style=flat-square)

Protótipo IoT desenvolvido como **Projeto de Extensão** em Análise e Desenvolvimento de Sistemas (Estácio). O sistema monitora a presença e a temperatura de um ambiente para desligar automaticamente o ar-condicionado via sinal infravermelho, evitando desperdício de energia.

---

### ⚙️ Como funciona
O Arduino desliga o aparelho enviando um comando infravermelho (`RAW`) quando **duas condições** acontecem ao mesmo tempo:
* **Sala vazia:** o sensor de presença (PIR) não detecta movimento.
* **Ambiente climatizado:** o sensor DHT22 registra temperatura abaixo de **28 °C**.

---

### 🔌 Componentes e Pinagem

| Componente | Função | Pino |
| :--- | :--- | :---: |
| **Sensor PIR** | Detecção de movimento | `D2` |
| **Sensor DHT22** | Leitura de temperatura e umidade | `D4` |
| **LED Emissor IR** | Envio do sinal de desligamento (Gree / Samsung) | `D9` |
| **Receptor IR** | Leitura dos códigos do controle original (calibração) | `D11` |

---

### 🚀 Como rodar
1. Instale as bibliotecas **`IRremote`** e **`DHT sensor library`** na Arduino IDE.
2. Monte o circuito conforme a tabela de pinagem acima (`5V` e `GND`).
3. Abra o Monitor Serial em **`9600` baud** e faça o upload do código `.ino` para a placa.

---

Desenvolvido por **Rafaela Vergotti, Ahmed Nasser, Lucas Luares, João Moura** 
