const int pwmPin = 3; // PWM output pin
const int fixedFrequency = 1; // Constant frequency in Hz
const int waitDelay = 1./ fixedFrequency * 1000000 / 360; // delay between pwm update in us

void setup() {
  pinMode(pwmPin, OUTPUT);
  Serial.begin(115200); 
  Serial.print("Start PWM with frequency ");
  Serial.print(fixedFrequency);
  Serial.print(" Hz\n\r");
}

void loop() {
  // Generate a sine wave
  for (int i = 0; i < 360; i++) {
    float sinValue = sin(radians(i)); // Calculate sine value
    int dutyCycle = (sinValue + 1) / 2 * 255; // Map sine value to PWM duty cycle
    analogWrite(pwmPin, dutyCycle); // Update PWM output
    delayMicroseconds(waitDelay); // Adjust delay for desired frequency
  }
}

