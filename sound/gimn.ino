
uint16_t freq, dur;


#if defined(ARDUINO_ARCH_ESP32) || CONFIG_IDF_TARGET_ESP32C3
ledcSetup(2, 2000, 8);
ledcAttachPin(BUZ_PIN, 2);
#endif
/*
  #if !defined(ARDUINO_ARCH_ESP32)
    tone(BUZ_PIN, freq, dur);
    noTone(BUZ_PIN);
  #else
    ledcWriteTone(2, freq);
    ledcWriteTone(2, 0);
  #endif
*/
void gimn()
{
#if !defined(ARDUINO_ARCH_ESP32)
  tone(BUZ_PIN, 174, 250);
  delay(278);
  tone(BUZ_PIN, 233, 500);
  delay(556);
  tone(BUZ_PIN, 174, 375);
  delay(417);
  tone(BUZ_PIN, 195, 125);
  delay(139);
  tone(BUZ_PIN, 220, 500);
  delay(556);
  tone(BUZ_PIN, 146, 250);
  delay(278);
  tone(BUZ_PIN, 146, 250);
  delay(278);
  tone(BUZ_PIN, 195, 500);
  delay(556);
  tone(BUZ_PIN, 174, 375);
  delay(417);
  tone(BUZ_PIN, 155, 125);
  delay(139);
  tone(BUZ_PIN, 174, 500);
  delay(556);
  tone(BUZ_PIN, 116, 250);
  delay(278);
  tone(BUZ_PIN, 116, 250);
  delay(278);
  tone(BUZ_PIN, 130, 500);
  delay(556);
  tone(BUZ_PIN, 130, 375);
  delay(417);
  tone(BUZ_PIN, 146, 125);
  delay(139);
  tone(BUZ_PIN, 155, 500);
  delay(556);
  tone(BUZ_PIN, 155, 375);
  delay(417);
  tone(BUZ_PIN, 174, 125);
  delay(139);
  tone(BUZ_PIN, 195, 500);
  delay(556);
  tone(BUZ_PIN, 220, 375);
  delay(417);
  tone(BUZ_PIN, 233, 125);
  delay(139);
  tone(BUZ_PIN, 261, 750);
  delay(833.3325);
  tone(BUZ_PIN, 174, 250);
  delay(278);
  tone(BUZ_PIN, 293, 500);
  delay(556);
  tone(BUZ_PIN, 261, 375);
  delay(417);
  tone(BUZ_PIN, 233, 125);
  delay(139);
  tone(BUZ_PIN, 261, 500);
  delay(556);
  tone(BUZ_PIN, 174, 250);
  delay(278);
  tone(BUZ_PIN, 174, 250);
  delay(278);
  tone(BUZ_PIN, 233, 500);
  delay(556);
  tone(BUZ_PIN, 220, 375);
  delay(417);
  tone(BUZ_PIN, 195, 125);
  delay(139);
  tone(BUZ_PIN, 220, 500);
  delay(556);
  tone(BUZ_PIN, 146, 375);
  delay(417);
  tone(BUZ_PIN, 146, 125);
  delay(139);
  tone(BUZ_PIN, 195, 500);
  delay(556);
  tone(BUZ_PIN, 174, 375);
  delay(417);
  tone(BUZ_PIN, 155, 125);
  delay(139);
  tone(BUZ_PIN, 174, 500);
  delay(556);
  tone(BUZ_PIN, 116, 375);
  delay(417);
  tone(BUZ_PIN, 116, 125);
  delay(139);
  tone(BUZ_PIN, 233, 500);
  delay(556);
  tone(BUZ_PIN, 220, 375);
  delay(417);
  tone(BUZ_PIN, 195, 125);
  delay(139);
  tone(BUZ_PIN, 174, 1000);
  delay(1111);
  tone(BUZ_PIN, 293, 1000);
  delay(1111);
  tone(BUZ_PIN, 261, 250);
  delay(278);
  tone(BUZ_PIN, 233, 250);
  delay(278);
  tone(BUZ_PIN, 220, 250);
  delay(278);
  tone(BUZ_PIN, 233, 250);
  delay(278);
  tone(BUZ_PIN, 261, 750);
  delay(833);
  tone(BUZ_PIN, 174, 250);
  delay(278);
  tone(BUZ_PIN, 174, 1000);
  delay(1111);
  tone(BUZ_PIN, 233, 1000);
  delay(1111);
  tone(BUZ_PIN, 220, 250);
  delay(278);
  tone(BUZ_PIN, 195, 250);
  delay(278);
  tone(BUZ_PIN, 174, 250);
  delay(278);
  tone(BUZ_PIN, 195, 250);
  delay(278);
  tone(BUZ_PIN, 220, 750);
  delay(833.3325);
  tone(BUZ_PIN, 146, 250);
  delay(278);
  tone(BUZ_PIN, 146, 1000);
  delay(1111);
  tone(BUZ_PIN, 233, 500);
  delay(556);
  tone(BUZ_PIN, 195, 375);
  delay(417);
  tone(BUZ_PIN, 220, 125);
  delay(139);
  tone(BUZ_PIN, 233, 500);
  delay(556);
  tone(BUZ_PIN, 195, 375);
  delay(417);
  tone(BUZ_PIN, 220, 125);
  delay(139);
  tone(BUZ_PIN, 233, 500);
  delay(556);
  tone(BUZ_PIN, 195, 375);
  delay(417);
  tone(BUZ_PIN, 233, 125);
  delay(139);
  tone(BUZ_PIN, 311, 1000);
  delay(1111);
  tone(BUZ_PIN, 311, 1000);
  delay(1111);
  tone(BUZ_PIN, 293, 250);
  delay(278);
  tone(BUZ_PIN, 261, 250);
  delay(278);
  tone(BUZ_PIN, 233, 250);
  delay(278);
  tone(BUZ_PIN, 261, 250);
  delay(278);
  tone(BUZ_PIN, 293, 750);
  delay(833.3325);
  tone(BUZ_PIN, 233, 250);
  delay(278);
  tone(BUZ_PIN, 233, 1000);
  delay(1111);
  tone(BUZ_PIN, 261, 1000);
  delay(1111);
  tone(BUZ_PIN, 233, 250);
  delay(278);
  tone(BUZ_PIN, 220, 250);
  delay(278);
  tone(BUZ_PIN, 195, 250);
  delay(278);
  tone(BUZ_PIN, 220, 250);
  delay(278);
  tone(BUZ_PIN, 233, 750);
  delay(833.3325);
  tone(BUZ_PIN, 195, 250);
  delay(278);
  tone(BUZ_PIN, 195, 1000);
  delay(1111);
  tone(BUZ_PIN, 233, 500);
  delay(556);
  tone(BUZ_PIN, 220, 375);
  delay(417);
  tone(BUZ_PIN, 195, 125);
  delay(139);
  tone(BUZ_PIN, 174, 500);
  delay(556);
  tone(BUZ_PIN, 116, 375);
  delay(417);
  tone(BUZ_PIN, 116, 125);
  delay(139);
  tone(BUZ_PIN, 174, 1000);
  delay(1111);
  tone(BUZ_PIN, 195, 500);
  delay(556);
  tone(BUZ_PIN, 220, 500);
  delay(556);
  tone(BUZ_PIN, 233, 2000);
  delay(2222.22);
#endif
}
