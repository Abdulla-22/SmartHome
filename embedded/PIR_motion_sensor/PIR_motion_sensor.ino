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
  int flame = digitalRead(input);

  if(flame == HIGH){
    Serial.println("Fire !!");
  }
  else{
    Serial.println("No Fire");

  }
 
}