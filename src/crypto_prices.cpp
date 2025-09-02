#include "crypto_prices.h"
// no HTTPClient/ArduinoJson here anymore

// globalUIDispatch is set by CardController; we use it to run LVGL changes on the UI task
extern std::function<void(std::function<void()>, bool)> globalUIDispatch;

static String trim(const String& s){ String t=s; t.trim(); return t; }

CryptoPricesApp::CryptoPricesApp(const String& csv, EventQueue& eq)
: eventQueue(eq) {
  setConfig(csv);

  // Subscribe to results; do NOT touch LVGL directly in case this runs off-UI.
  eventQueue.subscribe([this](const Event& e){
    if (e.type != EventType::CRYPTO_FETCH_RESULT) return;

    // Marshal LVGL updates onto the UI task
    if (globalUIDispatch) {
      // copy rows to avoid dangling temp
      auto rowsCopy = e.rows;
      globalUIDispatch([this, rowsCopy]() {
        renderRows(rowsCopy);
        // mark not in flight after UI finished rendering (safe)
        const_cast<CryptoPricesApp*>(this)->fetchInFlight = false;
      }, true);
    } else {
      // Fallback (shouldn't happen once UI queue is set)
      renderRows(e.rows);
      fetchInFlight = false;
    }
  });
}

CryptoPricesApp::~CryptoPricesApp(){ cleanup(); }

void CryptoPricesApp::setConfig(const String& csv){
  ids.clear();
  int start=0;
  while (true){
    int comma = csv.indexOf(',', start);
    String tok = (comma==-1) ? csv.substring(start) : csv.substring(start, comma);
    tok = trim(tok);
    if (tok.length()) ids.push_back(tok);
    if (comma==-1) break;
    start = comma+1;
  }
}

void CryptoPricesApp::setup(lv_obj_t* parent){
  if (main_container) return;
  main_container = lv_obj_create(parent);
  lv_obj_remove_style_all(main_container);
  lv_obj_set_size(main_container, LV_HOR_RES, LV_VER_RES);
  lv_obj_clear_flag(main_container, LV_OBJ_FLAG_SCROLLABLE);

  title = lv_label_create(main_container);
  lv_label_set_text(title, "Crypto Prices");
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 6, 6);

  list = lv_obj_create(main_container);
  lv_obj_set_size(list, LV_HOR_RES - 12, LV_VER_RES - 28);
  lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -4);
  lv_obj_set_style_pad_all(list, 6, 0);
  lv_obj_set_style_border_width(list, 0, 0);
  lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
  lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

  buildList();

  // First fetch as soon as the card starts looping
  nextFetchAt = millis();
  fetchInFlight = false;
}

void CryptoPricesApp::buildList(){
  rows.clear();
  lv_obj_clean(list);
  for (auto& id : ids){
    lv_obj_t* row = lv_label_create(list);
    String sym = id; sym.toUpperCase();
    lv_label_set_text(row, (sym + "  €…  $…").c_str());
    rows.push_back(row);
  }
}

// Called by CardNavigationStack while the card is visible
void CryptoPricesApp::loop(){
  const uint32_t now = millis();
  if (!fetchInFlight && now >= nextFetchAt) {
    fetchInFlight = true;
    nextFetchAt = now + 60000; // 60s cadence

    // Publish request; background subscriber (in CardController) will fetch.
    Event req;
    req.type = EventType::CRYPTO_FETCH_REQUEST;
    // Build CSV from ids
    String csv;
    for (size_t i=0;i<ids.size();++i){ if(i) csv+=','; csv+=ids[i]; }
    req.csvIds = csv;
    eventQueue.publishEvent(req);
  }
}

void CryptoPricesApp::renderRows(const std::vector<CryptoRow>& data){
  // Update labels with already-formatted strings from background
  const size_t n = min(rows.size(), data.size());
  for (size_t i=0;i<n; ++i){
    const auto& r = data[i];
    String line = r.symbol + "  €" + r.eur + "  $" + r.usd;
    lv_label_set_text(rows[i], line.c_str());
  }
}

void CryptoPricesApp::cleanup(){
  if (main_container){
    lv_obj_del(main_container);
    main_container = nullptr;
    title = nullptr;
    list = nullptr;
    rows.clear();
  }
}

lv_obj_t* CryptoPricesApp::get_main_container(){ return main_container; }
