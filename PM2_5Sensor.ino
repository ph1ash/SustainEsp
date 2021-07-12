/* Waveshare Dust Sensor tested with Arduino UNO, Michael H March 30, 2021
  VCC red => 3.3V
  GND black => GND
  AOUT blue => A0
  ILED yellow => D12
*/

#define AOUT A0
#define ILED 12

long int systemTime;    // replaces delay()
int AoutMin = 150;      // equal to Aout min in mV for clean air (was 360)
int AoutMax = 500;      // equal to Aout max in mV (toothpick in measuring window) (was 3600)
float dustConst;        // it will be computed from Aout range and Sharp sensor range 0-500 ug/m3
int dustConc;           // dust concentration in ug/m3/mV
int sum10;     // sum of 10 counts
int AoutAvg;            // direct Sharp Vout in mV computed as average from 11 measurements
int dustQualityIndex;   // it is quite arbitrary
char *dustQualityChar[] = {"1.Excellent", "2.Very good", "3.Good", "4.Fair", "5.Poor"};

void initPM2_5Sensor() {
  pinMode(AOUT, INPUT);  // pin selected for 3.3V, GND, AOUT, and ILED
  pinMode(ILED, OUTPUT); // in order RED, BLACK, BLUE, and YELLOW as on WaveShare board
  analogRead(AOUT);       // activate IOref pin, 1.1V will be present
}

unsigned int readPM2_5Sensor() {
  sum10 = 0;
  for (int i = 0; i < 10; i++) {
    //Sharp datasheet: pulse cycle 10ms, pulse width 0.32ms, sampling at 0,28ms
    digitalWrite(ILED, HIGH);
    delayMicroseconds(280);
    int reading = analogRead(AOUT);
    sum10 += reading;
    Serial.print(reading);
    Serial.print(" , ");
    delayMicroseconds(40);   // 280+40=320
    digitalWrite(ILED, LOW);
    delayMicroseconds(9680); // 320 + 9680 = 10000us
  }
  Serial.println();
  if ((millis() - systemTime) > 1000) {
    int q = computeAirQuality();
    systemTime = millis();
    return q;
  }
  return 0;
}

float computeAirQuality()
{
  // mV & magic number pulled from manual analog readings: Linear interp
  // (500-0)/(500-150) * (x-150) -> (500/350) * (x-150)-> 1.428 * (x-150)
  // Tried a few numbers, ratio of 1.25 seems to be _most_ accurate
  int q, i;
  q = floor(1.25f * float((sum10/10) - 100)); // ug/m3
  if (q < 0) q = 0; // handle non positive values

  if (q < 40) i = 0;
  else if (q < 80) i = 1;
  else if (q < 160) i = 2;
  else if (q < 320) i = 3;
  else i = 4;

  dustConc = q; dustQualityIndex = i;
  printAirQuality(q);
  return q;
}

void printAirQuality(int dustConcentration)
{
  Serial.print(" Sum_10 ");
  Serial.print(sum10/10);
  Serial.print(" \t ");
  Serial.print(dustConcentration);
  Serial.print(" ug/m3\t\t");
  Serial.print(dustQualityChar[dustQualityIndex]);
  Serial.print(" air quality\n");
} 
