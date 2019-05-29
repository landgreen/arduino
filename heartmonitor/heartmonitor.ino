const int sum = 512;

void setup() {
//  57600
  Serial.begin(9600);
}

void loop() {
  int avg = 0;
  for (int i = 0; i < sum; i++) {
    avg += analogRead(A0);
  }
  Serial.println(avg / sum);
  //  delay(5);
}
