int ledPin = 2; // The digital pin where the LED is connected

void setup()
{
    pinMode(ledPin, OUTPUT); // Set the LED pin as output
}

void loop()
{
    digitalWrite(ledPin, HIGH); // Turn the LED on
    delay(1000);                // Wait for a second
    digitalWrite(ledPin, LOW);  // Turn the LED off
    delay(1000);                // Wait for a second
}
