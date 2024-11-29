int buzzer =2;

void setup() {

  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);

}

void loop() {

  tone(buzzer, 100);  // Play a 100 Hz tone


}