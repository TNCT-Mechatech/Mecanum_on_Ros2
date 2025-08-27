#include "SignalControl.hpp"

PWMTYPE pwm_controls[4] = 
{
    {ControlType::PCA9685, 0, 0, 1000},
    {ControlType::PCA9685, 0, 1, 1000},
    {ControlType::PCA9685, 0, 2, 1000},
    {ControlType::PCA9685, 0, 3, 1000}
};

DIRTYPE dir_controls[4] = 
{
    {ControlType::RASPIGPIO, -1, 21, -1, false},
    {ControlType::RASPIGPIO, -1, 20, -1, true},
    {ControlType::RASPIGPIO, -1, 16, -1, true},
    {ControlType::RASPIGPIO, -1, 12, -1, true}
};
