int digitalPin = 2;   // KY-037 digital interface
int analogPin = A0;   // KY-037 analog interface
int digitalVal;       // digital readings
int analogVal;        // analog readings
void setup()
{
  pinMode(digitalPin,INPUT); 
  pinMode(analogPin, INPUT);
  Serial.begin(9600);
}
void loop()
{
  // Read the digital inteface
  
  // Read analog interface
  analogVal = analogRead(analogPin);
  // Print analog value to serial
  Serial.println(analogVal);  

  delay(100);
}