const int trigger = 8; 
const int echo = 7; 
float dist;
long totaltime;

void setup() {
  Serial.begin(9600);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  digitalWrite(trigger, LOW); 
  delayMicroseconds(5);
  
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigger, LOW); 
  totaltime = pulseIn(echo, HIGH);
  dist = totaltime / 58; 
  
  Serial.print("Distance is= "); 
  Serial.print(dist);
  Serial.println(" cm"); 

  delay(100);  // Add a short delay to avoid overwhelming the Serial Monitor
}
