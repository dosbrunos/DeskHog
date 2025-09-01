#pragma once
#include <lvgl.h>
#include <Arduino.h>
#include "crypto_prices.h"
#include "ui/InputHandler.h"   // for handleButtonPress/update signatures (match your project)

class CryptoPricesCard : public InputHandler {
public:
  CryptoPricesCard(lv_obj_t* parent, const String& configCSV);
  ~CryptoPricesCard();

  lv_obj_t* getCard();
  bool handleButtonPress(uint8_t button_index) override; // optional
  void prepareForRemoval();
  bool update() override;                                 // called by CardNavigationStack

private:
  CryptoPricesApp* app = nullptr;
  lv_obj_t* cardContainer = nullptr;
  bool markedForRemoval = false;
};
