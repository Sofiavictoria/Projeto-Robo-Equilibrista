# 🤖 Projeto Robô Equilibrista
Projeto desenvolvido para a disciplina de Princípios de Controle, com o objetivo de projetar, modelar e implementar um sistema de controle para um robô autoequilibrante (tipo pêndulo invertido).

![Arduino](https://img.shields.io/badge/Hardware-Arduino_Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/Code-C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)

## 📌 Descrição do Projeto

Este projeto consiste na construção de um robô de duas rodas capaz de se manter em equilíbrio na posição vertical utilizando técnicas de controle realimentado.

O sistema é baseado no modelo clássico do pêndulo invertido, onde o robô deve constantemente ajustar sua posição para evitar a queda.

---

# 🎯 Objetivos
Aplicar conceitos de sistemas dinâmicos e controle
Implementar controle em tempo real
Trabalhar com sensores inerciais (acelerômetro + giroscópio)
Projetar e ajustar um controlador (PID)
Validar o desempenho do sistema na prática

---

## 🧠 Conceitos Envolvidos
Modelagem de sistemas dinâmicos
Linearização de sistemas
Controle PID (Proporcional, Integral e Derivativo)
Estimação de estado (filtro complementar)
Realimentação (feedback)
Estabilidade de sistemas

---
## 🧰 Componentes Utilizados
Microcontrolador (ex: Arduino ou similar)
Driver de motores (ex: ponte H)
Motores DC com caixa de redução
Sensor IMU (ex: MPU6050)
Rodas e chassi
Bateria recarregável

---

## ⚙️ Arquitetura do Sistema
        +-------------------+
        |     Referência    |
        |   (ângulo = 0°)   |
        +---------+---------+
                  |
                  v
          +---------------+
          | Controlador   |
          |     PID       |
          +-------+-------+
                  |
                  v
          +---------------+
          |   Motores     |
          +-------+-------+
                  |
                  v
          +---------------+
          |   Robô        |
          | (Planta)      |
          +-------+-------+
                  |
                  v
          +---------------+
          |   Sensor IMU  |
          +-------+-------+
                  |
                  +-------> Realimentação
---

## 🔄 Funcionamento
O sensor IMU mede o ângulo de inclinação do robô
O filtro complementar combina dados do acelerômetro e giroscópio
O controlador PID calcula o erro entre o ângulo atual e o desejado (0°)
O sistema ajusta a velocidade dos motores
O ciclo se repete continuamente (loop em tempo real)

---

## 🧪 Controle PID

A saída do controlador é dada por:

u(t) = Kp * e(t) + Ki * ∫e(t) dt + Kd * de(t)/dt

Onde:

Kp: ganho proporcional
Ki: ganho integral
Kd: ganho derivativo
e(t): erro (ângulo desejado - ângulo atual)

## 🛠️ Implementação
Linguagem
C/C++ (Arduino)
Etapas principais
Leitura dos dados do IMU
Cálculo do ângulo com filtro complementar
Aplicação do controle PID
Geração de sinais PWM para os motores

---

## 📊 Ajuste de Parâmetros (Tuning)

O ajuste dos ganhos PID foi realizado empiricamente:

Aumentar Kp até o sistema responder rapidamente
Ajustar Kd para reduzir oscilações
Ajustar Ki para corrigir erro estacionário
⚠️ Desafios Encontrados
Ruído nos sensores
Instabilidade inicial do sistema
Ajuste fino dos parâmetros PID
Limitações mecânicas do robô

---

## 👩‍🔬 Autores

| Nome | Curso |
|------|--------|
| Sofia Vitória Bispo da Silva | Engenharia Eletronica |
|  | Engenharia Eletronica |

**Orientação:** Prof. Roberto — Universidade de Brasília.

---
