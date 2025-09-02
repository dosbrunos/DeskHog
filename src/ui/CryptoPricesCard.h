#pragma once
#include <lvgl.h>
#include "crypto_prices.h"
#include "events/EventQueue.h"

class CryptoPricesCard : public InputHandler {
public:
  CryptoPricesCard(lv_obj_t* parent, const String& cfg, EventQueue& eq);
  ~CryptoPricesCard();

  lv_obj_t* getCard();
  bool handleButtonPress(uint8_t) override;
  void prepareForRemoval();
  bool update() override;

private:
  CryptoPricesApp* app = nullptr;
  lv_obj_t* cardContainer = nullptr;
  bool markedForRemoval = false;
};
