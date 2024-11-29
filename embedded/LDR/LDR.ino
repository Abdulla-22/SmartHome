int buzzer =2;

void setup() {

  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);

}

void loop() {

  tone(buzzer, 1000);  // Play a 1000 Hz tone


}