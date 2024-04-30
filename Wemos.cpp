#include <pigpio.h>
#include "httplib.h"

#define LED_PIN 17 // GPIO pin waarop de LED is aangesloten

void setup() {
  if (gpioInitialise() < 0) {
    fprintf(stderr, "pigpio initialisation failed.\n");
    return;
  }
  gpioSetMode(LED_PIN, PI_OUTPUT);
  gpioWrite(LED_PIN, 0); // Zet de LED uit
}

int main(void) {
  setup();
  httplib::Server svr;

  svr.Get("/toggleLED", [](const httplib::Request&, httplib::Response& res) {
    static bool ledState = false;
    gpioWrite(LED_PIN, ledState ? 0 : 1); // Wissel de LED-status
    ledState = !ledState;
    res.set_content("LED Toggled", "text/plain");
  });

  svr.listen("0.0.0.0", 80);
  return 0;
}
