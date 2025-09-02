#pragma once
#include <lvgl.h>
#include <Arduino.h>
#include <vector>
#include "events/EventQueue.h"     // ← add this
// also ensure globalUIDispatch is declared somewhere included (it is in CardController.cpp)

class CryptoPricesApp {
public:
  // accept EventQueue so we can publish/subscribe
  CryptoPricesApp(const String& configCSV, EventQueue& eq);
  ~CryptoPricesApp();

  void setup(lv_obj_t* parent_screen);
  void loop();                     // will handle refresh cadence
  void cleanup();
  lv_obj_t* get_main_container();

  void setConfig(const String& csv);

private:
  EventQueue& eventQueue;          // ← store reference
  lv_obj_t* main_container = nullptr;
  lv_obj_t* title = nullptr;
  lv_obj_t* list = nullptr;

  std::vector<String> ids;
  std::vector<lv_obj_t*> rows;

  // refresh control (no LVGL timer)
  uint32_t nextFetchAt = 0;
  bool     fetchInFlight = false;

  void buildList();
  void renderRows(const std::vector<CryptoRow>& data);  // from Event payload
};
