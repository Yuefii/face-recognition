#include "esp_camera.h"
#include <WiFi.h>

// Ganti dengan SSID dan Password jaringan WiFi Anda
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Konfigurasi kamera ESP32-CAM
camera_config_t config;
void setup() {
  Serial.begin(115200);
  
  // Setup kamera ESP32-CAM
  config.ledc_channel = LEDC_CHANNEL;
  config.ledc_timer = LEDC_TIMER;
  config.pin_d0 = 16;
  config.pin_d1 = 17;
  config.pin_d2 = 18;
  config.pin_d3 = 19;
  config.pin_d4 = 21;
  config.pin_d5 = 22;
  config.pin_d6 = 23;
  config.pin_d7 = 25;
  config.pin_xclk = 0;
  config.pin_pclk = 4;
  config.pin_vsync = 5;
  config.pin_href = 13;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 12;
  config.fb_count = 2;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  // Hubungkan ke WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Tampilkan IP setelah terhubung
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Mulai server HTTP untuk streaming
  WiFiServer server(80);
  server.begin();
  
  while (true) {
    WiFiClient client = server.available();
    if (client) {
      String req = client.readStringUntil('\r');
      if (req.indexOf("GET /stream") != -1) {
        camera_fb_t *fb = NULL;
        fb = esp_camera_fb_get();
        if (!fb) {
          Serial.println("Camera capture failed");
          return;
        }
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
        client.println();

        while (client.connected()) {
          fb = esp_camera_fb_get();
          if (!fb) {
            Serial.println("Camera capture failed");
            break;
          }
          client.println("--frame");
          client.println("Content-Type: image/jpeg");
          client.println("Content-Length: " + String(fb->len));
          client.println();
          client.write(fb->buf, fb->len);
          esp_camera_fb_return(fb);
        }
      }
    }
  }
}

void loop() {
  // Tidak ada kode di sini karena server berjalan di setup()
}
