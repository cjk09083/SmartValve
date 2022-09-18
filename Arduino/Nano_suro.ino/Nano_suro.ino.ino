//백업용 파일 20210307 //

#include<Wire.h>
void setup() {
  
Serial.begin(9600);
Wire.begin(1);
Wire.onRequest(requestEvent);
Serial.print("STATE");
}

void loop() {
}
void requestEvent(){
  int charge_value =analogRead(A1);
  //charge_value = 324;
  char fol = char(int(charge_value/1000));
  char thr = char((fol*10-int(charge_value/100))*-1);
  char two = char((fol*100+thr*10-int(charge_value/10))*-1);
  char one = char((fol*1000+thr*100+two*10-charge_value)*-1);
  Wire.write(fol); 
  Wire.write(thr); 
  Wire.write(two); 
  Wire.write(one); 
  Serial.print(int(fol));
  Serial.print(int(thr));
  Serial.print(int(two));
  Serial.print(int(one));

  Wire.write(',');
  Serial.print(',');

  int bat_value =analogRead(A0);
//  bat_value = 516;
  fol=char(int(bat_value/1000));
  thr = char((fol*10-int(bat_value/100))*-1);
  two = char((fol*100+thr*10-int(bat_value/10))*-1);
  one = char((fol*1000+thr*100+two*10-bat_value)*-1);
  Wire.write(fol); 
  Wire.write(thr); 
  Wire.write(two); 
  Wire.write(one); 
  Serial.print(int(fol));
  Serial.print(int(thr));
  Serial.print(int(two));
  Serial.println(int(one));
  
}
