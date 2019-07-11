#include "/Users/i/Documents/Arduino/fw_controls/src/include/Mandrel.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Switch.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* This is a simple sanity test to ensure that the step count of the mandrel is properly found to be 850 +- 1 step.
 * 
 * The debounce time of the switch is known to directly effect the ability of the software to accurately record the
 * number of steps taken by the mandrel. Reasoning being that a long debounce time will cause the reset of the mandrel
 * counter to be delay within observable times, (due to the time taken to debounce the switch, the system is cant be sure
 * whether it really detected a rising edge for ~20'000 us). The current mandrel switch is of good physical qualtiy and 
 * only requires a 1000 us delay because it does not bounce very much. This software debounce can be circumvented if the
 * switch is physically debounced with a capacitor / diode circuit which should be considered in the future, but is currently
 * seen as uneccessary and time consuming.
 * 
 * To execute the test simply run the code and open a serial monitor and ensure that the number of mandrel steps being outputed
 * is correct within 1 step. A tolerance of steps, say +- 2 or greater indicates a problem in the system. */

int test_mandrel_steps_to_rotate_()
{
  Serial.begin(115200);
  Mandrel Mandrel;
  Switch M_Encoder_Switch(config::m_encoder_switch_pin);

  /* START TEST PARAMETERS */
  // Using fcn of 200/deg_wrap_angle to define carriage vel, 'tan_vel' is between 3.5 -> 14 in/s for 15 <= deg_wrap_angle <= 80
  double tan_vel = 8;
  // Test 'revolutions' between 1 and 10 revs
  unsigned long int revolutions = 1;
  /* END TEST PARAMETERS */

  Mandrel.set_velocity(tan_vel);

  // The two below variables are helpers in testing s.t. revolutions can be specified by velocity will be tested
  double ang_vel = tan_vel;
  ang_vel /= TWO_PI;
  ang_vel /= config::mandrel_radius;

  // Initialize test time to 1s = 1,000,000us
  // Test time = 1s * (X rev) / (Y rev/s) = s
  unsigned long int test_time = 1000000;
  test_time *= revolutions;
  test_time /= ang_vel;

  // Set test_time to time required to do X revolutions
  unsigned long int start_time = micros();
  unsigned long int end_time = test_time + start_time;

  Mandrel.clear_step_count();
  while (micros() < end_time)
  {
    unsigned long int curr_usec = micros();
    if ((curr_usec - Mandrel.get_last_step_time()) > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
    }

    if (M_Encoder_Switch.is_rising_edge())
    {
      Serial.print(Mandrel.get_step_count());
      Serial.print("\n");

      Mandrel.clear_step_count();
    }
  }

  return 0;
}

void setup()
{
  test_mandrel_steps_to_rotate_();
}

void loop() {}
