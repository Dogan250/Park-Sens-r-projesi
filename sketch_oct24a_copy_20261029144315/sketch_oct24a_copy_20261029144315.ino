#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TRIG_PIN 7
#define ECHO_PIN 6
#define SDA_PIN 17
#define SCL_PIN 18

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Ekran objesini oluştur (-1 reset pini yok demek)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  // Sensör pin ayarları
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // ESP32-S3 için I2C pinlerini manuel başlattım yoksa ekran sapıtıyor
  Wire.begin(SDA_PIN, SCL_PIN);

  // Ekranı başlat (Adres genelde 0x3C oluyor, Çin işi OLED'lerde böyle)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED patladı, kablolara bak");
    for(;;); // Ekran yoksa sonsuz döngüye sok, sistemi kitle
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); // Zaten tek renk ama kütüphane istiyor
}

void loop() {
  // Sensörü temizle (garanti olsun diye, bazen içinde parazit kalıyor)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // 10 mikrosaniye tetikleme yolla (Sensöre 'ses dalgası gönder' diyoruz)
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Yankıyı oku (30000 timeout koydum ki sensör okuyamazsa sistem kitlenip beklemesin)
  long sure = pulseIn(ECHO_PIN, HIGH, 30000);

  // Gidiş-dönüş süresini cm cinsinden mesafeye çevir
  float mesafe = sure * 0.0343 / 2.0;

  // Ekranı her loop'ta temizlemezsek yazılar üst üste biner
  display.clearDisplay();

  // --- EKRAN TASARIMI ---
  
  // Üstteki "Mesafe:" yazısı (küçük font)
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Mesafe:");

  // Alttaki değer kısmı (büyük font)
  display.setTextSize(2);
  display.setCursor(0, 20);

  // Eğer sensörden 0 döndüyse veya elimi çok yapıştırıp sensörü kör ettiysem "Yok" yaz
  if (sure == 0 || mesafe <= 0 || mesafe > 400) {
    display.print("Yok");
  } else {
    // Virgülden sonra 1 hane göstersin (15.2 cm gibi)
    display.print(mesafe, 1);
    display.print(" cm");
  }

  // Çizdiğimiz her şeyi ekrana bas
  display.display();

  // Çok hızlı okuyup ekranı titretmemek için ufak bekleme
  delay(100);
}