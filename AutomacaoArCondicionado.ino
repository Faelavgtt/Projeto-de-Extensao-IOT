#include <Arduino.h>
#include <DHT.h>

// Pinos finais do projeto
#define PINO_PIR 2
#define PINO_DHT 4
#define PINO_IR 9
#define TIPO_DHT DHT22

DHT dht(PINO_DHT, TIPO_DHT);

// Seus 199 pulsos reais capturados do controle Samsung AC
const uint16_t rawDataOff[] = {
  4488, 4344, 624, 1532, 624, 456, 616, 1532, 624, 1536, 624, 456, 624, 456, 616, 1532,
  624, 456, 624, 456, 624, 1532, 616, 456, 624, 456, 624, 1532, 620, 1532, 624, 456,
  624, 1532, 624, 456, 616, 1536, 624, 1532, 624, 1536, 620, 1528, 624, 460, 620, 1532,
  624, 1532, 628, 1532, 616, 456, 624, 456, 620, 460, 612, 460, 620, 1536, 624, 456,
  624, 448, 624, 1532, 624, 1536, 624, 1532, 616, 464, 616, 456, 624, 456, 624, 456,
  616, 456, 624, 456, 624, 456, 624, 456, 616, 1532, 624, 1536, 620, 1536, 616, 1532,
  624, 1540, 620, 5136, 4484, 4348, 616, 1532, 628, 452, 624, 1536, 620, 1536, 616, 456,
  624, 456, 624, 1532, 624, 448, 624, 456, 624, 1532, 628, 452, 628, 452, 620, 1532,
  624, 1532, 628, 452, 624, 1528, 620, 460, 620, 1536, 624, 1532, 624, 1528, 624, 1532,
  624, 456, 624, 1532, 616, 1544, 616, 1532, 624, 456, 624, 456, 624, 456, 616, 456,
  624, 1532, 624, 456, 616, 456, 624, 1536, 624, 1532, 616, 1540, 616, 456, 624, 456,
  624, 456, 616, 464, 616, 456, 624, 456, 616, 464, 616, 456, 624, 1532, 624, 1536,
  616, 1540, 616, 1536, 620, 1532, 628
};

// Regras de automacao
const float TEMP_LIMITE = 28.0;                                // Desliga se estiver abaixo de 25.0 C
const unsigned long TEMPO_AUSENCIA_MAX = 15UL * 1000UL; // 15 minutos sem movimento (para teste rapido, mude para 15UL * 1000UL)
const unsigned long INTERVALO_DHT = 3000;                      // Le temperatura a cada 3 segundos
const unsigned long INTERVALO_REENVIO_IR = 60000;              // Se o ar continuar gelando, tenta novamente apos 1 min

unsigned long ultimoMovimento = 0;
unsigned long ultimaLeituraDHT = 0;
unsigned long ultimoEnvioIR = 0;
float temperaturaAtual = 30.0;
float umidadeAtual = 0.0;

void enviarPulso38kHz(uint16_t duracaoMicros) {
  unsigned long inicio = micros();
  while (micros() - inicio < duracaoMicros) {
    digitalWrite(PINO_IR, HIGH);
    delayMicroseconds(9);
    digitalWrite(PINO_IR, LOW);
    delayMicroseconds(13);
  }
}

void enviarComandoSamsung() {
  int tamanho = sizeof(rawDataOff) / sizeof(rawDataOff[0]);
  for (int i = 0; i < tamanho; i++) {
    if (i % 2 == 0) {
      enviarPulso38kHz(rawDataOff[i]);
    } else {
      digitalWrite(PINO_IR, LOW);
      delayMicroseconds(rawDataOff[i]);
    }
  }
  digitalWrite(PINO_IR, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(PINO_PIR, INPUT);
  pinMode(PINO_IR, OUTPUT);
  digitalWrite(PINO_IR, LOW);

  dht.begin();

  Serial.println("Estabilizando PIR (15s)...");
  delay(15000);
  ultimoMovimento = millis();
  Serial.println("Sistema ativo! Monitorando presenca + temperatura (< 25 C).");
}

void loop() {
  unsigned long agora = millis();

  // 1. Sensor PIR: Reseta contador se detectar movimento
  if (digitalRead(PINO_PIR) == HIGH) {
    ultimoMovimento = agora;
  }

  // 2. Sensor DHT22: Le temperatura e umidade
  if (agora - ultimaLeituraDHT >= INTERVALO_DHT) {
    ultimaLeituraDHT = agora;
    float u = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(u) || isnan(t)) {
      Serial.println("[ERRO] Falha na leitura do DHT22!");
    } else {
      umidadeAtual = u;
      temperaturaAtual = t;

      unsigned long segundosSemMovimento = (agora - ultimoMovimento) / 1000;
      Serial.print("Temp: ");
      Serial.print(temperaturaAtual);
      Serial.print(" C | Umidade: ");
      Serial.print(umidadeAtual);
      Serial.print("% | Sem movimento ha: ");
      Serial.print(segundosSemMovimento);
      Serial.println("s");
    }
  }

  // 3. Decisao Conjunta: Sem movimento pelo tempo limite E temperatura < 25 C
  bool semPresenca = (agora - ultimoMovimento >= TEMPO_AUSENCIA_MAX);
  bool ambienteGelado = (temperaturaAtual < TEMP_LIMITE);

  if (semPresenca && ambienteGelado) {
    if (ultimoEnvioIR == 0 || (agora - ultimoEnvioIR >= INTERVALO_REENVIO_IR)) {
      Serial.println("[ACAO] Sala vazia + Temp < 25 C! Desligando ar-condicionado...");
      enviarComandoSamsung();
      ultimoEnvioIR = agora;
      Serial.println("[OK] Comando OFF enviado.");
    }
  } else {
    ultimoEnvioIR = 0;
  }
}