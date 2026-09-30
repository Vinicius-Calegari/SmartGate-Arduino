# Cenários de teste em bancada

| Cenário | Entrada | Resultado esperado |
|---|---|---|
| Repouso | A e B livres | cancela fechada, LED vermelho |
| Aproximação A | A entre 50–150 cm | alerta sem abrir |
| A → B | A < 50 cm, depois B < 50 cm e B livre | abre, acompanha passagem e fecha |
| B → A | B < 50 cm, depois A < 50 cm e A livre | comportamento simétrico |
| Obstáculo fechando | A ou B < 50 cm durante fechamento | reabre imediatamente |
| Eco ausente | `pulseIn` expira | distância tratada como 999 cm |
| Veículo para no meio | destino não conclui antes do timeout | só inicia fechamento quando ambos sensores estiverem livres |
| Ruído isolado | leitura curta única | cenário a melhorar com filtro de leituras |

## Critério para evolução

Antes de alterar a máquina de estados, repetir os cenários A→B, B→A, obstáculo e timeout. Para uma aplicação física real, estes testes não substituem análise de risco nem componentes de segurança certificados.
