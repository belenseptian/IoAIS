void initOLED()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.drawBitmap(30, 0, henafra, 80, 60, WHITE);
  display.display();
  delay(5000);
}

void displayText(int text_size, int cursor_x, int cursor_y, String text, bool clear_)
{
  if(clear_ == true) display.clearDisplay(); 
  display.setTextSize(text_size);
  display.setTextColor(WHITE);
  display.setCursor(cursor_x, cursor_y);
  // Display static text
  display.println(text);
  display.display(); 
}