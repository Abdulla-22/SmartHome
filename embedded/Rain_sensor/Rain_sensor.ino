int sensorRead = 0;
int sensorPin= 2;

void setup() {
	Serial.begin(9600);
	pinMode(A0, INPUT);
  pinMode(sensorPin, INPUT);
}

void loop() {
	int sensorRead= digitalRead(sensorPin);
	Serial.println(sensorRead);
}