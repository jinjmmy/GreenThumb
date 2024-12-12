#include <Arduino.h>
#include <inttypes.h>
#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

void setup() {
    // Start the serial monitor for debugging
    Serial.begin(9600);
    delay(5000);
    Serial.println("Setup started");
    delay(1000);
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // If NVS partition is truncated or needs to be erased, retry after erasing
        Serial.println("check");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open NVS handle
    Serial.println("\nOpening Non-Volatile Storage (NVS) handle...");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        Serial.println("Done");

        // Write Wi-Fi credentials to NVS
        Serial.println("Updating SSID and password in NVS...");
        char ssid[] = "K9T6";      // Replace with your Wi-Fi SSID
        char pass[] = "19491001"; // Replace with your Wi-Fi password
        err = nvs_set_str(my_handle, "ssid", ssid);
        err |= nvs_set_str(my_handle, "pass", pass);
        Serial.printf((err != ESP_OK) ? "Failed to update SSID and password!\n" : "Done\n");

        // Commit the changes to NVS
        Serial.println("Committing updates to NVS...");
        err = nvs_commit(my_handle);
        Serial.printf((err != ESP_OK) ? "Failed to commit updates!\n" : "Done\n");

        // Close the NVS handle
        nvs_close(my_handle);
    }
}

void loop() {
    // Main loop code (if needed)
    delay(1000);
}
