#include "CryptoPriceCard.h"
#include "network/CryptoPriceFetcher.cpp"

CryptoPriceCard::CryptoPriceCard(lv_obj_t* parent) {
    _card = lv_obj_create(parent);
    lv_obj_set_size(_card, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(_card, lv_color_hex(0x000000), 0);

    _price_label = lv_label_create(_card);
    lv_label_set_text(_price_label, "Fetching prices...");
    lv_obj_align(_price_label, LV_ALIGN_CENTER, 0, 0);

    updatePrices();
}

CryptoPriceCard::~CryptoPriceCard() {
    lv_obj_del(_card);
}

void CryptoPriceCard::updatePrices() {
    CryptoPriceFetcher fetcher;
    String prices;
    if (fetcher.fetchPrices(prices)) {
        lv_label_set_text(_price_label, prices.c_str());
    } else {
        lv_label_set_text(_price_label, "Failed to fetch prices.");
    }
}