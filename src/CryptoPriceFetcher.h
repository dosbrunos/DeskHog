#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>

/**
 * @class CryptoPriceFetcher
 * @brief Fetches cryptocurrency prices from the CoinGecko API.
 */
class CryptoPriceFetcher {
public:
    /**
     * @brief Constructor for CryptoPriceFetcher.
     */
    CryptoPriceFetcher();

    /**
     * @brief Fetches cryptocurrency prices.
     * 
     * @param result A reference to a String where the fetched prices will be stored.
     * @return True if the fetch was successful, false otherwise.
     */
    bool fetchPrices(String& result);

private:
    WiFiClientSecure client; ///< Secure client for HTTPS requests.
};