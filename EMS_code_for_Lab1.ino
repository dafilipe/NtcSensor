/*
  EMS - Electronics for Micro-Systems
  Laboratory 1: Resistive Sensor Interface and Actuation
  Raspberry Pi Pico 2 W - Arduino IDE template

  Main work left for the students:
    1) Convert ADC code -> voltage (check ADC assumptions)
    2) Derive and implement V_AFE -> R_NTC for THEIR AFE
    3) Implement the Beta-model conversion R_NTC -> temperature
    4) Select relay hysteresis thresholds
    5) Optional: add the soil-moisture channel

  The acquisition loop, serial protocol and relay state machine are provided.
*/
#include <Arduino.h>
#include <math.h>
// -----------------------------------------------------------------------------
// Hardware configuration
// -----------------------------------------------------------------------------
constexpr uint8_t PIN_NTC_ADC = 26;       // GP26 / ADC0
constexpr uint8_t PIN_SOIL_ADC = 27;      // GP27 / ADC1 (optional extension)
constexpr uint8_t PIN_RELAY = 15;         // GPIO driving the relay transistor; you can select another port

constexpr bool RELAY_ACTIVE_HIGH = true;

// Pico ADC specs.
constexpr float ADC_VREF = 3.3f;
constexpr uint16_t ADC_MAX_CODE = 4095;   // 12-bit result

// Sampling/display rate. This is low frequency lab.
constexpr uint32_t SAMPLE_PERIOD_MS = 250;

// -----------------------------------------------------------------------------
// NTC parameters
// -----------------------------------------------------------------------------
constexpr float NTC_R0 = 10000.0f;        // ohm
constexpr float NTC_T0_K = 25.0f + 273.15f;
constexpr float NTC_BETA = 3380.0f;       // TODO: replace using the datasheet
constexpr float AFE_GAIN = 4.265f;
const float Vdd = 3.3f;
const float R1 = 10000.0f;
const float Tmax = 50.0f;

// -----------------------------------------------------------------------------
// Relay control
// 
// Hysteresis ??? 
// -----------------------------------------------------------------------------
float T_ON_C  = 30.0f;                    // TODO: justify/select
float T_OFF_C = 28.0f;                    // TODO: justify/select

bool relayState = false;

// -----------------------------------------------------------------------------
// Optional soil-moisture extension
// -----------------------------------------------------------------------------
constexpr bool ENABLE_SOIL = false;

// -----------------------------------------------------------------------------
// Some helpful functions
// -----------------------------------------------------------------------------
float Kelvin_to_Celsius(float Kelvin) {
    return Kelvin - 273.15f;
}

void setRelay(bool on)
{
  relayState = on;

  if (RELAY_ACTIVE_HIGH) {
    digitalWrite(PIN_RELAY, on ? HIGH : LOW);
  } else {
    digitalWrite(PIN_RELAY, on ? LOW : HIGH);
  }
}

float adcCodeToVoltage(uint16_t VinBit)
{
  /*
    TODO - STUDENT TASK
    Convert ADC code into voltage.
    Start from the ADC model discussed in class and check the assumptions used
    by ADC_VREF and ADC_MAX_CODE above.
  */
   return (VinBit * ADC_VREF) / ADC_MAX_CODE;
}

float afeVoltageToNtcResistance(float vAfe)
{
  /*
    TODO - STUDENT TASK
    Derive this equation from YOUR analog front-end.
    The laboratory does NOT prescribe one unique AFE topology. Therefore, this
    function must be written from the transfer characteristic obtained during
    the design phase.
    Input:
      vAfe  - measured AFE output voltage [V]
    Return:
      NTC resistance [ohm]
    Important:
      - reject impossible values;
      - avoid division by zero;
      - the equation must match the circuit actually built.
  */
  // 1. Rejeitar valores impossíveis ou divisão por zero logo no início
  if (vAfe <= 0.0f || vAfe >= Vdd || !isfinite(vAfe)) {
      return NAN;
  }

  float vDivisor = vAfe / AFE_GAIN;

  if (vDivisor <= 0.0f || vDivisor >= Vdd) {
      return NAN;
  }

  return (R1 * vDivisor) / (Vdd - vDivisor);
}

/*------------------------------------------------------------------------------------------*/
float ntcResistanceToTemperatureC(float rNtc)
{
  /*
    TODO - STUDENT TASK
    Implement the inverse Beta model:
      T = 1 / [ 1/T0 + (1/BETA) ln(R/R0) ]
    T is obtained in kelvin and must be converted to degrees Celsius.
  */
  if (rNtc <= 0.0f || !isfinite(rNtc)) {
      return NAN;
  }
  float T = 0;
  T = 1 /(1/NTC_T0_K + (1/NTC_BETA)*log(rNtc/NTC_R0));
  return Kelvin_to_Celsius(T);
}

void updateRelay(float temperatureC)
{
  // Hysteresis state machine is provided.
  // Students choose and justify T_ON_C and T_OFF_C.
  if (!isfinite(temperatureC)) {
    setRelay(false);
    return;
  }

  if (!relayState && temperatureC >= T_ON_C) {
    setRelay(true);
  }
  else if (relayState && temperatureC <= T_OFF_C) {
    setRelay(false);
  }
}

float calculateSoilPercent(uint16_t adcCode)
{
  /*
    OPTIONAL EXTENSION (>17.5/20)

    Define a relative 0-100% moisture indication using experimentally obtained
    "dry" and "wet" reference conditions.

    This is NOT an absolute soil-water-content measurement.
  */
  (void)adcCode;
  return NAN;
}
// -----------------------------------------------------------------------------
// Serial protocol
//
// One CSV line is transmitted every SAMPLE_PERIOD_MS:
//
// DATA,time_ms,adc_ntc,v_ntc,r_ntc,temp_c,relay,adc_soil,soil_pct
//
// Example:
// DATA,1250,2148,1.731,9284.2,26.1,0,-1,nan
//
// Soil fields are -1 / nan when the optional extension is disabled.
// -----------------------------------------------------------------------------
void sendData(uint32_t timeMs, uint16_t adcNtc, float vNtc, float rNtc,float tempC,int soilAdc,float soilPct)
{
  Serial.print("DATA,");
  Serial.print(timeMs);
  Serial.print(",");
  Serial.print(adcNtc);
  Serial.print(",");
  Serial.print(vNtc, 4);
  Serial.print(",");
  Serial.print(rNtc, 1);
  Serial.print(",");
  Serial.print(tempC, 2);
  Serial.print(",");
  Serial.print(relayState ? 1 : 0);
  Serial.print(",");
  Serial.print(soilAdc);
  Serial.print(",");
  Serial.println(soilPct, 1);
}
// -----------------------------------------------------------------------------
// Setup / loop
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);

  pinMode(PIN_RELAY, OUTPUT);
  setRelay(false);

  analogReadResolution(12);

  // Give the USB serial connection a short time to "start"...
  delay(1000);

  Serial.println("# EMS Lab 1 - Pico 2 W");
  Serial.println("# Protocol:");
  Serial.println("# DATA,time_ms,adc_ntc,v_ntc,r_ntc,temp_c,relay,adc_soil,soil_pct");
}
// ---------- MAIN LOOP --------------------------------
void loop()
{
  static uint32_t previousMs = 0;
  const uint32_t now = millis();

  if (now - previousMs < SAMPLE_PERIOD_MS) {
    return;
  }
  previousMs = now;

  // Mandatory NTC acquisition
  const uint16_t adcNtc = analogRead(PIN_NTC_ADC);
  const float vNtc = adcCodeToVoltage(adcNtc);
  const float rNtc = afeVoltageToNtcResistance(vNtc);
  const float tempC = ntcResistanceToTemperatureC(rNtc);

  updateRelay(tempC);

  // Optional soil-moisture channel
  int soilAdc = -1;
  float soilPct = NAN;

  if (ENABLE_SOIL) {
    soilAdc = analogRead(PIN_SOIL_ADC);
    soilPct = calculateSoilPercent((uint16_t)soilAdc);
  }

  sendData(now, adcNtc, vNtc, rNtc, tempC, soilAdc, soilPct);
}
