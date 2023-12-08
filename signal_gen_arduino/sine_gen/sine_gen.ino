const int fixedFrequencyLow = 20; // Constant frequency for pin 3 in Hz
const int fixedFrequencyHigh = 20; // Constant frequency for pin 5 in Hz

const int pwmPinLow = 5; // low PWM output pin
const int pwmPinHigh = 6; // high PWM output pin
const float sineSteps =  100.0;
const float maxSineValue = 2.0*PI; //360.0; //2pi
const int waitPeriod = 500; // us

const float holdLimitLow = 1. / (fixedFrequencyLow * waitPeriod * 0.000001 * sineSteps);
const float holdLimitHigh = 1. / (fixedFrequencyHigh * waitPeriod * 0.000001 * sineSteps);


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

/*void setup() {
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
      //sinValueLow = sin(radians((float)sineCounterLow / sineSteps * maxSineValue));
      sinValueLow = sin(sineCounterLow / sineSteps * maxSineValue);
      // scale the sine value so it fits the range
      dutyCycleLow = (sinValueLow + 1) / 2 * waitPeriod;
    }

    // Generate high sine wave
    if(holdCounterHigh < holdLimitHigh) {
      holdCounterHigh++;   
    } else {
      holdCounterHigh = 0;
      sineCounterHigh = (sineCounterHigh < sineSteps) ? (sineCounterHigh + 1) : 0; 
      sinValueHigh = sin(radians((float)sineCounterHigh / sineSteps * maxSineValue));
      // scale the sine value so it fits the range
      dutyCycleHigh = (sinValueHigh + 1) / 2 * waitPeriod;
    }

    last_period_time = micros();    
  } 
}*/
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
int limit1 = 0;
int limit2 = 0;

ISR(TIMER1_COMPA_vect)
{
  int dutyCycle = (sin(radians(sineCounterLow*3.6)) + 1) / 2 * 255; // Map sine value to PWM duty cycle
  analogWrite(pwmPinLow, dutyCycle);
  if(sineCounterLow >= sineSteps) {
    sineCounterLow = 0;
  } else {
    sineCounterLow++;
  }
  OCR1A += limit1;
}

ISR(TIMER2_COMPA_vect)
{
  int dutyCycle = (sin(radians(sineCounterHigh*3.6)) + 1) / 2 * 255; // Map sine value to PWM duty cycle
  analogWrite(pwmPinHigh, dutyCycle);
  if(sineCounterHigh >= sineSteps) {
    sineCounterHigh = 0;
  } else {
    sineCounterHigh++;
  }
  OCR2A += limit2;
}

void setup() {
  pinMode(pwmPinLow, OUTPUT);
  pinMode(pwmPinHigh, OUTPUT);

  // Configure Timer1 for PWM generation on pwmPinLow
  TCCR1A = 0;           // Init Timer1A
  TCCR1B = 0;           // Init Timer1B
  TCCR1B |= B00000010;  // Prescaler = 1
  int prescaler1 = 8;
  int fixedPeriodLow = 1 / fixedFrequencyLow;
  limit1 = 16000000 / (prescaler1 * fixedFrequencyLow * sineSteps) - 1;
  OCR1A = limit1;  // Timer Compare1A Register
  TIMSK1 |= B00000010;  // Enable Timer COMPA Interrupt
  
  // Configure Timer2 for PWM generation on pwmPinHigh
  TCCR2A = 0;           // Init Timer1A
  TCCR2B = 0;           // Init Timer1B
  TCCR2B |= B00000111;  // Prescaler = 1
  int prescaler2 = 1024;
  limit2 = 16000000 / (prescaler2 * fixedFrequencyHigh * sineSteps) - 1;
  OCR2A = limit2;  // Timer Compare1A Register
  TIMSK2 |= B00000010;  // Enable Timer COMPA Interrupt

}

void loop() {
  // Your main code here
}
