const int adcPin = 0;

int VinBit = 0;
float AVin = 0.0f;
float Vin = 0.0f;
float Rntc = 0.0f;

const float A = 1.129148e-03f;
const float betaNTC_1DC103K = 3965.0f;
const float T0 = 25.0f;
const float T0K = T0 + 273.15f;
const float Vdd = 6.0f;
const float R1 = 10000.0f;
const float Tmax = 15.0f;


//funcoes auxiliares
float Kelvin_to_celsius(float Kelvin) {
    return Kelvin - 273.15f;
}

float Celsius_to_Kelvin(float Celsius) {
    return Celsius + 273.15f;
}

float calc_resistor(float R1, float Vout, float Vdd) {
    return (R1 * Vout) / (Vdd - Vout);
}
//calcula a tensão de entrada a partir do valor lido pelo ADC
float calc_voltage(int VinBit) {
    return (VinBit * 3.3f) / 4095.0f;
}

void setup() {
    Serial.begin(115200);
}

void loop() {
    VinBit = analogRead(adcPin);
    AVin = calc_voltage(VinBit);
    Vin = AVin / A;
    Rntc = calc_resistor(R1, Vin, Vdd);

    Serial.print("ADC: ");
    Serial.print(VinBit);
    Serial.print(" | Vout: ");
    Serial.print(AVin, 4);
    Serial.print(" V | Vin: ");
    Serial.print(Vin, 4);
    Serial.print(" V | Rntc: ");
    Serial.print(Rntc, 2);
    Serial.println(" Ohm");

    //calcular a temperatura em Kelvin usando a equação 
    delay(500);
}