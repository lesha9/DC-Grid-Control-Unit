#include <Wire.h>
#include <INA3221.h>
#include "X9C10X.h"

#define SERIAL_SPEED     115200  // serial baud rate
#define PRINT_DEC_POINTS 3       // decimal points to print

// Set I2C address to 0x41 (A0 pin -> VCC)
INA3221 ina3221(INA3221_ADDR40_GND);

X9C10X pot_up_voltage(10000);
X9C10X pot_up_current(10000);
// X9C10X pot_dw_voltage(10000);
X9C10X pot_dw_current(10000);

uint8_t direction = LOW;
uint8_t step = 1;

void setup() 
{
    Serial.begin(SERIAL_SPEED);

    while (!Serial) {
        delay(1);
    }
    ina3221.begin();
    ina3221.reset();

    // Set shunt resistors to 10 mOhm for all channels
    ina3221.setShuntRes(100, 100, 100); //номиналы резисторов шунта в мОмах
    // Set series filter resistors to 10 Ohm for all channels.
    // Series filter resistors introduce error to the current measurement.
    // The error can be estimated and depends on the resitor values and the bus
    // voltage.
    ina3221.setFilterRes(100, 100, 100); //номиналы резисторов фильтра в Омах

    Serial.println();
    Serial.print("X9C10X_LIB_VERSION: ");
    Serial.println(X9C10X_LIB_VERSION);

    pot_up_voltage.begin(25, 26, 14);  // pulse, direction, select 
    pot_up_voltage.setPosition(99, true);

    pot_up_current.begin(25, 26, 27);  // pulse, direction, select 
    pot_up_current.setPosition(0, true);

    // pot_dw_voltage.begin(26, 25, 32);  // pulse, direction, select 
    // pot_dw_voltage.setPosition(0);

    pot_dw_current.begin(25, 26, 33);  // pulse, direction, select 
    pot_dw_current.setPosition(0, true);
}
    float down_cur_val = 0.0f; //ТОК ПОНИЖАЮЩИЙ DCDC

    float up_cur_val = 0.0f; //ТОК ПОВЫШАЮЩИЙ DCDC

    const int bufferSize = 32;
    char buffer[bufferSize];
    int bufferIndex = 0;
    bool isFirstValue = true;
	
    float firstValue = 0.0f;
    float secondValue = 22.0f;

void loop() 
{
    if (Serial.available() > 0)
    {
      up_cur_val = Serial.parseFloat();
    }


    float current[3];
    float current_compensated[3];
    float voltage[3];

    current[0]             = ina3221.getCurrent(INA3221_CH1);
    current_compensated[0] = ina3221.getCurrentCompensated(INA3221_CH1);
    voltage[0]             = ina3221.getVoltage(INA3221_CH1);

    current[1]             = ina3221.getCurrent(INA3221_CH2);
    current_compensated[1] = ina3221.getCurrentCompensated(INA3221_CH2);
    voltage[1]             = ina3221.getVoltage(INA3221_CH2);

    current[2]             = ina3221.getCurrent(INA3221_CH3);
    current_compensated[2] = ina3221.getCurrentCompensated(INA3221_CH3);
    voltage[2]             = ina3221.getVoltage(INA3221_CH3);

    Serial.print("Channel 1: \n Current: ");
    Serial.print(current[0], PRINT_DEC_POINTS);
    Serial.print("A\n Compensated current: ");
    Serial.print(current_compensated[0], PRINT_DEC_POINTS);
    Serial.print("\n Voltage: ");
    Serial.print(voltage[0], PRINT_DEC_POINTS);
    Serial.println("V");

    Serial.print("Channel 2: \n Current: ");
    Serial.print(current[1], PRINT_DEC_POINTS);
    Serial.print("A\n Compensated current: ");
    Serial.print(current_compensated[1], PRINT_DEC_POINTS);
    Serial.print("\n Voltage: ");
    Serial.print(voltage[1], PRINT_DEC_POINTS);
    Serial.println("V");

    Serial.print("Channel 3: \n Current: ");
    Serial.print(current[2], PRINT_DEC_POINTS);
    Serial.print("A\n Compensated current: ");
    Serial.print(current_compensated[2], PRINT_DEC_POINTS);
    Serial.print("\n Voltage: ");
    Serial.print(voltage[2], PRINT_DEC_POINTS);
    Serial.println("V\n");
    delay(100);

    //Изменение тока входящего dcdc
    if(current[2] > down_cur_val)
    {
      pot_dw_current.incr();
      delay(100);
    }
    else
    {
      pot_dw_current.decr();
      delay(100);
    }

    //Изменение тока исходящего dcdc
    if(current[1] > up_cur_val)
    {
      pot_up_current.decr();
      delay(100);
    }
    else
    {
      pot_up_current.incr();
      delay(100);
    }

    //Изменение напряжения исходящего dcdc
    if(voltage[1] < secondValue)
    {
      pot_up_voltage.decr();
      delay(100);
    }
    else
    {
      pot_up_voltage.incr();
      delay(100);
    }


    // Serial.print("POS:\t0\t");
    // Serial.println(pot_dw_current.getPosition());
    // Serial.print("VAL:\t\t");
    // Serial.println(pot_dw_current.getOhm());

    // Serial.print("POS CUR:\t0\t");
    // Serial.println(pot_dw_current.getPosition());
    // Serial.print("VAL:\t\t");
    // Serial.println(pot_dw_current.getOhm());

    Serial.print("POS VOL:\t0\t");
    Serial.println(pot_up_voltage.getPosition());

    Serial.print("POS CUR:\t0\t");
    Serial.println(pot_up_current.getPosition());

    Serial.print("SET CUR:\t0\t");
    Serial.println(up_cur_val);

    Serial.print("SET VOL:\t0\t");
    Serial.println(secondValue);

    // Serial.print("VAL:\t\t");
    // Serial.println(pot_up_voltage.getOhm());

    // for (uint8_t i = 0; i < 100; i++)
    // {
    //   pot.incr();
    //   delay(100);
    // }
    // Serial.println(millis());

    // for (uint8_t i = 0; i < 100; i++)
    // {
    //   pot.decr();
    //   delay(100);
    // }
    // Serial.println(millis());

    
}