#ifndef MANDREL_H
#define MANDREL_H

#include "Config.h"
#include "Motor.h"

class Mandrel : public Motor
{
public:
  Mandrel() : Motor(config::mandrel_step_pin, 52) {} // No mandrel direction control => direction pin = 52

  void set_velocity(const double &tan_velocity)
  {
    set_usec_per_step(scaled_inch_per_step_ / tan_velocity);
  }

  void inc_step_count() { ++step_count_; }
  void clear_step_count() { step_count_ = 0; }
  int_type get_step_count() const { return step_count_; }
  void sync_step_count_with_backup() { step_count_ = backup_step_count_; }

  void inc_backup_step_count() { ++backup_step_count_; }
  void clear_backup_step_count() { backup_step_count_ = 0; }

  void set_step_count_at_far_dir_flip() { step_count_at_far_dir_flip_ = step_count_; }
  int_type get_step_count_at_far_dir_flip() { return step_count_at_far_dir_flip_; }

  void inc_step_count_at_start_of_pass()
  {
    step_count_at_start_of_pass_ += config::pass_offset_steps;
    if (step_count_at_start_of_pass_ >= config::mandrel_steps_per_rev)
    {
      // step_count_at_start_of_pass_ exists in range [0, 849]
      step_count_at_start_of_pass_ -= config::mandrel_steps_per_rev;
    }
  }

  void set_step_count_at_start_of_pass_even() { step_count_at_start_of_pass_ = config::mandrel_steps_per_rev / 2; }
  void set_step_count_at_start_of_pass_odd()
  {
    step_count_at_start_of_pass_ = (config::mandrel_steps_per_rev / 2) + (config::pass_offset_steps / 2);
  }
  int_type get_step_count_at_start_of_pass() const { return step_count_at_start_of_pass_; }

  void add_to_far_end_wait_steps(const int_type &additional_steps) { far_end_wait_steps_ += additional_steps; }
  int_type get_far_end_wait_steps() const { return far_end_wait_steps_; }

private:
  int_type step_count_ = 0;
  int_type backup_step_count_ = 0;
  int_type step_count_at_far_dir_flip_ = 0;

  int_type step_count_at_start_of_pass_ = config::mandrel_steps_per_rev / 2; // DO NOT TOUCH - configuration algorithm assumes initial value
  int_type far_end_wait_steps_ = 3 * config::deg_wrap_angle;                 // Mandrel speed increases with wrap angle => increase wait steps

  constexpr static long_int_type scaled_inch_per_step_ = (1000000 * TWO_PI * config::mandrel_radius) /
                                                         config::mandrel_steps_per_rev;
};

#endif