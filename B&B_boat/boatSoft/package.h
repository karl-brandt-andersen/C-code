struct package
{
  char x;
  char y;
  char degree;  //char is 0 til 250 - 1 byte
  signed char tempSen1; //signed is -127 til 128
  signed char tempSen2;
  bool engineFail; // 1 bit
  bool light;
  
} Package;
