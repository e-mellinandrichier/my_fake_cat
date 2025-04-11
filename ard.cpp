

const int IR_PIN = 11;       
const int LED_BERCEUSE = 10;
const int LED_OEIL_1 = 5;
const int LED_OEIL_2 = 6;
const int BUZZER_PIN = 7;
const int PHOTO_PIN = A0;

unsigned long lastIRTime = 0;
boolean ledBerceuseState = false; 
boolean irActive = false; 

int berceuseBrightness = 0;   
int fadeAmount = 3;
unsigned long previousMillis = 0;
const long interval = 50;


const int LIGHT_MAX = 500; 
const int LIGHT_MIN = 300;
int eyeBrightness = 255; 
boolean eyesCompletelyOff = false;
const int EYE_OFF_THRESHOLD = 20;

int snoreFrequency = 100;   
int snoreVolume = 0; 
int snoreDirection = 1;
unsigned long buzzerPreviousMillis = 0;
const long buzzerInterval = 70;
boolean buzzerEnabled = false;

void setup() {
  Serial.begin(9600);
  

  pinMode(LED_BERCEUSE, OUTPUT);
  pinMode(LED_OEIL_1, OUTPUT);
  pinMode(LED_OEIL_2, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PHOTO_PIN, INPUT);
  

  analogWrite(LED_BERCEUSE, 0);
  analogWrite(LED_OEIL_1, 255);
  analogWrite(LED_OEIL_2, 255);
  
}

void loop() {

  int lightLevel = analogRead(PHOTO_PIN);
  int newEyeBrightness = map(constrain(lightLevel, LIGHT_MIN, LIGHT_MAX), 
                            LIGHT_MIN, LIGHT_MAX, 
                            0, 255);
  

  if (newEyeBrightness != eyeBrightness) {

    if (newEyeBrightness > eyeBrightness) {
      eyeBrightness = min(eyeBrightness + 3, newEyeBrightness);
    } else {
      eyeBrightness = max(eyeBrightness - 3, newEyeBrightness);
    }
    

    analogWrite(LED_OEIL_1, eyeBrightness);
    analogWrite(LED_OEIL_2, eyeBrightness);

  }
  

  boolean newEyeState = (eyeBrightness <= EYE_OFF_THRESHOLD);  
  
  if (newEyeState != eyesCompletelyOff) {
    eyesCompletelyOff = newEyeState;
    
    if (eyesCompletelyOff) {
      buzzerEnabled = true;
    } else {
      buzzerEnabled = false;
      noTone(BUZZER_PIN);
    }
  } 
  

  int irValue = digitalRead(IR_PIN);
  

  if (irValue == LOW && !irActive) {
    if (millis() - lastIRTime > 300) {
      ledBerceuseState = !ledBerceuseState;
      
      if (!ledBerceuseState) {
        analogWrite(LED_BERCEUSE, 0);
        berceuseBrightness = 0;
      }
    
      
      lastIRTime = millis();
    }
    irActive = true;
  }

  if (irValue == HIGH && irActive) {
    irActive = false;
  }
  
  if (ledBerceuseState) {
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      
      berceuseBrightness = berceuseBrightness + fadeAmount;
      
      if (berceuseBrightness <= 5) {
        berceuseBrightness = 5;
        fadeAmount = -fadeAmount;
        delay(400);
      }
      if (berceuseBrightness >= 255) {
        berceuseBrightness = 255;
        fadeAmount = -fadeAmount;
        delay(400);
      }
      
      analogWrite(LED_BERCEUSE, berceuseBrightness);
    }
  }
  
  if (buzzerEnabled && eyesCompletelyOff) {
    unsigned long currentMillis = millis();
    
    if (currentMillis - buzzerPreviousMillis >= buzzerInterval) {
      buzzerPreviousMillis = currentMillis;
      
      snoreVolume = snoreVolume + (snoreDirection * 2);
      
      if (snoreVolume <= 0 || snoreVolume >= 20) {
        snoreDirection = -snoreDirection;
        snoreVolume = constrain(snoreVolume, 0, 20);
        

        if (snoreVolume <= 0) {
          snoreFrequency = random(80, 120);
          delay(random(1000, 2000));
        }
      }
      
      if (snoreVolume > 0) {
        tone(BUZZER_PIN, snoreFrequency + (snoreVolume * 2), buzzerInterval);
      } else {
        noTone(BUZZER_PIN);
      }
    }
  }
  
  delay(20);
}