#include "CryptoPriceFetcher.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

CryptoPriceFetcher::CryptoPriceFetcher() : client() {
    client.setTimeout(5000); // Set a 5-second timeout for the client
}

bool CryptoPriceFetcher::fetchPrices(String& result) {
    const char* host = "api.coingecko.com";
    const char* url = "/api/v3/simple/price?ids=bitcoin,ethereum,dogecoin&vs_currencies=usd";

    if (!client.connect(host, 443)) {
        Serial.println("Connection to CoinGecko failed!");
        return false;
    }

    // Send HTTP GET request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    // Wait for response
    while (client.connected() && !client.available()) {
        delay(10);
    }

    // Read response
    String response;
    while (client.available()) {
        response += client.readString();
    }

    // Parse HTTP response
    int bodyIndex = response.indexOf("\r\n\r\n");
    if (bodyIndex == -1) {
        Serial.println("Invalid HTTP response");
        return false;
    }
    String body = response.substring(bodyIndex + 4);

    // Parse JSON
    DynamicJsonDocument doc(2048); // Increased buffer size
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        return false;
    }

    // Extract prices
    String prices = "BTC: $" + String(doc["bitcoin"]["usd"].as<float>(), 2) + "\n" +
                    "ETH: $" + String(doc["ethereum"]["usd"].as<float>(), 2) + "\n" +
                    "DOGE: $" + String(doc["dogecoin"]["usd"].as<float>(), 2);
    result = prices;

    return true;
}