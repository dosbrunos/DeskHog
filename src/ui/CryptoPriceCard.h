#pragma once

#include <lvgl.h>
#include <Arduino.h>
#include "ui/InputHandler.h"

class CryptoPriceCard : public InputHandler {
public:
    CryptoPriceCard(lv_obj_t* parent);
    ~CryptoPriceCard();

    lv_obj_t* getCard() const { return _card; }
    void updatePrices();

private:
    lv_obj_t* _card;
    lv_obj_t* _price_label;
};