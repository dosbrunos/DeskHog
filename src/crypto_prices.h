#pragma once
#include <lvgl.h>
#include <Arduino.h>
#include <vector>

class CryptoPricesApp {
public:
  CryptoPricesApp(const String& configCSV);       // parse ids here
  ~CryptoPricesApp();

  void setup(lv_obj_t* parent_screen);            // build LVGL UI here
  void loop();                                    // optional (if you need periodic work)
  void cleanup();                                  // delete LVGL objects
  lv_obj_t* get_main_container();

  void setConfig(const String& csv);               // allow updates if needed

private:
  lv_obj_t* main_container = nullptr;
  lv_obj_t* title = nullptr;
  lv_obj_t* list = nullptr;
  lv_timer_t* timer = nullptr;

  std::vector<String> ids;                         // coin ids (e.g. "bitcoin")
  std::vector<lv_obj_t*> rows;                    // per-coin label rows

  void buildList();
  static void timer_cb(lv_timer_t* t);            // calls fetchAndUpdate()
  void fetchAndUpdate();                          // HTTP GET + update labels
};
