<div align="center">

# 🚧 SmartGate Arduino

### Protótipo de cancela automática bidirecional com detecção de veículos e proteção contra fechamento sobre obstáculos

![Arduino](https://img.shields.io/badge/Arduino-UNO-00878F?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-Embedded-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Status](https://img.shields.io/badge/status-protótipo_funcional-238636?style=for-the-badge)

</div>

---

## Visão geral

O **SmartGate** é um protótipo de automação desenvolvido com Arduino Uno para controlar uma cancela de forma bidirecional. Dois sensores ultrassônicos identificam a aproximação e a passagem do veículo, enquanto um servo executa o movimento da cancela.

O sistema também fornece feedback por LED e buzzer e verifica os sensores durante o fechamento para evitar que a barreira continue descendo quando existe um obstáculo na região monitorada.

O objetivo do projeto não é apenas movimentar um servo, mas implementar uma pequena lógica de controle capaz de interpretar **de qual lado o veículo chegou**, acompanhar sua passagem e decidir quando é seguro iniciar o fechamento.

## ⚙️ Funcionamento

```text
                   ÁREA DA CANCELA

 Sensor A  ───────────►  | CANCELA |  ◄─────────── Sensor B
      │                       │                        │
      └── detecta entrada ────┼──── detecta saída ────┘
                              │
                         Servo motor
                              │
                   LED RGB + buzzer
```

Em repouso, o sistema mantém a cancela fechada e a sinalização vermelha. Quando um objeto entra na faixa de alerta, o buzzer aumenta a frequência dos avisos conforme a distância diminui. Ao atingir a zona de acionamento, a cancela abre.

A variável `estado` registra qual sensor iniciou o ciclo. Isso permite distinguir os fluxos **A → B** e **B → A**. O fechamento só é iniciado depois que o veículo alcança e libera o sensor localizado no lado oposto.

Durante o movimento de fechamento, os dois sensores continuam sendo consultados. Se um obstáculo for detectado, o fechamento é interrompido e a cancela volta a abrir.

## 🧠 Máquina de estados

| Estado | Significado | Próxima condição |
|---|---|---|
| `0` | Sistema aguardando veículo | Sensor A ou B detecta aproximação |
| `1` | Entrada iniciada pelo lado A | Veículo passa e libera o sensor B |
| `2` | Entrada iniciada pelo lado B | Veículo passa e libera o sensor A |

Essa abordagem evita depender apenas de temporizadores para decidir quando fechar a cancela.

## 🧰 Componentes do protótipo

- Arduino Uno
- 2 × sensores ultrassônicos HC-SR04
- 1 × micro servo
- buzzer piezoelétrico
- LED RGB / sinalização equivalente
- resistores
- protoboard
- jumpers
- alimentação USB

## 🔌 Mapeamento de pinos

| Componente | Pino Arduino |
|---|---:|
| Sensor A — TRIG | 3 |
| Sensor A — ECHO | 4 |
| Sensor B — TRIG | 13 |
| Sensor B — ECHO | 12 |
| LED vermelho | 7 |
| LED verde | 8 |
| LED azul | 10 |
| Buzzer | 6 |
| Servo | 9 |

## 📏 Parâmetros de controle

```cpp
const int DISTANCIA_ALERTA = 150;   // cm
const int DISTANCIA_CANCELA = 50;   // cm
const int POS_FECHADA = 90;         // graus
const int POS_ABERTA = 0;           // graus
```

Os valores são parâmetros de protótipo e devem ser recalibrados conforme posicionamento dos sensores, geometria da passagem e mecanismo utilizado.

## 🛡️ Proteção durante o fechamento

O firmware mede novamente as distâncias a cada passo do servo durante o fechamento. Se qualquer sensor indicar um objeto dentro da zona configurada, `abrirCancela()` é executada e o ciclo de fechamento é abortado.

> Este é um **protótipo educacional**. Os sensores HC-SR04 e um micro servo não constituem, por si só, um sistema de segurança adequado para uma cancela real. Uma aplicação física deve utilizar sensores, atuadores, redundâncias, fins de curso e dispositivos de segurança apropriados ao risco.

## 🚦 Sinalização

- 🔴 **Vermelho:** cancela fechada / sistema aguardando.
- 🟡 **Amarelo:** aproximação detectada ou fechamento em andamento.
- 🟢 **Verde:** cancela aberta.
- 🔊 **Buzzer:** alerta de proximidade e indicação sonora durante movimentos.

## 💻 Firmware

O código principal está em [`SmartGate-Arduino.ino`](./SmartGate-Arduino.ino).

Para executar:

1. Abra o arquivo na Arduino IDE.
2. Selecione a placa **Arduino Uno**.
3. Conecte os componentes conforme o mapeamento de pinos.
4. Compile e envie o firmware.
5. Use o Serial Monitor em **9600 baud** para acompanhar a inicialização.

## 🏗️ Decisões técnicas

O protótipo separa leitura dos sensores, movimento do servo e sinalização em funções específicas. A direção do veículo é representada por uma máquina de estados simples, e a leitura ultrassônica utiliza timeout para evitar espera indefinida quando não há retorno do eco.

A distância é calculada a partir do tempo de voo do pulso ultrassônico:

```text
distância ≈ tempo × 0,034 / 2
```

A divisão por dois representa o percurso de ida e volta da onda sonora.

## ⚠️ Limitações atuais

A versão atual ainda utiliza `delay()`, `pulseIn()` e pequenos loops bloqueantes. Isso é aceitável para demonstrar o protótipo, mas limita a capacidade do microcontrolador de executar tarefas concorrentes.

Uma evolução relevante é transformar o controle em uma **máquina de estados totalmente não bloqueante**, utilizando `millis()` para temporização e leituras periódicas dos sensores.

## 🗺️ Roadmap

- [ ] substituir temporizações bloqueantes por `millis()`;
- [ ] filtrar leituras ultrassônicas para reduzir falsos positivos;
- [ ] adicionar fins de curso independentes;
- [ ] implementar timeout de passagem;
- [ ] registrar eventos via Serial;
- [ ] adicionar display ou interface de monitoramento;
- [ ] estudar sensores redundantes para uma versão física mais robusta;
- [ ] documentar o circuito com esquema elétrico reproduzível.

---

<div align="center">

Desenvolvido por **Vinícius Calegari**  
Engenharia de Controle e Automação • Desenvolvimento de Software • Sistemas Embarcados

</div>
