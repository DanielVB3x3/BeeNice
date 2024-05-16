#include <Wire.h>
#include <Adafruit_CCS811.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HX711.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_PIN_1 4
#define LED_PIN_2 16
#define LED_PIN_3 17

#define CCS811_ADDR 0x5A // Default I2C address
Adafruit_CCS811 ccs;

#define DOUT 13
#define CLK 12
HX711 scale;

float temperature, humidity, voc, weight;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);

  if (!ccs.begin(CCS811_ADDR)) {
    Serial.println("Failed to start CCS811 sensor! Check wiring.");
    while (1);
  }
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare(); // Reset the scale to 0
}

void loop() {
  if (!ccs.readData()) {
    Serial.println("Failed to read data from CCS811 sensor!");
    return;
  }
  if (!ccs.available()) {
    Serial.println("Data not available from CCS811 sensor!");
    return;
  }
  temperature = ccs.calculateTemperature();
  humidity = ccs.calculateHumidity();
  voc = ccs.getTVOC();

  weight = scale.get_units(10); // Get weight measurement
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");

  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");

  display.print("VOC: ");
  display.print(voc);
  display.println(" mg/m3");

  display.print("Weight: ");
  display.print(weight);
  display.println(" g");

  display.display();

  // Control LED lights based on sensor readings
  if (temperature < 25) {
    digitalWrite(LED_PIN_1, HIGH);
    delay(500);
    digitalWrite(LED_PIN_1, LOW);
    delay(500);
  } else {
    digitalWrite(LED_PIN_1, LOW);
  }

  if (humidity < 50) {
    digitalWrite(LED_PIN_2, HIGH);
    delay(500);
    digitalWrite(LED_PIN_2, LOW);
    delay(500);
  } else {
    digitalWrite(LED_PIN_2, LOW);
  }

  if (weight > 50000) {
    digitalWrite(LED_PIN_3, HIGH);
    delay(500);
    digitalWrite(LED_PIN_3, LOW);
    delay(500);
  } else {
    digitalWrite(LED_PIN_3, LOW);
  }

  // Turn on LED if weight is below 20000
  if (weight < 20000) {
    digitalWrite(LED_PIN_1, HIGH);
  }

  // Turn on LED if VOC is above 1
  if (voc > 1) {
    digitalWrite(LED_PIN_2, HIGH);
  }

  delay(60000); // Wait for 1 minute
}
