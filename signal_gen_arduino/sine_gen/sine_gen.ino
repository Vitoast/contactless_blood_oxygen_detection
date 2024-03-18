/***
@author Tobias Steinbach
Part of the project seminar "Contactless Blood Oxygen Detection"

  This code can be used to create 2 independent PWM outputs on pins 5 and 6 of an Arduino Uno.
  A sine wave is generated with a period length based on the fixed_frequency_XX variables.
  Since this is inteded to be used with orange and infrared LED arrays, the naming is _OR and _IR respectively.
  The frequencies can be whole numbers from 1 to 20.
  To create the signal the Timers 1 and 2 of the Arduino are used.
  Information about the state of the program can be printed out with the serial interface.
***/

const int fixed_frequency_IR = 5; // Constant frequency for pin 3 in Hz
const int fixed_frequency_OR = 10; // Constant frequency for pin 5 in Hz

const int pwm_pin_IR = 5; // low PWM output pin
const int pwm_pin_OR = 6; // high PWM output pin
const float sine_steps =  100.0; // define resolution of sine wave

int sine_counter_IR = 0; // Used to compute sine values from steps 1 to 100
int sine_counter_OR = 0;

int limit_timer_1 = 0; // Limits for the Timer used for counting
int limit_timer_2 = 0;

unsigned long last_period_time_IR = 0;
unsigned long last_period_time_OR =0;

// ISR for handling timer events that update the IR PWM
ISR(TIMER1_COMPA_vect)
{
  // Calculate new duty cycle based on counter
  int dutyCycle = (sin(radians(sine_counter_IR*3.6)) + 1) / 2 * 255; // Map sine value to PWM duty cycle
  analogWrite(pwm_pin_IR, dutyCycle);
  if(sine_counter_IR >= (sine_steps-1)) {
    // Use this to debug period time
    /*Serial.print(micros() - last_period_time_IR);  
    Serial.print(" µs in low frequency\n\r");
    last_period_time_IR = micros();*/
    sine_counter_IR = 0;   
  } else {
    sine_counter_IR++;
  }
  OCR1A += limit_timer_1;  
}

// ISR for handling timer events that update OR PWM
ISR(TIMER2_COMPA_vect)
{
  // Calculate new duty cycle based on counter
  int dutyCycle = (sin(radians(sine_counter_OR*3.6)) + 1) / 2 * 255; // Map sine value to PWM duty cycle
  analogWrite(pwm_pin_OR, dutyCycle);
  if(sine_counter_OR >= (sine_steps-1)) {
    // Use this to debug period time
    /*Serial.print(micros() - last_period_time_OR);  
    Serial.print(" µs in high frequency\n\r");
    last_period_time_OR = micros();*/
    sine_counter_OR = 0;    
  } else {
    sine_counter_OR++;
  }
  OCR2A += limit_timer_2;
}

// Initialize program
void setup() {

  pinMode(pwm_pin_IR, OUTPUT);
  pinMode(pwm_pin_OR, OUTPUT);
  // Print out general information via UART
  Serial.begin(115200); 
  Serial.print("Start PWM with low frequency ");
  Serial.print(fixed_frequency_IR);
  Serial.print(" Hz and high frequency ");
  Serial.print(fixed_frequency_OR);  
  Serial.print(" Hz\n\r");

  // Configure Timer1 for PWM generation on pwm_pin_IR
  TCCR1A = 0;           // Init Timer1A
  TCCR1B = 0;           // Init Timer1B
  // Adjust prescaler based on desired frequency
  int prescaler_timer_1 = 0;
  if (fixed_frequency_IR < 15) {
    prescaler_timer_1 = 8;
    TCCR1B |= B00000010;
    
  } else {
    prescaler_timer_1 = 1;
    TCCR1B |= B00000001;
  }
  limit_timer_1 = 16000000 / (prescaler_timer_1 * fixed_frequency_IR * sine_steps) - 1;
  OCR1A = limit_timer_1;  // Timer Compare1A Register
  TIMSK1 |= B00000010;  // Enable Timer COMPA Interrupt
  
  // Configure Timer2 for PWM generation on pwm_pin_OR
  TCCR2A = 0; // Init Timer2A
  TCCR2B = 0;
  // Adjust Prescaler based on desired frequency
  int prescaler_timer_2 = 0;  // Init Timer2B
  if (fixed_frequency_OR < 3) {
    TCCR2B |= B00000111;
    prescaler_timer_2 = 1024;
  }
  else if (fixed_frequency_OR < 6) {
    TCCR2B |= B00000110;
    prescaler_timer_2 = 256;
  }
  else {
    TCCR2B |= B00000101;
    prescaler_timer_2 = 128;
  }
  limit_timer_2 = 16000000 / (prescaler_timer_2 * fixed_frequency_OR * sine_steps) - 1;
  OCR2A = limit_timer_2;  // Timer Compare1A Register
  TIMSK2 |= B00000010;  // Enable Timer COMPA Interrupt
}

void loop() {
  // Empty for now
}
