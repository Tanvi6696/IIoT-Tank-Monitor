#include <WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include <LiquidCrystal.h>

// ============================================================
// WI-FI AND THINGSPEAK
// ============================================================

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

unsigned long THINGSPEAK_CHANNEL_ID = 3464654;
const char* THINGSPEAK_WRITE_API_KEY = "YOUR_THINGSPEAK_API_KEY";

WiFiClient client;

// Numeric data vector
float numericData[3];

// Channel location metadata
const float CHANNEL_LATITUDE = 40.7128;
const float CHANNEL_LONGITUDE = -74.0060;
const float CHANNEL_ELEVATION = 10.0;

// ============================================================
// HC-SR04
// ============================================================

#define TRIG_PIN 5
#define ECHO_PIN 18

// ============================================================
// IR SENSOR
// ============================================================

#define IR_PIN 36

// ============================================================
// 16x2 PARALLEL LCD
// ============================================================

#define LCD_RS 23
#define LCD_E 22
#define LCD_D4 19
#define LCD_D5 21
#define LCD_D6 32
#define LCD_D7 33

// ============================================================
// TANK HEIGHT
// ============================================================

const float TANK_HEIGHT_CM = 100.0;

// ============================================================
// LCD OBJECT
// ============================================================

LiquidCrystal lcd(
  LCD_RS,
  LCD_E,
  LCD_D4,
  LCD_D5,
  LCD_D6,
  LCD_D7
);

// ============================================================
// TIMING
// ============================================================

unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 500;

unsigned long lastThingSpeakUpdate = 0;
const unsigned long THINGSPEAK_INTERVAL = 15000;

// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(9600);

  // ==========================================================
  // HC-SR04
  // ==========================================================

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  // ==========================================================
  // IR SENSOR
  // ==========================================================

  pinMode(IR_PIN, INPUT);

  // ==========================================================
  // LCD
  // ==========================================================

  lcd.begin(16, 2);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Tank Monitor");

  lcd.setCursor(0, 1);
  lcd.print("Connecting WiFi");

  // ==========================================================
  // WI-FI CONNECTION
  // ==========================================================

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // ==========================================================
  // THINGSPEAK
  // ==========================================================

  ThingSpeak.begin(client);

  // Configure ThingSpeak channel location metadata
  ThingSpeak.setLatitude(CHANNEL_LATITUDE);
  ThingSpeak.setLongitude(CHANNEL_LONGITUDE);
  ThingSpeak.setElevation(CHANNEL_ELEVATION);

  delay(1000);

  // ==========================================================
  // SYSTEM READY
  // ==========================================================

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Tank Monitor");

  lcd.setCursor(0, 1);
  lcd.print("System Ready");

  Serial.println("================================");
  Serial.println("IIoT Tank Monitor");
  Serial.println("HC-SR04 + IR + ThingSpeak");
  Serial.println("================================");

  delay(1500);

  lcd.clear();
}

// ============================================================
// HC-SR04 DISTANCE
// ============================================================

float readDistanceCM() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );

  // No echo received
  if (duration == 0) {
    return -1.0;
  }

  float distanceCM = duration * 0.0343 / 2.0;

  return distanceCM;
}

// ============================================================
// LIQUID LEVEL CALCULATION
// ============================================================

float calculateLevel(float distanceCM) {

  if (distanceCM <= 0) {
    return -1.0;
  }

  float levelPercent =
    ((TANK_HEIGHT_CM - distanceCM) / TANK_HEIGHT_CM) * 100.0;

  // Keep value between 0 and 100
  levelPercent = constrain(
    levelPercent,
    0.0,
    100.0
  );

  return levelPercent;
}

// ============================================================
// SEND DATA TO THINGSPEAK
// ============================================================

void sendToThingSpeak(
  float distanceCM,
  float levelPercent,
  int irStatus
) {

  // ==========================================================
  // CHECK WI-FI
  // ==========================================================

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi disconnected. Reconnecting...");

    WiFi.disconnect();
    WiFi.begin(
      WIFI_SSID,
      WIFI_PASSWORD
    );

    return;
  }

  // ==========================================================
  // STORE VALUES IN NUMERIC VECTOR
  // ==========================================================

  numericData[0] = distanceCM;
  numericData[1] = levelPercent;
  numericData[2] = irStatus;

  Serial.println("Numeric data vector:");

  for (int i = 0; i < 3; i++) {

    Serial.print("numericData[");

    Serial.print(i);

    Serial.print("] = ");

    Serial.println(
      numericData[i],
      2
    );
  }

  // ==========================================================
  // THINGSPEAK FIELD 1 & FIELD 2
  // ==========================================================

  if (numericData[0] >= 0) {

    // Field 1 = Distance
    ThingSpeak.setField(
      1,
      numericData[0]
    );

    // Field 2 = Tank Level
    ThingSpeak.setField(
      2,
      numericData[1]
    );

  } else {

    ThingSpeak.setField(
      1,
      -1
    );

    ThingSpeak.setField(
      2,
      -1
    );
  }

  // ==========================================================
  // THINGSPEAK FIELD 3
  // ==========================================================

  // Field 3 = IR status
  ThingSpeak.setField(
    3,
    numericData[2]
  );

  // ==========================================================
  // STATUS MESSAGE
  // ==========================================================

  ThingSpeak.setStatus(
    irStatus == 1
      ? "Object detected by IR sensor"
      : "No object detected"
  );

  // ==========================================================
  // WRITE DATA
  // ==========================================================

  int responseCode = ThingSpeak.writeFields(
    THINGSPEAK_CHANNEL_ID,
    THINGSPEAK_WRITE_API_KEY
  );

  Serial.print("ThingSpeak response code: ");

  Serial.println(responseCode);

  if (responseCode == 200) {

    Serial.println(
      "ThingSpeak update successful!"
    );

  } else {

    Serial.println(
      "ThingSpeak update failed."
    );
  }
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  // ==========================================================
  // SENSOR READ EVERY 500 ms
  // ==========================================================

  if (millis() - lastRead >= READ_INTERVAL) {

    lastRead = millis();

    // ========================================================
    // READ ULTRASONIC SENSOR
    // ========================================================

    float distanceCM =
      readDistanceCM();

    // ========================================================
    // READ IR SENSOR
    // LOW = OBJECT DETECTED
    // HIGH = NO OBJECT
    // ========================================================

    int irStatus =
      digitalRead(IR_PIN) == LOW
      ? 1
      : 0;

    bool objectDetected =
      irStatus == 1;

    // ========================================================
    // CALCULATE TANK LEVEL
    // ========================================================

    float levelPercent =
      calculateLevel(distanceCM);

    // ========================================================
    // LCD LINE 1
    // ========================================================

    lcd.setCursor(0, 0);

    lcd.print(" ");

    lcd.setCursor(0, 0);

    if (distanceCM > 0) {

      lcd.print("D:");
      lcd.print((int)distanceCM);

      lcd.print("cm L:");

      lcd.print((int)levelPercent);

      lcd.print("%");

    } else {

      lcd.print("Ultra ERROR");
    }

    // ========================================================
    // LCD LINE 2
    // ========================================================

    lcd.setCursor(0, 1);

    lcd.print(" ");

    lcd.setCursor(0, 1);

    if (objectDetected) {

      lcd.print("IR: Object");

    } else {

      lcd.print("IR: No Object");
    }

    // ========================================================
    // SERIAL MONITOR
    // ========================================================

    Serial.println(
      "------------------------------"
    );

    if (distanceCM > 0) {

      Serial.print("Distance: ");

      Serial.print(
        distanceCM,
        1
      );

      Serial.println(" cm");

      Serial.print("Level: ");

      Serial.print(
        levelPercent,
        1
      );

      Serial.println(" %");

    } else {

      Serial.println(
        "Ultrasonic: ERROR"
      );
    }

    Serial.print(
      "IR numeric status: "
    );

    Serial.println(
      irStatus
    );

    Serial.print(
      "WiFi RSSI: "
    );

    Serial.println(
      WiFi.RSSI()
    );

    Serial.println(
      "------------------------------"
    );

    // ========================================================
    // THINGSPEAK UPDATE EVERY 15 SECONDS
    // ========================================================

    if (
      millis() - lastThingSpeakUpdate
      >= THINGSPEAK_INTERVAL
    ) {

      lastThingSpeakUpdate =
        millis();

      sendToThingSpeak(
        distanceCM,
        levelPercent,
        irStatus
      );
    }
  }
}
