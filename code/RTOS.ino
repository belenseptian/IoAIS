void taskTransmit(void *pvParameters)
{
  while (1)
  {
    Serial1.println(rawAIS);
    if(gps.equals("!") || gps.equals("$"))
    {
      // sendtoServer(rawAIS);
      if(connected == 1)
      {
        sendtoWeb(encrypt(rawAIS));
      }
    }
    vTaskDelay(700 / portTICK_PERIOD_MS);
  }
}

void taskRead(void *pvParameters)
{
  while (1)
  {
    rawAIS = converttoChars(getSentence());
    gps = String(rawAIS[0]);
    free(rawAIS);
    delay(5);
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}

void initTask()
{
  xTaskCreatePinnedToCore(
    taskTransmit, // Function name of the task
    "Task 1",       // Name of the task (e.g. for debugging)
    10000,          // Stack size (bytes)
    NULL,           // Parameter to pass
    1,              // Task priority
    NULL,           // Task handle
    0               // run on Core 0
  );

  xTaskCreatePinnedToCore(
    taskRead,  // Function name of the task
    "Task 2", // Name of the task (e.g. for debugging)
    10000,    // Stack size (bytes)
    NULL,     // Parameter to pass
    1,        // Task priority
    NULL,     // Task handle
    1         // run on Core 1
  );
}
