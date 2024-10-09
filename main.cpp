#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int screenWidth = 16;
int screenHeight = 2;

int buttonInicia = 0;
int buttonSim = 12;  // Botão que representa a primeira sequência ou "SIM" nas perguntas
int buttonNao = 13;  // Botão que representa a segunda sequência ou "NÃO" nas perguntas

int ledVermelho = 2;
int ledAmarelo = 11;
int pinBuzzer = 10;

char *apresentacao[6][2] = {
    {"QUIZ DEVERLYN", "Pressione INICIO"},
    {"Fase da memoria", "ATENCAO"},
    {"Fase de", "perguntas"},
    {"Escolha uma sequencia", ""},
    {"Fim de Jogo", "Voce perdeu"},
    {"Fim de Jogo", "Voce ganhou"}
};

// Perguntas de conhecimentos gerais
char *perguntas[3][5] = {
    {"A Terra e redonda?", "O ferro e mole?", "O gato mia?", "O mar e doce?", "A agua ferve?"},
    {"O Brasil tem praia?", "O cavalo voa?", "A lua e de queijo?", "O ar e leve?", "A neve e quente?"},
    {"O atomo e visivel?", "O Sol e uma estrela?", "A China e pequena?", "O ouro é barato?", "O pinguim nada?"}
};

int respostas[3][5] = {
    {buttonSim, buttonNao, buttonSim, buttonNao, buttonSim},
    {buttonSim, buttonNao, buttonNao, buttonSim, buttonNao},
    {buttonNao, buttonSim, buttonNao, buttonNao, buttonSim}
};

// Sequências de teste para exibir na tela
int sequenciaLed[3][5] = {
    {ledAmarelo, ledAmarelo, ledVermelho, ledAmarelo, ledAmarelo},
    {ledVermelho, ledAmarelo, ledVermelho, ledVermelho, ledAmarelo},
    {ledVermelho, ledAmarelo, ledAmarelo, ledVermelho, ledAmarelo}
};

int respostaSequencia[3][1] = {
    {buttonSim},
    {buttonNao},
    {buttonSim}
};

char* respostaApresentacao[3][1] = {
    {"a, a, v, a, a"},
    {"v, a, v, a, a"},
    {"v, a, a, v, a"}
};

// Sequência correta: 0 = primeira, 1 = segunda
int sequenciaCorreta = 0;

int fase = 0; // 0 = fase da memória, 1 = fase de perguntas
int nivel = 0;
int pontos = 0;
int erros = 0;
bool fimDeJogo = false;
int perguntaAtual = 0;

void setup() {
    lcd.begin(screenWidth, screenHeight);

    pinMode(buttonSim, INPUT_PULLUP);
    pinMode(buttonNao, INPUT_PULLUP);
    pinMode(buttonInicia, INPUT_PULLUP);

    pinMode(ledVermelho, OUTPUT);
    pinMode(ledAmarelo, OUTPUT);
    
    Serial.begin(9600);
}

void loop() {
    lcd.clear();
    inicioDoJogo();
}

void inicioDoJogo() {
    if (!fimDeJogo) {
        escreve(apresentacao[0][0], apresentacao[0][1]);
        if (digitalRead(buttonInicia) == LOW) {
            if (fase == 0) {
                escreve(apresentacao[1][0], apresentacao[1][1]);
                jogoMemoria();
            } else if (fase == 1) {
                escreve(apresentacao[2][0], apresentacao[2][1]);
                jogoDasPerguntas();
            }
        }
    } else {
        finalizaPartida();
    }
}

void jogoMemoria() {
    //exibe sequencia led
    for(int a = 0; a<3; a++){
        displayMatrizLed(a);
        delay(1000);
        // Exibe duas sequências na tela
        exibeSequencias(a);
        // Espera o usuário escolher uma sequência (botão 12 ou 13)

        delay(1000);
        escreve("Escolha","Vdd ou falso");
        int escolha = esperaEscolha();

        // Verifica se a escolha está correta
        for(int i=0; i<3; i++){
            sequenciaCorreta = respostaSequencia[i][0];
            if (escolha == sequenciaCorreta) {
                acertou();
            } else {
                errou();
            }
        }

        delay(1000);
    }


    // Se o usuário acertar 3 vezes, vai para a fase de perguntas
    if (pontos >= 3) {
        fase = 1;
        nivel = 0;
        perguntaAtual = 0;
    }
}

void exibeSequencias(int posicao) {
    delay(1000);
    escreve("vdd ou false",respostaApresentacao[posicao][0]);
}

int esperaEscolha() {
    unsigned long tempoInicio = millis();
    while (millis() - tempoInicio < 5000) {  // Espera até 5 segundos
        if (digitalRead(buttonSim) == LOW) {
            return buttonSim;  // Escolheu a primeira sequência
        }
        if (digitalRead(buttonNao) == LOW) {
            return buttonNao;  // Escolheu a segunda sequência
        }
    }
    return -1;  // Nenhuma escolha foi feita no tempo limite
}

void acertou() {
    pontos++;
    tone(pinBuzzer, 1000);  // Som para resposta correta
    delay(500);
    noTone(pinBuzzer);
}

void errou() {
    erros++;
    tone(pinBuzzer, 500);  // Som para resposta incorreta
    delay(500);
    noTone(pinBuzzer);

    if (erros >= 3) {
        fimDeJogo = true;
    }
}

void jogoDasPerguntas() {
    if (perguntaAtual < 5) {
        // Mostra a pergunta atual
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(perguntas[nivel][perguntaAtual]);

        // Espera a resposta do usuário
        int respostaUsuario = esperaEscolha();

        // Verifica a resposta do usuário
        if (respostaUsuario == respostas[nivel][perguntaAtual]) {
            acertou();
        } else {
            errou();
        }

        perguntaAtual++;

        // Após 5 perguntas, avança de nível
        if (perguntaAtual >= 5) {
            nivel++;
            perguntaAtual = 0;
            if (nivel >= 3) {
                fimDeJogo = true;
            }
        }
    }
}

void finalizaPartida() {
    lcd.clear();
    if (pontos >= 15) {
        escreve(apresentacao[5][0], apresentacao[5][1]);
    } else {
        escreve(apresentacao[4][0], apresentacao[4][1]);
    }
    delay(5000);  // Pausa antes de reiniciar o jogo
    reiniciaJogo();
}

void reiniciaJogo() {
    fimDeJogo = false;
    erros = 0;
    pontos = 0;
    fase = 0;
    nivel = 0;
}

void escreve(char* primeira, char* segunda) {
    lcd.clear();
    lcd.setCursor(obtemPosicao(primeira), 0);
    lcd.print(primeira);
    lcd.setCursor(obtemPosicao(segunda), 1);
    lcd.print(segunda);
    delay(2000);
}

int obtemPosicao(char* texto) {
    int tamanho = strlen(texto);
    return (screenWidth - tamanho) / 2;
}

void displayMatrizLed(int linha) {
    for (int i = 0; i < 5; i++) {
        digitalWrite(sequenciaLed[linha][i], HIGH);
        delay(1000);
        digitalWrite(sequenciaLed[linha][i], LOW);
    }
    delay(2000);
}