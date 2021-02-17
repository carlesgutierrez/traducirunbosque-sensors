/////////////////////////////////////////////////
unsigned long hexToDec(String hexString) {
  unsigned long decValue = 0;
  char nextInt;
  for ( long i = 0; i < hexString.length(); i++ ) {
    nextInt = toupper(hexString[i]);
    if( isxdigit(nextInt) ) {
        if (nextInt >= '0' && nextInt <= '9') nextInt = nextInt - '0';
        if (nextInt >= 'A' && nextInt <= 'F') nextInt = nextInt - 'A' + 10;
        decValue = (decValue << 4) + nextInt;
    }
  }
  return decValue;
}
