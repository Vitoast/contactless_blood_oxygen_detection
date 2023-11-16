const int fixedFrequencyLow = 3; // Constant frequency for pin 1 in Hz
const int fixedFrequencyHigh = 5; // Constant frequency for pin 1 in Hz

const int sineSteps =  360;
const int waitPeriod = 255;

const float holdLimitLow = 1. / (fixedFrequencyLow * waitPeriod * 0.000001 * sineSteps);
const float holdLimitHigh = 1. / (fixedFrequencyHigh * waitPeriod * 0.000001 * sineSteps);

const int pwmPinLow = 3; // low PWM output pin
const int pwmPinHigh = 5; // high PWM output pin

int sineCounterLow = 0;
int sineCounterHigh = 0;
int holdCounterLow = 0;
int holdCounterHigh = 0;

float sinValueLow = 0;
float sinValueHigh = 0;
long dutyCycleLow = 0;
long dutyCycleHigh = 0;

long curr_time = 0.;
long last_period_time = 0.;

void setup() {
  pinMode(pwmPinLow, OUTPUT);
  digitalWrite(pwmPinLow, LOW);
  pinMode(pwmPinHigh, OUTPUT);
  digitalWrite(pwmPinHigh, LOW);
  Serial.begin(115200); 
  Serial.print("Start PWM with low frequency ");
  Serial.print(fixedFrequencyLow);
  Serial.print(" Hz and high frequency ");
  Serial.print(fixedFrequencyHigh);  
  Serial.print(" Hz\n\r");
  last_period_time = micros();
}

void loop() {
  curr_time = micros() - last_period_time;
  if (curr_time > dutyCycleLow)        // we are in the last 236ms of the wave, vh should be low
    digitalWrite(pwmPinLow, LOW);
  else                         // we are in the first 14ms of the wave, vh should be high
    digitalWrite(pwmPinLow, HIGH);

  if (curr_time > dutyCycleHigh)        // we are in the last 236ms of the wave, vh should be low
    digitalWrite(pwmPinHigh, LOW);
  else                         // we are in the first 14ms of the wave, vh should be high
    digitalWrite(pwmPinHigh, HIGH);

  if (curr_time > waitPeriod) {
    // we should start a new period

    // Generate low sine wave
    if(holdCounterLow < holdLimitLow) {
      holdCounterLow++;   
    } else {
      holdCounterLow = 0;
      sineCounterLow = (sineCounterLow < sineSteps) ? (sineCounterLow + 1) : 0; 
      sinValueLow = sin(radians(sineCounterLow));
      // scale the sine value so it fits the range 0 to 255
      dutyCycleLow = (sinValueLow + 1) / 2 * waitPeriod;
    }

    // Generate high sine wave
    if(holdCounterHigh < holdLimitHigh) {
      holdCounterHigh++;   
    } else {
      holdCounterHigh = 0;
      sineCounterHigh = (sineCounterHigh < sineSteps) ? (sineCounterHigh + 1) : 0; 
      sinValueHigh = sin(radians(sineCounterHigh));
      // scale the sine value so it fits the range 0 to 255
      dutyCycleHigh = (sinValueHigh + 1) / 2 * waitPeriod;
    }

    last_period_time = micros();    
  } 
}
/*
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
*/
/*
const int pwmPinLow = 3; // PWM output pin 1
const int pwmPinHigh = 5; // PWM output pin 2
const int fixedFrequencyLow = 3; // Constant frequency for pin 1 in Hz
const int fixedFrequencyHigh = 5; // Constant frequency for pin 2 in Hz

void setup() {
  pinMode(pwmPinLow, OUTPUT);
  pinMode(pwmPinHigh, OUTPUT);

  // Configure Timer1 for PWM generation on pwmPinLow
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  int prescaler1 = 8; // Adjust prescaler as needed
  int top1 = 16000000 / (2 * prescaler1 * fixedFrequencyLow) - 1;
  ICR1 = top1;

  TCCR1A |= (1 << WGM11) | (1 << COM1A1);
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);

  // Configure Timer2 for PWM generation on pwmPinHigh
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  int prescaler2 = 8; // Adjust prescaler as needed
  int top2 = 16000000 / (2 * prescaler2 * fixedFrequencyHigh) - 1;
  OCR2A = top2;

  TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2A1);
  TCCR2B |= (1 << CS21);

  // Enable Timer1 and Timer2 overflow interrupts (optional)
  //TIMSK1 |= (1 << TOIE1);
  //TIMSK2 |= (1 << TOIE2);
}

void loop() {
  // Your main code here
}*/
