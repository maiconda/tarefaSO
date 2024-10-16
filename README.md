(em desenvolvimento)

Simulação do Crescimento de Colônias de Bactérias com Prevenção de Deadlock
Sumário
Descrição do Projeto
Estrutura do Código
Compilação e Execução
Análise do Comportamento
Versão com Deadlock
Versão com Prevenção de Impasse
Impacto das Técnicas de Prevenção
Execução e Testes
Testes para Verificar o Deadlock
Testes para Validar a Prevenção de Impasse
Conclusão
Descrição do Projeto
Este projeto implementa uma simulação do crescimento de colônias de bactérias utilizando programação concorrente em C com threads e mutexes. O objetivo é demonstrar a ocorrência de um deadlock (impasse) quando múltiplas threads competem por recursos compartilhados e, posteriormente, aplicar uma técnica de prevenção de impasse para evitar o deadlock.

Contexto do Problema
Crescimento Populacional: Cada colônia de bactérias segue a fórmula de crescimento exponencial:

𝑃
(
𝑡
)
=
𝑃
0
⋅
𝑒
𝑟
𝑡
P(t)=P 
0
​
 ⋅e 
rt
 
Onde:

𝑃
(
𝑡
)
P(t): População no tempo 
𝑡
t.
𝑃
0
P 
0
​
 : População inicial.
𝑟
r: Taxa de crescimento.
𝑒
e: Base dos logaritmos naturais.
Recursos Compartilhados:

Recurso A: Exemplo - nutrientes.
Recurso B: Exemplo - espaço ou área de cultivo.
Requisitos para Crescimento: Cada colônia precisa obter simultaneamente um recurso do tipo A e um do tipo B para completar uma etapa de crescimento. Os recursos são escassos e compartilhados entre as colônias.

Objetivo
Versão Inicial: Permitir que o deadlock ocorra ao projetar o programa de forma que as threads tentem obter os recursos em ordens diferentes.
Versão Modificada: Implementar a prevenção de impasse utilizando a técnica de Ordenação Consistente de Recursos, onde todas as threads obtêm os recursos na mesma ordem.
Estrutura do Código
Arquivos:

growth_simulation_deadlock.c: Implementação que permite o deadlock.
growth_simulation_prevention.c: Implementação com prevenção de impasse.
Principais Componentes:

Threads: Cada colônia é representada por uma thread.
Recursos: Dois arrays de mutexes (recursosA e recursosB) representam os recursos compartilhados.
Função da Colônia (colonia_func): Implementa o comportamento de cada colônia, incluindo a tentativa de obtenção dos recursos e o cálculo do crescimento populacional.
Compilação e Execução
Compilação
Utilize os comandos abaixo para compilar os programas:

Versão com Deadlock:

bash
Copiar código
gcc -o growth_simulation_deadlock growth_simulation_deadlock.c -pthread -lm
Versão com Prevenção de Impasse:

bash
Copiar código
gcc -o growth_simulation_prevention growth_simulation_prevention.c -pthread -lm
Execução
A sintaxe geral para executar os programas é:

bash
Copiar código
./nome_do_programa <P0> <r> <T> <num_colonias> <num_recursos>
Parâmetros:

<P0>: População inicial de cada colônia (ex.: 1000).
<r>: Taxa de crescimento (ex.: 0.1).
<T>: Tempo total a ser simulado (ex.: 5).
<num_colonias>: Número total de colônias (ex.: 4).
<num_recursos>: Número total de recursos de cada tipo (ex.: 1).
Exemplo de Execução:

Versão com Deadlock:

bash
Copiar código
./growth_simulation_deadlock 1000 0.1 5 4 1
Versão com Prevenção de Impasse:

bash
Copiar código
./growth_simulation_prevention 1000 0.1 5 4 1
Análise do Comportamento
Versão com Deadlock
Nesta versão, as colônias tentam obter os recursos em ordens diferentes, criando a possibilidade de um deadlock.

Comportamento Observado:

As colônias entram em um estado de espera circular.
O programa trava, indicando que ocorreu um deadlock.
As colônias ficam bloqueadas aguardando recursos mantidos por outras colônias.
Exemplo de Saída Indicando Deadlock:

css
Copiar código
Colônia 0 tentando obter Recurso A 0
Colônia 0 obteve Recurso A 0
Colônia 1 tentando obter Recurso B 0
Colônia 1 obteve Recurso B 0
Colônia 0 tentando obter Recurso B 0
Colônia 1 tentando obter Recurso A 0
Neste ponto, ambas as colônias estão bloqueadas, cada uma esperando pelo recurso mantido pela outra.

Versão com Prevenção de Impasse
Nesta versão, todas as colônias adquirem os recursos na mesma ordem (Recurso A antes de Recurso B), prevenindo o deadlock.

Comportamento Observado:

As colônias aguardam pela disponibilidade dos recursos sem entrar em impasse.
O programa executa até o final, com todas as colônias completando suas etapas de crescimento.
Não há bloqueios indefinidos.
Exemplo de Saída:

less
Copiar código
Colônia 0 tentando obter Recurso A 0
Colônia 0 obteve Recurso A 0
Colônia 1 tentando obter Recurso A 0
Colônia 0 tentando obter Recurso B 0
Colônia 0 obteve Recurso B 0
Colônia 0: Tempo 0, População 1000.00
Colônia 0 liberou Recurso B 0
Colônia 0 liberou Recurso A 0
Colônia 1 obteve Recurso A 0
...
Impacto das Técnicas de Prevenção
Desempenho:

Sem Prevenção: O deadlock leva ao bloqueio completo das colônias envolvidas, interrompendo o progresso do programa.
Com Prevenção: O programa continua progredindo, mesmo que as colônias tenham que esperar pelos recursos.
Complexidade do Código:

A implementação da prevenção de impasse através da ordenação consistente simplifica o código.
Reduz a complexidade e facilita a manutenção.
Execução e Testes
Testes para Verificar o Deadlock
Configuração:

P0: 1000
r: 0.1
T: 5
num_colonias: 4
num_recursos: 1
Procedimento:

Executar o programa várias vezes para observar o deadlock.
Analisar a saída para identificar onde as colônias ficam bloqueadas.
Resultado:

O programa trava consistentemente, indicando a ocorrência de deadlock.
As colônias ficam bloqueadas esperando pelos recursos.
Testes para Validar a Prevenção de Impasse
Configuração:

Mesmos parâmetros utilizados nos testes anteriores.
Procedimento:

Executar o programa várias vezes para garantir a ausência de deadlock.
Verificar se todas as colônias completam suas etapas de crescimento.
Resultado:

O programa executa corretamente em todas as execuções.
Não ocorre deadlock, confirmando a eficácia da prevenção.
Conclusão
O projeto demonstrou com sucesso a ocorrência de um deadlock em um ambiente de programação concorrente e a implementação eficaz de uma técnica de prevenção de impasse. A ordenação consistente dos recursos mostrou-se uma solução simples e eficaz para eliminar o deadlock, melhorando o desempenho e a confiabilidade do programa sem adicionar complexidade significativa ao código.

Observações Finais:

Certifique-se de ter a biblioteca de threads (-pthread) e matemática (-lm) ao compilar.
Experimente diferentes configurações de parâmetros para explorar o comportamento do programa.
O código está bem documentado para facilitar a compreensão e futuras modificações.
