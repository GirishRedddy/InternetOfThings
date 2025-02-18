#include <DHT.h>  // Include the DHT library for sensor functionality
#define DHT11_PIN 4  // Pin where the DHT11 sensor is connected
#define ledPin 2  // Pin where the LED is connected

DHT dht11(DHT11_PIN, DHT11);  // Initialize the DHT11 sensor object

bool Flag = true;  // Flag to control LED state
int LEDMODE = 0;  // Variable to control LED behavior (on, off, or flash)
float minTempThreshold = 15.0;  // Default minimum temperature threshold in Celsius
float minHumiThreshold = 30.0;  // Default minimum humidity threshold in percentage2
float maxTempThreshold = 30.0;  // Default maximum temperature threshold in Celsius
float maxHumiThreshold = 70.0;  // Default maximum humidity threshold in percentage
bool monitoring = false;  // Flag to control monitoring state (start/stop)

void setup() {
  Serial.begin(9600);  // Initialize serial communication at 9600 baud rate
  dht11.begin();  // Initialize the DHT11 sensor
  pinMode(ledPin, OUTPUT);  // Set the LED pin as an output
  digitalWrite(ledPin, LOW);  // Turn the LED off initially

  // Display initial instructions to the user on the serial monitor
  Serial.println("ESP32 Temperature and Humidity Monitoring");
  Serial.println("Commands:");
  Serial.println("1 - Start/Stop Monitoring");
  Serial.println("2 - Set Custom Temperature Threshold");
  Serial.println("3 - Set Custom Humidity Threshold");
  Serial.println("4 - View Current Thresholds");
  Serial.println("5 - Exit");
}

void printTempHumi(float humi, float tempC, float tempF) {
  // Print the humidity and temperature readings in both Celsius and Fahrenheit
  Serial.print("Humidity: ");
  Serial.print(humi);
  Serial.print("%");

  Serial.print("  |  ");

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.print("°C  ~  ");
  Serial.print(tempF);
  Serial.println("°F");
}

void clearBuffer(){
  // Clear any data left in the serial buffer to ensure correct reading of new input
  while(Serial.available() > 0){
    Serial.read();
  }
}

void setupLED() {
  // Control the LED based on the current LEDMODE value
  if (LEDMODE == 1) {
    digitalWrite(ledPin, HIGH);  // Turn LED on for high temperature/humidity
  } else if (LEDMODE == 2) {
    digitalWrite(ledPin, HIGH);  // Flash LED for low temperature/humidity
    delay(1000);  // Wait for 1 second
    digitalWrite(ledPin, LOW);  // Turn LED off
  } else if (LEDMODE == 3) {
    digitalWrite(ledPin, LOW);  // Turn LED off for normal temperature/humidity
  }
  delay(1000);  // Wait for 1 second before continuing
}

void loop() {
  // Check if there is any input from the serial monitor
  if (Serial.available()) {
    int command = Serial.parseInt();  // Parse the integer command from the user
    clearBuffer();  // Clear the buffer to ensure the next input is read correctly

    // Handle different commands based on user input
    switch (command) {
      case 1:
        // Toggle monitoring on/off
        monitoring = !monitoring;
        if (monitoring) {
          Serial.println("Monitoring started...");  // Print message to indicate monitoring started
        } else {
          Serial.println("Monitoring stopped...");  // Print message to indicate monitoring stopped
        }
        break;
      
      case 2:
        // Set custom temperature threshold
        Serial.println("Enter the new Minimum temperature threshold (in Celsius): ");
        clearBuffer();  // Clear the buffer before taking user input
        while (Serial.available() == 0) {  }  // Wait until the user enters a value
        minTempThreshold = Serial.parseFloat();  // Parse the new minimum temperature threshold
        clearBuffer();  // Clear the buffer
        Serial.println("Enter the new Maximum temperature threshold (in Celsius): ");
        clearBuffer();
        while (Serial.available() == 0) {  }  // Wait for user input
        maxTempThreshold = Serial.parseFloat();  // Parse the new maximum temperature threshold
        clearBuffer();
        // Display the updated temperature thresholds
        Serial.print("Temperature threshold set to: ");
        Serial.print("Minimum : "); Serial.print(minTempThreshold);
        Serial.print("\tMaximum : "); Serial.print(maxTempThreshold);
        Serial.println("°C");
        break;
      
      case 3:
        // Set custom humidity threshold
        Serial.println("Enter the new Minimum humidity threshold (in %): ");
        clearBuffer();
        while (Serial.available() == 0) {  }
        minHumiThreshold = Serial.parseFloat();  // Parse the new minimum humidity threshold
        clearBuffer();
        Serial.println("Enter the new Maximum humidity threshold (in %): ");
        clearBuffer();
        while (Serial.available() == 0) {  }
        maxHumiThreshold = Serial.parseFloat();  // Parse the new maximum humidity threshold
        clearBuffer();
        // Display the updated humidity thresholds
        Serial.print("Humidity threshold set to: ");
        Serial.print("Minimum : "); Serial.print(minHumiThreshold);
        Serial.print("\tMaximum : "); Serial.print(maxHumiThreshold);
        Serial.println("%");
        break;
      
      case 4:
        // View current threshold settings
        Serial.print("Current Temperature Threshold: ");
        Serial.print("Minimum : "); Serial.print(minTempThreshold);
        Serial.print("\tMaximum : "); Serial.print(maxTempThreshold);
        Serial.println("°C");
        Serial.print("Current Humidity Threshold: ");
        Serial.print("Minimum : "); Serial.print(minHumiThreshold);
        Serial.print("\tMaximum : "); Serial.print(maxHumiThreshold);
        Serial.println("%");
        break;

      case 5:
        // Exit the program
        Serial.println("Exiting...");
        while (true) {}  // Halt the loop indefinitely
        break;
      
      default:
        // Handle invalid commands
        Serial.println("Invalid command. Please try again.");
        break;
    }
  }

  // Monitoring logic: Only if monitoring flag is true
  if (monitoring) {
    // Read humidity and temperature from the DHT11 sensor
    float humi = dht11.readHumidity();
    float tempC = dht11.readTemperature();  // Temperature in Celsius
    float tempF = dht11.readTemperature(true);  // Temperature in Fahrenheit

    setupLED();  // Control LED based on the thresholds

    // Check if sensor reading failed
    if (isnan(tempC) || isnan(tempF) || isnan(humi)) {
      Serial.println("Failed to read from DHT11 sensor!");  // Print error message if sensor fails
    } else {
      printTempHumi(humi, tempC, tempF);  // Display the current readings

      // Compare the readings with the thresholds and update LEDMODE accordingly
      if (tempC > maxTempThreshold || humi > maxHumiThreshold) {
        LEDMODE = 1;  // Turn LED ON for high temperature/humidity
      } else if (tempC < minTempThreshold || humi < minHumiThreshold) {
        LEDMODE = 2;  // Flash LED for low temperature/humidity
      } else {
        LEDMODE = 3;  // Turn LED OFF for moderate temperature/humidity
      }
    }
  }
}
