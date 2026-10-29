#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TRIG_PIN 7
#define ECHO_PIN 6
#define SDA_PIN 17
#define SCL_PIN 18

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- HAREKETLİ ORTALAMA FİLTRESİ İÇİN DEĞİŞKENLER ---
// 3.3V dalgalanmasını önlemek için son 5 ölçümün ortalamasını alacağız
#define ORTALAMA_SAYISI 5
float olcumler[ORTALAMA_SAYISI];
int okumaSirasi = 0;
float toplamMesafe = 0;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Wire.begin(SDA_PIN, SCL_PIN);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED baslatilamadi, baglantilari kontrol et.");
    while(true);
  }

  // Başlangıçta dizinin içinde çöp değer kalmasın diye içini sıfırlıyoruz
  for (int i = 0; i < ORTALAMA_SAYISI; i++) {
    olcumler[i] = 0;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // Sensörü tetikle
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long sure = pulseIn(ECHO_PIN, HIGH, 30000);
  float anlikMesafe = sure * 0.0343 / 2.0;
  float gosterilecekMesafe = 0;

  // Sensör saçmalamadıysa veriyi filtreye sok
  if (sure != 0 && anlikMesafe > 0 && anlikMesafe <= 400) {
    
    // Dizideki en eski ölçümü toplamdan çıkar
    toplamMesafe = toplamMesafe - olcumler[okumaSirasi];
    
    // Yeni ölçümü diziye yaz
    olcumler[okumaSirasi] = anlikMesafe;
    
    // Yeni ölçümü toplama ekle
    toplamMesafe = toplamMesafe + olcumler[okumaSirasi];
    
    // Sırayı bir ilerlet, dizinin sonuna geldiyse başa (0'a) dön
    okumaSirasi = (okumaSirasi + 1) % ORTALAMA_SAYISI;

    // Nihai ortalamayı hesapla
    gosterilecekMesafe = toplamMesafe / ORTALAMA_SAYISI;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Mesafe:");

  display.setTextSize(2);
  display.setCursor(0, 20);

  // Sensör tamamen boşa düştüyse veya çok yakınsa hala "Yok" yazsın
  if (sure == 0 || anlikMesafe <= 0 || anlikMesafe > 400) {
    display.print("Yok");
  } else {
    // Ham veriyi değil, filtrelenmiş pürüzsüz ortalama değeri yazdır
    display.print(gosterilecekMesafe, 1);
    display.print(" cm");
  }

  display.display();
  
  // Ekranın çok hızlı titremesini engellemek için gecikme
  delay(100);
}
