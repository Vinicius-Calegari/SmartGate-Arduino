#include <Servo.h>

Servo cancela;

constexpr uint8_t TRIG_A = 3;
constexpr uint8_t ECHO_A = 4;
constexpr uint8_t TRIG_B = 13;
constexpr uint8_t ECHO_B = 12;
constexpr uint8_t LED_RED = 7;
constexpr uint8_t LED_GREEN = 8;
constexpr uint8_t LED_BLUE = 10;
constexpr uint8_t BUZZER = 6;
constexpr uint8_t SERVO_PIN = 9;

constexpr int DISTANCIA_ALERTA = 150;
constexpr int DISTANCIA_CANCELA = 50;
constexpr int POS_FECHADA = 90;
constexpr int POS_ABERTA = 0;
constexpr unsigned long SENSOR_INTERVAL_MS = 70;
constexpr unsigned long SERVO_INTERVAL_MS = 20;
constexpr unsigned long PASSAGEM_TIMEOUT_MS = 12000;

enum class Estado { AGUARDANDO, PASSAGEM_A_B, PASSAGEM_B_A, FECHANDO };
Estado estado = Estado::AGUARDANDO;

int anguloAtual = POS_FECHADA;
long distA = 999;
long distB = 999;
unsigned long ultimaLeitura = 0;
unsigned long ultimoPassoServo = 0;
unsigned long inicioPassagem = 0;
unsigned long ultimoBeep = 0;
bool sensorDestinoVisto = false;

long medirDistancia(uint8_t trig, uint8_t echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  const unsigned long tempo = pulseIn(echo, HIGH, 25000);
  return tempo == 0 ? 999 : static_cast<long>(tempo * 0.034 / 2.0);
}

void setCor(bool r, bool g, bool b) {
  digitalWrite(LED_RED, r);
  digitalWrite(LED_GREEN, g);
  digitalWrite(LED_BLUE, b);
}

void iniciarAbertura(Estado direcao) {
  estado = direcao;
  inicioPassagem = millis();
  sensorDestinoVisto = false;
  anguloAtual = POS_ABERTA;
  cancela.write(anguloAtual);
  setCor(false, true, false);
  tone(BUZZER, 3500, 160);
  Serial.println(direcao == Estado::PASSAGEM_A_B ? F("Fluxo A -> B") : F("Fluxo B -> A"));
}

void iniciarFechamento() {
  estado = Estado::FECHANDO;
  ultimoPassoServo = 0;
  setCor(true, true, false);
  Serial.println(F("Fechamento iniciado"));
}

void concluirFechamento() {
  anguloAtual = POS_FECHADA;
  cancela.write(anguloAtual);
  estado = Estado::AGUARDANDO;
  setCor(true, false, false);
  noTone(BUZZER);
  Serial.println(F("Cancela fechada"));
}

void atualizarSensores() {
  const unsigned long agora = millis();
  if (agora - ultimaLeitura < SENSOR_INTERVAL_MS) return;
  ultimaLeitura = agora;
  distA = medirDistancia(TRIG_A, ECHO_A);
  distB = medirDistancia(TRIG_B, ECHO_B);
}

void atualizarAlerta() {
  if (estado != Estado::AGUARDANDO) return;
  const long menor = min(distA, distB);
  if (menor > DISTANCIA_ALERTA) {
    setCor(true, false, false);
    noTone(BUZZER);
    return;
  }
  if (menor > DISTANCIA_CANCELA) {
    setCor(true, true, false);
    const unsigned long intervalo = map(menor, DISTANCIA_ALERTA, DISTANCIA_CANCELA, 500, 100);
    if (millis() - ultimoBeep >= intervalo) {
      ultimoBeep = millis();
      tone(BUZZER, 1800, 45);
    }
  }
}

void atualizarPassagem() {
  if (estado != Estado::PASSAGEM_A_B && estado != Estado::PASSAGEM_B_A) return;
  const bool destinoAtivo = estado == Estado::PASSAGEM_A_B
      ? distB <= DISTANCIA_CANCELA
      : distA <= DISTANCIA_CANCELA;

  if (destinoAtivo) sensorDestinoVisto = true;
  if (sensorDestinoVisto && !destinoAtivo) {
    iniciarFechamento();
    return;
  }

  if (millis() - inicioPassagem > PASSAGEM_TIMEOUT_MS) {
    Serial.println(F("Timeout de passagem; aguardando nova leitura segura"));
    if (distA > DISTANCIA_CANCELA && distB > DISTANCIA_CANCELA) iniciarFechamento();
  }
}

void atualizarFechamento() {
  if (estado != Estado::FECHANDO) return;

  if (distA <= DISTANCIA_CANCELA || distB <= DISTANCIA_CANCELA) {
    Serial.println(F("Obstaculo detectado; reabrindo"));
    anguloAtual = POS_ABERTA;
    cancela.write(anguloAtual);
    setCor(false, true, false);
    inicioPassagem = millis();
    sensorDestinoVisto = false;
    estado = distA <= DISTANCIA_CANCELA ? Estado::PASSAGEM_A_B : Estado::PASSAGEM_B_A;
    return;
  }

  const unsigned long agora = millis();
  if (agora - ultimoPassoServo < SERVO_INTERVAL_MS) return;
  ultimoPassoServo = agora;

  if (anguloAtual < POS_FECHADA) {
    anguloAtual++;
    cancela.write(anguloAtual);
    if (anguloAtual % 10 == 0) tone(BUZZER, 1200, 30);
  } else {
    concluirFechamento();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_A, OUTPUT); pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT); pinMode(ECHO_B, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_RED, OUTPUT); pinMode(LED_GREEN, OUTPUT); pinMode(LED_BLUE, OUTPUT);
  cancela.attach(SERVO_PIN);
  cancela.write(POS_FECHADA);
  setCor(true, false, false);
  Serial.println(F("SmartGate iniciado"));
}

void loop() {
  atualizarSensores();

  if (estado == Estado::AGUARDANDO) {
    if (distA <= DISTANCIA_CANCELA) iniciarAbertura(Estado::PASSAGEM_A_B);
    else if (distB <= DISTANCIA_CANCELA) iniciarAbertura(Estado::PASSAGEM_B_A);
  }

  atualizarAlerta();
  atualizarPassagem();
  atualizarFechamento();
}
