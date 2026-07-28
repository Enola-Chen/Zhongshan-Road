// Firebase upload module for ESP32-Paxcounter
// Runs in a dedicated FreeRTOS task (8KB stack) to avoid
// stack overflow in irqHandlerTask when using HTTPClient.

#include "firebase_upload.h"
#include "globals.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Firebase Realtime Database base URL
#define FIREBASE_DB_URL "https://citywalk-1369c-default-rtdb.firebaseio.com"

// Fixed GPS coordinates of this device's location
#define FIREBASE_LAT 24.137774
#define FIREBASE_LNG 120.686979

typedef struct {
  int wifi_count;
  int ble_count;
  int pax_count;
} firebase_data_t;

static QueueHandle_t firebaseQueue = NULL;
static TaskHandle_t  firebaseTaskHandle = NULL;

static void firebase_task(void *pvParameters) {
  firebase_data_t data;

  for (;;) {
    if (xQueueReceive(firebaseQueue, &data, portMAX_DELAY) != pdTRUE)
      continue;

    // Reconnect if WiFi dropped
    if (WiFi.status() != WL_CONNECTED) {
      ESP_LOGW(TAG, "Firebase: WiFi lost, reconnecting...");
      WiFi.reconnect();
      for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++)
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    if (WiFi.status() != WL_CONNECTED) {
      ESP_LOGE(TAG, "Firebase: WiFi reconnect failed, skipping upload");
      continue;
    }

    // PUT to /devices/<clientId>.json — always overwrites with latest data
    char url[160];
    snprintf(url, sizeof(url), "%s/devices/%s.json", FIREBASE_DB_URL, clientId);

    char json[256];
    snprintf(json, sizeof(json),
      "{\"lat\":%.6f,\"lng\":%.6f,\"wifi\":%d,\"ble\":%d,\"pax\":%d,\"ts\":%lu}",
      FIREBASE_LAT, FIREBASE_LNG,
      data.wifi_count, data.ble_count, data.pax_count,
      (unsigned long)(millis() / 1000));

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT(json);
    if (httpCode > 0) {
      ESP_LOGI(TAG, "Firebase: OK (HTTP %d) pax=%d wifi=%d ble=%d",
               httpCode, data.pax_count, data.wifi_count, data.ble_count);
    } else {
      ESP_LOGE(TAG, "Firebase: upload failed: %s",
               http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

void firebase_init(void) {
  firebaseQueue = xQueueCreate(3, sizeof(firebase_data_t));

  ESP_LOGI(TAG, "Firebase: connecting to WiFi SSID=%s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++)
    vTaskDelay(pdMS_TO_TICKS(500));

  if (WiFi.status() == WL_CONNECTED)
    ESP_LOGI(TAG, "Firebase: WiFi connected, IP=%s",
             WiFi.localIP().toString().c_str());
  else
    ESP_LOGW(TAG, "Firebase: WiFi connect failed (will retry on first upload)");

  // Dedicated upload task on core 0, 8 KB stack
  xTaskCreatePinnedToCore(firebase_task, "firebase", 8192,
                          NULL, 1, &firebaseTaskHandle, 0);
}

void firebase_upload(int wifi_count, int ble_count, int pax_count) {
  if (firebaseQueue == NULL) return;

  firebase_data_t data = { wifi_count, ble_count, pax_count };
  if (xQueueSend(firebaseQueue, &data, 0) != pdTRUE)
    ESP_LOGW(TAG, "Firebase: queue full, dropping upload");
}
