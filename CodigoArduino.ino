#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// LCD  (I2C address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// ================== anotações ==================
// Luzes do semáforo para os carros
int carRed = 12; 
int carYellow = 11; 
int carGreen = 10;

// Luzes para os pedestres
int pedRed = 9;  
int pedGreen = 8;

// Som e botão
int buzzer = 7;  
int button = 2;

// Tempo total de travessia (em segundos)
int crossTime = 15; 
unsigned long changeTime;   // armazena a ùltima vez que as luzes mudaram

// ================== Notas da música (Marcha imperial - Star Wars) ==================
// Notas musicais e suas frequências
const int a=440, f=349, cH=523, eH=659, fH=698, gS=415, 
          aH=880, gSH=830, gH=784, fSH=740, aS=455, 
          dSH=622, dH=587, cSH=554, b=466;

// Melodia completa
int melody[] = {
  a, a, a, f, cH, a, f, cH, a,      
  eH, eH, eH, fH, cH, gS, f, cH, a, 
  aH, a, a, aH, gSH, gH, fSH, fH, fSH, 
  aS, dSH, dH, cSH, cH, b, cH,      
  f, gS, f, a, cH, a, cH, eH,       
  aH, a, a, aH, gSH, gH, fSH, fH, fSH, 
  aS, dSH, dH, cSH, cH, b, cH,      
  f, gS, f, cH, a, f, cH, a         
};

// Duração (em ms) de cada nota
int durations[] = {
  500, 500, 500, 350, 150, 500, 350, 150, 650,
  500, 500, 500, 350, 150, 500, 350, 150, 650,
  500, 300, 150, 500, 325, 175, 125, 125, 250,
  250, 500, 325, 175, 125, 125, 250,
  250, 500, 350, 125, 500, 375, 125, 650,
  500, 300, 150, 500, 325, 175, 125, 125, 250,
  250, 500, 325, 175, 125, 125, 250,
  250, 500, 375, 125, 500, 375, 125, 650
};

int songLength = sizeof(melody) / sizeof(int); // Número total de notas
int currentNote = 0;                           // Índice da nota atual

// ================== Tocar música ==================
void playStarWarsStep() {
  // Reinicia a música se ela acabar
  if (currentNote >= songLength) currentNote = 0;

  int duration = durations[currentNote];
  tone(buzzer, melody[currentNote], duration);  // Toca a nota

  // Espera a duração da nota e dá uma pequena pausa
  delay(duration * 1.10); 
  noTone(buzzer);

  currentNote++;  // Contador que leva até a próxima nota
}

// ================== Configuração ==================
void setup() {
  // Configura pins como saídas ou entradas
  pinMode(carRed, OUTPUT); 
  pinMode(carYellow, OUTPUT); 
  pinMode(carGreen, OUTPUT);
  pinMode(pedRed, OUTPUT); 
  pinMode(pedGreen, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  // Estado inicial do semáforo
  digitalWrite(carGreen, HIGH);
  digitalWrite(pedRed, HIGH);

  // Inicializa o LCD
  lcd.init();
  lcd.backlight();
  exibirMensagemInicial();

  changeTime = 0; // Ainda sem ciclo anterior
}

// Mostra a mensagem inicial no LCD
void exibirMensagemInicial() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("APERTE O BOTAO");
  lcd.setCursor(0, 1);
  lcd.print("P/ ATRAVESSAR");
}

// ================== LOOP ==================
void loop() {
  // Se o botão for apertado e se passarem 5 segundos
  if (digitalRead(button) == LOW && (changeTime == 0 || (millis() - changeTime) > 5000)) {
      changeLights();
  }
}

// ================== Sequência das luzes do semáforo ==================
void changeLights() {
  lcd.clear();
  lcd.print("AGUARDE...");

  // Carros: verde -> amarelo -> vermelho
  digitalWrite(carGreen, LOW);
  digitalWrite(carYellow, HIGH);
  delay(2000);
  digitalWrite(carYellow, LOW);
  digitalWrite(carRed, HIGH);
  delay(1000);

  // pedestres: vermelho -> verde
  digitalWrite(pedRed, LOW);
  digitalWrite(pedGreen, HIGH);

  currentNote = 0; // Toca a música do início

  // loop da travessia de 15 segundos
  for (int t = crossTime; t > 0; t--) {
    lcd.setCursor(0, 0);
    lcd.print("ATRAVESSE!    ");
    lcd.setCursor(0, 1);
    lcd.print("TEMPO: ");
    lcd.print(t);
    lcd.print("s  ");

    unsigned long segundoAtual = millis();
    // Continua tocando a música durante o segundo de intervalo
    while (millis() - segundoAtual < 1000) {
      playStarWarsStep();
    }
  }

  noTone(buzzer); // Para a músics

  // Fase de alerta (PARE)
  for (int i = 0; i< 10; i++) {
    lcd.clear();
    lcd.print("PARE!");
    tone(buzzer, 2000);
    digitalWrite(pedGreen, HIGH);
    delay(200);
    noTone(buzzer);
    digitalWrite(pedGreen, LOW);
    delay(200);
  }

  // Reinicia
  digitalWrite(pedRed, HIGH);
  delay(500);
  digitalWrite(carRed, LOW);
  delay(1000);
  digitalWrite(carGreen, HIGH);
  
  changeTime = millis();       // Salva o tempo do último ciclo
  exibirMensagemInicial();     // Mostra a mensagem inicial no LCD
}
