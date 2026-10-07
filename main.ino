int Kelvin_to_celsius (int Kelvin) {
    return (Kelvin - 273.15);
}

int Celsius_to_Kelvin (int Celsius) {
    return (Celsius + 273.15);
}

void setup() {
    const int betaNTC_1DC103K-EC = 3965 ; // +- 0.75% (importante para monteCarlo)
    const int T0 = 25; // celcius
    const int T0K = Kelvin_to_celsius(T0);
    

}

void loop() {
  



}\