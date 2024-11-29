// C++ code
//

int input= 2;

void setup()
{
  pinMode(input, INPUT);
  
  Serial.begin(9600);
}

void loop()
{
  int motion = digitalRead(input);

  Serial.println(motion);

  delay(200);
 
}