<div align="center">

# 🚧 SmartGate Arduino

### Cancela bidirecional com máquina de estados, detecção ultrassônica e reabertura por obstáculo

![Arduino](https://img.shields.io/badge/Arduino-UNO-00878F?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-Embedded-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![CI](https://img.shields.io/github/actions/workflow/status/Vinicius-Calegari/SmartGate-Arduino/arduino-ci.yml?style=for-the-badge&label=firmware)

</div>

## O problema

Uma cancela automática não precisa apenas abrir um servo. Ela precisa identificar **de qual lado o veículo chegou**, acompanhar a travessia, decidir quando o veículo realmente liberou a passagem e reagir se surgir um obstáculo durante o fechamento.

O SmartGate é um protótipo didático que explora exatamente essa lógica de controle.

## Arquitetura

```text
Sensor A ─┐                         ┌─ Sensor B
          ├─> Arduino Uno ─> Servo ─┤
          │       │                 │
          │       ├─> LED RGB       │
          │       └─> Buzzer        │
          └──── máquina de estados ─┘
```

Documentação detalhada: [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md)

## Máquina de estados

| Estado | Responsabilidade |
|---|---|
| `AGUARDANDO` | monitora aproximação pelos dois lados |
| `PASSAGEM_A_B` | acompanha veículo iniciado no sensor A |
| `PASSAGEM_B_A` | acompanha veículo iniciado no sensor B |
| `FECHANDO` | move o servo enquanto continua verificando obstáculos |

A direção é representada por `enum class`, eliminando números mágicos para os estados.

## Evolução técnica aplicada

A primeira versão utilizava `delay()` e loops `while` para movimentação e espera de passagem. Isso bloqueava o loop principal.

A versão atual foi refatorada para:

- temporização de sensores com `millis()`;
- movimento incremental do servo sem `delay()`;
- alerta sonoro periódico sem bloquear o loop;
- timeout de passagem;
- reabertura durante fechamento;
- máquina de estados explícita;
- compilação automática no GitHub Actions.

`pulseIn()` ainda é utilizado para medir o HC-SR04, mas possui timeout curto para limitar o bloqueio inerente à leitura ultrassônica.

## Componentes

- Arduino Uno
- 2 × HC-SR04
- micro servo
- buzzer piezoelétrico
- LED RGB / sinalização equivalente
- resistores, protoboard e jumpers

## Pinagem

| Componente | Pino |
|---|---:|
| Sensor A TRIG | 3 |
| Sensor A ECHO | 4 |
| Sensor B TRIG | 13 |
| Sensor B ECHO | 12 |
| LED vermelho | 7 |
| LED verde | 8 |
| LED azul | 10 |
| Buzzer | 6 |
| Servo | 9 |

## Comportamento

1. Em repouso, a cancela permanece fechada.
2. A faixa de aproximação ativa sinalização de alerta.
3. A detecção em A ou B define a direção e abre a cancela.
4. O sensor oposto precisa ser detectado e depois liberado.
5. O fechamento começa sem interromper a leitura periódica.
6. Uma detecção durante o fechamento provoca reabertura.
7. Um timeout impede que o ciclo de passagem permaneça aberto indefinidamente.

## Executar

Abra [`SmartGate-Arduino.ino`](SmartGate-Arduino.ino) na Arduino IDE, selecione **Arduino Uno**, compile e envie. O Serial Monitor utiliza `9600 baud`.

## Segurança

> Este é um protótipo educacional, não um dispositivo de segurança certificado.

HC-SR04 e microservo não substituem fotocélulas adequadas, fins de curso, redundância, relés/controladores de segurança e mecanismos dimensionados para uma cancela física real.

## Roadmap

- [x] substituir `delay()` por controle temporal não bloqueante
- [x] implementar timeout de passagem
- [x] documentar máquina de estados
- [x] adicionar CI de compilação
- [ ] filtro/mediana para leituras ultrassônicas
- [ ] fins de curso independentes
- [ ] telemetria estruturada via Serial
- [ ] esquema elétrico reproduzível
- [ ] testes em bancada para cenários de falha

---

<div align="center">
Desenvolvido por <b>Vinícius Calegari</b><br/>
Engenharia de Controle e Automação • Software • Sistemas Embarcados
</div>
