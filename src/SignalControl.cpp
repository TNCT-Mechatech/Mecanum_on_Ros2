#include "SignalControl.hpp"

CTYPE pwm_controls[4] = 
{
    {ControlType::PCA9685, 0, 0, 1000},
    {ControlType::PCA9685, 0, 1, 1000},
    {ControlType::PCA9685, 0, 2, 1000},
    {ControlType::PCA9685, 0, 3, 1000}
};

CTYPE dir_controls[4] = 
{
    {ControlType::RASPIGPIO, -1, 21, 500},
    {ControlType::RASPIGPIO, -1, 20, 500},
    {ControlType::RASPIGPIO, -1, 16, 500},
    {ControlType::RASPIGPIO, -1, 12, 500}
};
