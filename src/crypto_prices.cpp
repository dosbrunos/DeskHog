#include "crypto_prices.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

static String trim(const String& s){ String t=s; t.trim(); return t; }

CryptoPricesApp::CryptoPricesApp(const String& csv){ setConfig(csv); }
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

  // 60s refresh
  timer = lv_timer_create([](lv_timer_t* t){ ((CryptoPricesApp*)t->user_data)->fetchAndUpdate(); }, 60000, this);
  fetchAndUpdate(); // immediate first load
}

void CryptoPricesApp::buildList(){
  rows.clear();
  lv_obj_clean(list);
  for (auto& id : ids){
    lv_obj_t* row = lv_label_create(list);
    lv_label_set_text(row, (id + ": …").c_str());
    rows.push_back(row);
  }
}

void CryptoPricesApp::fetchAndUpdate(){
  if (ids.empty()) return;

  // Build CSV
  String idsCSV;
  for (size_t i=0;i<ids.size();++i){ if(i) idsCSV+=','; idsCSV+=ids[i]; }

  String url = "https://api.coingecko.com/api/v3/simple/price?ids=" + idsCSV + "&vs_currencies=eur,usd";

  HTTPClient http;
  http.setTimeout(7000);
  if (!http.begin(url)) return;
  int code = http.GET();
  if (code != 200){ http.end(); return; }

  DynamicJsonDocument doc(8192);
  auto err = deserializeJson(doc, http.getStream());
  http.end();
  if (err) return;

  // Update rows: display "SYM  €…  $…"
  for (size_t i=0;i<ids.size() && i<rows.size();++i){
    String id = ids[i];
    String sym = id; // you can map to BTC/ETH etc if you want
    sym.toUpperCase();

    String eur = doc[id]["eur"].isNull() ? "-" : String((double)doc[id]["eur"], (double)doc[id]["eur"]>=1.0?2:6);
    String usd = doc[id]["usd"].isNull() ? "-" : String((double)doc[id]["usd"], (double)doc[id]["usd"]>=1.0?2:6);
    String line = sym + "  €" + eur + "  $" + usd;
    lv_label_set_text(rows[i], line.c_str());
  }
}

void CryptoPricesApp::loop(){ /* not needed with timer, keep empty */ }

void CryptoPricesApp::cleanup(){
  if (timer){ lv_timer_del(timer); timer=nullptr; }
  if (main_container){ lv_obj_del(main_container); main_container=nullptr; title=nullptr; list=nullptr; rows.clear(); }
}

lv_obj_t* CryptoPricesApp::get_main_container(){ return main_container; }
