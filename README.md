# ESP32 ve HC-SR04 ile OLED Ekranda Mesafe Olcer

Bu proje, gömülü sistemler çalışmaları kapsamında yaptığım bir mesafe ölçüm uygulamasıdır. HC-SR04 ultrasonik mesafe sensöründen okunan veriler ESP32-S3 mikrodenetleyicisi ile işlenmiş ve I2C haberleşme protokolü kullanılarak 0.96 inç OLED ekrana yazdırılmıştır.

## Video Hakkinda
Repoda bulunan videoda projenin breadboard üzerindeki çalışma halini görebilirsiniz. Sistem çalışırken elimi sensöre yaklaştırdığımda ekrandaki mesafe değeri gecikme olmadan güncellenmektedir. Elimi sensöre tamamen yapıştırdığımda (sensörün okuyamayacağı kadar kör noktaya girdiğinde) yazılım tarafında yazdığım filtre devreye girerek ekrana "Yok" yazdırmaktadır.

## Pin Baglantilari
Donanımların veri ve güç bağlantıları aşağıdaki gibi yapılmıştır:

HC-SR04 Ultrasonik Sensor:
- VCC -> ESP32 3.3V 
- GND -> ESP32 GND
- Trig -> ESP32 GPIO 7
- Echo -> ESP32 GPIO 6

OLED Ekran (I2C):
- VCC -> ESP32 3.3V
- GND -> ESP32 GND
- SDA -> ESP32 GPIO 17
- SCL -> ESP32 GPIO 18

## Projede Karsilasilan Sorunlar ve Cozumleri
Projeyi yaparken donanım ve yazılım tarafında bazı sorunlarla karşılaştım ve şu şekilde çözdüm:

1. Voltaj Uyumsuzlugu ve Veri Tutarsizligi: Normalde HC-SR04 sensörü 5V ile en stabil şekilde çalışır. Ancak sensörü 5V ile besleseydim, Echo pininden ESP32'ye geri dönen sinyal de 5V olacaktı. ESP32 pinleri 3.3V mantığında çalıştığı için araya dirençler ile bir voltaj bölücü devre kurmam gerekiyordu. Elimde direnç olmadığı için kartın GPIO pinini yakmamak adına sensörü mecburen 3.3V pininden besledim. Bu durum sensörün okuduğu verilerde ufak tutarsızlıklara ve dalgalanmalara yol açıyor ancak kartın donanımsal güvenliğini sağlamak için bu mühendislik tercihini yaptım.
2. Seri Port Sorunu: ESP32-S3 kartında Arduino IDE üzerinden "USB CDC on Boot" ayarı kapalı olduğunda seri porttan hata ayıklama verilerini alamıyordum. Tools menüsünden bu ayarı Enabled yaparak düzelttim.
3. Kilitlenme Sorunu: Kod içerisinde pulseIn komutunu direkt kullandığımda sensör okuma yapamazsa kod askıda kalıyordu. pulseIn(ECHO_PIN, HIGH, 30000) şeklinde komuta bir timeout (zaman aşımı) süresi vererek sistemin donmasını engelledim.
4. "Bu voltaj uyumsuzluğundan kaynaklanan veri dalgalanmasını donanımsal olarak çözemediğim için yazılım tarafında bir Hareketli Ortalama Filtresi (Moving Average Filter) yazdım. Sensörden gelen son 5 okumayı bir dizide tutup ortalamasını alarak donanımsal gürültüyü ekarte ettim ve ekranda çok daha stabil bir değer akışı sağladım."
