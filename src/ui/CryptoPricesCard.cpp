#include "ui/CryptoPricesCard.h"

CryptoPricesCard::CryptoPricesCard(lv_obj_t* parent, const String& cfg, EventQueue& eq){
  app = new CryptoPricesApp(cfg, eq);
  app->setup(parent);
  cardContainer = app->get_main_container();
}

CryptoPricesCard::~CryptoPricesCard(){
  if (app && !markedForRemoval){
    app->cleanup();
    delete app;
    app = nullptr;
  }
  cardContainer = nullptr;
}

lv_obj_t* CryptoPricesCard::getCard(){ return cardContainer; }

bool CryptoPricesCard::handleButtonPress(uint8_t){
  return false;
}

void CryptoPricesCard::prepareForRemoval(){ markedForRemoval = true; }

bool CryptoPricesCard::update(){
  if (app){ app->loop(); return true; }
  return false;
}
