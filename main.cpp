#include "mbed.h"

#include <cstdint>

#include <cstdio>

#define Blinking_RATE 250ms

#define max_value 2000

#define Vref 3.3

#define x_zero 1.73

#define y_zero 1.65

#define z_zero 0.72

#define voltage_sensitivty_x 0.3

#define voltage_sensitivty_y 0.3

#define voltage_sensitivty_z 0.3



const unsigned long baudRate = 9600;



float Length;

float distances = 0;

long int Count = 0;



float average = 0;

float x_average, y_average, z_average;



float x_total = 0;

float y_total = 0;

float z_total = 0;



float x_summary, y_summary, z_summary;

float x, y, z;

float value_y, value_x, value_z;

float x_1, y_1, z_1;

float low_average = 0;

int steps = 0;

bool signal = 0;

float high_average = 0;

int samples = 0;

float threshold = 14;

float calories;

AnalogIn x1pin(A0);

AnalogIn y1pin(A1);

AnalogIn z1pin(A2);

AnalogIn xpin(A4);

DigitalIn mypin(D8);

DigitalOut LED(D9);

DigitalOut LED2(D8);

// ...

Timer t;

int val;

int count = 0;



const unsigned int Delay = 400; // write interval (in ms)



// KY039 defines

#define TAB_LENGTH 4

#define RISE_THRESHOLD 5

#define Offset 0



Serial device(USBTX, USBRX); // tx, rx



// main() runs in its own thread in the OS

int main() {



  float analog_Tab[TAB_LENGTH], analog_sum;

  float last, analog_average, start;

  float Beat_Value, first, second, third, before;

  bool rising;

  int rise_count, n_reads;

  long int previous_beat, now, pointer;



  device.baud(9600);

  t.start();

  if (mypin.is_connected()) {

    printf("mypin is connected and initialized! \n\r");

  }

  for (int i = 0; i < 100; i++) {

    x = x1pin.read();

    y = y1pin.read();

    z = z1pin.read();



    value_y = y * 2000;

    value_x = x * 2000;

    value_z = z * 2000;

//comments

    x_total =

        x_total + (value_x / max_value * Vref - x_zero) / voltage_sensitivty_x;



    wait_ms(15);



    y_total =

        y_total + (value_y / max_value * Vref - y_zero) / voltage_sensitivty_y;



    wait_ms(15);



    z_total =

        z_total + (value_z / max_value * Vref - z_zero) / voltage_sensitivty_z;



    wait_ms(15);


  }



  x_summary = max_value * x_total;

  y_summary = max_value * y_total;

  z_summary = max_value * z_total;



  x_average = (x_summary / 100) * 0.9;

  y_average = (y_summary / 100) * 0.9;

  z_average = (z_summary / 100) * 0.9;



  while (true) {



    // WiFi connection



    // Init variables

    for (int i = 0; i < TAB_LENGTH; i++)

      analog_Tab[i] = 0;

    {

      analog_sum = 0;

      pointer = 0;

    }



    while (1) {



      if (mypin.read() == 1) {

        LED = 1;

      }



      else {

        LED = 0;



        //   printf("LED IS ON : %d \n\r", LED.read());

        //   printf("LDR : %d \n\r", mypin.read());

      }



      x = x1pin.read();

      y = y1pin.read();

      z = z1pin.read();



      value_y = y * max_value;

      value_x = x * max_value;

      value_z = z * max_value;



      x_1 = (value_x / max_value * Vref - x_zero) / voltage_sensitivty_x;

      y_1 = (value_y / max_value * Vref - y_zero) / voltage_sensitivty_y;

      z_1 = (value_z / max_value * Vref - z_zero) / voltage_sensitivty_z;



      average = sqrt((x_1) * (x_1) + (y_1) * (y_1) + (z_1) * (z_1)); //



      if (samples == 0) {

        high_average = average;



        low_average = average;

      }



      else if (high_average < average) {

        high_average = average;

      }



      else if (low_average > average) {

        low_average = average;

      }



      samples++;



      if (samples >= 50) {



        threshold = (high_average + low_average) / 2 + 2;



        samples = 0;

      }



      if (average > threshold && signal == 0) {



        steps++;



        calories = steps * 0.049;



        float distance = (Length * Count) / 100;

        distances = distance;



        signal = 1;



      } else if (average > threshold && signal == 1) {

        //   printf("%d", steps);

      } else if (average < threshold && signal == 1) {

        signal = 0;

        //   printf("%d", steps);

      }



      // the 50 Hz noise caused by electric light

      n_reads = 0;

      start = t.read_ms();

      analog_average = 0.;

      do {

        analog_average += xpin.read();

        n_reads++;

        now = t.read_ms();



      } while (now < start + 20);

      analog_average /= n_reads;



      analog_sum -= analog_Tab[pointer];

      analog_sum += analog_average;

      analog_Tab[pointer] = analog_average;

      last = analog_sum / TAB_LENGTH;



      if (last > before) {

        rise_count++;



        if (!rising && rise_count > RISE_THRESHOLD) {



          rising = true;

          first = t.read_ms() - previous_beat;

          previous_beat = t.read_ms();

          // Calculate heartbeat

          Beat_Value = 60000. / (0.4 * first + 0.3 * second + 0.3 * third) +

                       Offset;



          If(Beat_Value <= 40) { LED2 = 1; }

          printf("%f?%f?%ld?%f", Beat_Value * 1.35, calories, Count,

                 distances); // display the values on matlab



          //    printf("%f\n%f\n%f\n%ld\n%f\n",Beat_Value*1.35,calories,Count,distance); //display the values on serial port

          third = second;

          second = first;



          wait_ms(Delay); // delay

        }

      } else {

        //falling edge

        rising = false;

        rise_count = 0;

      }

      before = last;

      pointer++;

      pointer %= TAB_LENGTH;

      // printf("Check5555\n");

    }

  }

}
