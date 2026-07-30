#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- PIN DEFINITIONS ---
#define DHT_PIN 2           
#define DHT_TYPE DHT11      
#define WATER_LEVEL_PIN A1  
#define HEATER_RELAY_PIN 4  
#define COOLER_RELAY_PIN 5  
#define PUMP_RELAY_PIN 6    
#define LED_PIN 7           
#define BUZZER_PIN 8        

// 3 Adet Toprak Nem Sensörü için Array tanımlaması
const int NUM_SENSORS = 3;
const int moisturePins[NUM_SENSORS] = {A0, A2, A3}; 

// --- STRUCT DEFINITION ---
// Bir bitkinin ihtiyaç duyduğu tüm özellikleri tek bir pakette topluyoruz.
struct PlantProfile {
  String profileName;      // Bitkinin ekranda yazacak adı
  float minTemp;           // Minimum sıcaklık eşiği
  float maxTemp;           // Maksimum sıcaklık eşiği
  int moistureCritical;    // Sulama gerektiren nem eşiği
};

// --- PRESETS ---
PlantProfile amasyaApple = {"Amasya Apple", 5.0, 25.0, 600};
PlantProfile mersinBanana = {"Mersin Banana", 22.0, 35.0, 400}; // Muz daha nemli ortam ister (Düşük ADC değeri = Islak)

// AKTİF MOD SEÇİMİ
// Sistemi değiştirmek için burayı mersinBanana yap
PlantProfile activeProfile = amasyaApple; 

// --- GLOBAL VARIABLES ---
float currentTemp = 0.0;
int avgMoisture = 0;          // Artık tek bir sensör değil, ortalama değeri tutuyoruz
int currentWaterLevel = 0;
bool isTankEmpty = false;     
const int WATER_EMPTY_LEVEL = 100;
String soilStatus = "OK";

// --- OBJECT INITIALIZATION  ---
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(9600);
  
  dht.begin();
  lcd.init();
  lcd.backlight();
  
  pinMode(HEATER_RELAY_PIN, OUTPUT);
  pinMode(COOLER_RELAY_PIN, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  turnOffAllRelays();
  
  // Başlangıç ekranında hangi modda olduğumuzu gösterelim
  lcd.setCursor(0, 0);
  lcd.print("Mode Active:");
  lcd.setCursor(0, 1);
  lcd.print(activeProfile.profileName);
  delay(3000); // 3 saniye ekranda kalsın
}

void loop() {
  readSensors();
  checkSafety();
  
  if (!isTankEmpty) {
    controlTemperature();
    controlIrrigation();
  } else {
    turnOffAllRelays();
    triggerAlarm();
  }
  
  updateDisplay();
  delay(2000); 
}

// ==========================================
// --- MODULAR FUNCTIONS ---
// ==========================================

void readSensors() {
  currentTemp = dht.readTemperature();
  currentWaterLevel = analogRead(WATER_LEVEL_PIN);
  
  // Çoklu Sensör Ortalamasını hesapla
  long sum = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    sum += analogRead(moisturePins[i]); // Her sensörü oku ve toplama ekle
  }
  avgMoisture = sum / NUM_SENSORS;      // Toplamı sensör sayısına bölerek ortalamayı bul
  
  if (isnan(currentTemp)) {
    currentTemp = 0.0; 
  }
}

void checkSafety() {
  if (currentWaterLevel < WATER_EMPTY_LEVEL) {
    isTankEmpty = true;
  } else {
    isTankEmpty = false;
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void controlTemperature() {
  // Artık sabit değerler değil, activeProfile içindeki değişkenler kullanılıyor
  if (currentTemp < activeProfile.minTemp) {
    digitalWrite(HEATER_RELAY_PIN, HIGH); 
    digitalWrite(COOLER_RELAY_PIN, LOW);  
  } 
  else if (currentTemp > activeProfile.maxTemp) {
    digitalWrite(HEATER_RELAY_PIN, LOW);  
    digitalWrite(COOLER_RELAY_PIN, HIGH); 
  } 
  else {
    digitalWrite(HEATER_RELAY_PIN, LOW);
    digitalWrite(COOLER_RELAY_PIN, LOW);
  }
}

void controlIrrigation() {
  // Aktif profilin nem ihtiyacına göre kıyaslama yapılıyor
  if (avgMoisture > activeProfile.moistureCritical) { 
    digitalWrite(PUMP_RELAY_PIN, HIGH); 
    soilStatus = "WATERING";
  } else {
    digitalWrite(PUMP_RELAY_PIN, LOW);  
    soilStatus = "OK";
  }
}

void turnOffAllRelays() {
  digitalWrite(HEATER_RELAY_PIN, LOW);
  digitalWrite(COOLER_RELAY_PIN, LOW);
  digitalWrite(PUMP_RELAY_PIN, LOW);
}

void triggerAlarm() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  soilStatus = "NO WATER!";
}

void updateDisplay() {
  lcd.clear(); 
  
  if (isTankEmpty) {
    lcd.setCursor(0, 0); 
    lcd.print("DIKKAT: SU YOK!");
    lcd.setCursor(0, 1); 
    lcd.print("POMPA DURDURULDU");
  } else {
    // Ekranda önce Mod adı, sonra ortalama nemi gösteriyoruz
    lcd.setCursor(0, 0);
    lcd.print(activeProfile.profileName.substring(0, 6)); // Ekrana sığması için ilk 6 harfi alırız (Örn: Amasya)
    lcd.print(" T:");
    lcd.print(currentTemp, 1); // ,1 ondalık kısmı 1 hane gösterir
    lcd.print("C");
    
    lcd.setCursor(0, 1);
    lcd.print("AvgSoil:");
    lcd.print(avgMoisture);
  }
}