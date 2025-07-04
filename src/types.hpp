#pragma once

enum class ControlType { RASPIGPIO, PCA9685 };

struct CTYPE {
  ControlType controlType; // 端末のマクロで初期化(ex. RASPI_GPIO)
  int PCA9685Id;           // PCAを使用しない場合は0未満の値で初期化
  int channel;             // GPIO, PCA9685のpin name
  float freq;              // pwmの周波数
};