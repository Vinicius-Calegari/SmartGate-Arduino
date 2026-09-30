#include <Servo.h>

Servo cancela;

const int TRIG_A = 3;
const int ECHO_A = 4;
const int TRIG_B = 13;
const int ECHO_B = 12;

const int LED_RED = 7;
const int LED_GREEN = 8;
const int LED_BLUE = 10;

const int BUZZER = 6;
const int SERVO_PIN = 9;

const int DISTANCIA_ALERTA = 150;
const int DISTANCIA_CANCELA = 50;

const int POS_FECHADA = 90;
const int POS_ABERTA = 0;

int estado = 0;
bool cancelaAberta = false;

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_A, OUTPUT);
  pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT);
  pinMode(ECHO_B, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  cancela.attach(SERVO_PIN);
  cancela.write(POS_FECHADA);
  corVermelha();

  Serial.println("================================");
  Serial.println("   SISTEMA DE CANCELA INICIADO  ");
  Serial.println("================================");
}

void loop() {
  long distA = medirDistancia(TRIG_A, ECHO_A);
  long distB = medirDistancia(TRIG_B, ECHO_B);
  long menorDistancia = min(distA, distB);

  bool sensorA = (distA <= DISTANCIA_CANCELA);
  bool sensorB = (distB <= DISTANCIA_CANCELA);

  // Alerta de aproximação enquanto a cancela está fechada.
  if (!cancelaAberta && menorDistancia <= DISTANCIA_ALERTA &&
      menorDistancia > DISTANCIA_CANCELA) {
    corAmarela();
    int intervalo = map(menorDistancia, DISTANCIA_ALERTA,
                        DISTANCIA_CANCELA, 250, 30);
    tone(BUZZER, 1800);
    delay(20);
    noTone(BUZZER);
    delay(intervalo);
  }

  if (!cancelaAberta && menorDistancia > DISTANCIA_ALERTA) {
    corVermelha();
    noTone(BUZZER);
  }

  // estado 1: veículo entrou pelo sensor A.
  // estado 2: veículo entrou pelo sensor B.
  if (estado == 0 && sensorA) {
    abrirCancela();
    estado = 1;
    cancelaAberta = true;
    delay(500);
  } else if (estado == 0 && sensorB) {
    abrirCancela();
    estado = 2;
    cancelaAberta = true;
    delay(500);
  } else if (estado == 1) {
    if (distB <= DISTANCIA_CANCELA) {
      while (medirDistancia(TRIG_B, ECHO_B) <= DISTANCIA_CANCELA) {
        delay(60);
      }
      fecharCancela();
      estado = 0;
    }
  } else if (estado == 2) {
    if (distA <= DISTANCIA_CANCELA) {
      while (medirDistancia(TRIG_A, ECHO_A) <= DISTANCIA_CANCELA) {
        delay(60);
      }
      fecharCancela();
      estado = 0;
    }
  }

  delay(20);
}

long medirDistancia(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long tempo = pulseIn(echo, HIGH, 30000);
  if (tempo == 0) return 999;

  return tempo * 0.034 / 2;
}

void abrirCancela() {
  corVerde();
  tone(BUZZER, 3500, 200);

  for (int ang = POS_FECHADA; ang >= POS_ABERTA; ang--) {
    cancela.write(ang);
    delay(20);
  }

  cancela.write(POS_ABERTA);
}

void fecharCancela() {
  corAmarela();

  for (int ang = POS_ABERTA; ang <= POS_FECHADA; ang++) {
    long distA = medirDistancia(TRIG_A, ECHO_A);
    long distB = medirDistancia(TRIG_B, ECHO_B);

    // Segurança: se qualquer sensor detectar obstáculo durante
    // o fechamento, a cancela volta imediatamente a abrir.
    if (distA <= DISTANCIA_CANCELA || distB <= DISTANCIA_CANCELA) {
      abrirCancela();
      cancelaAberta = true;
      return;
    }

    tone(BUZZER, 1200);
    delay(40);
    noTone(BUZZER);
    delay(15);
    cancela.write(ang);
  }

  cancela.write(POS_FECHADA);
  noTone(BUZZER);
  corVermelha();
  cancelaAberta = false;
}

void corVermelha() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
}

void corVerde() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
}

void corAmarela() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
}
