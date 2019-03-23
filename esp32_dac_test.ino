#include <Arduino.h>
#include "i2sdac.h"

I2sDAC dac;

TaskHandle_t AudioTask;

// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
void setup()
{
	Serial.begin(115200);

	// High priority audio thread
	xTaskCreatePinnedToCore(
	    AudioThread,
	    "audio",
	    8192,
	    NULL,
	    3,
	    &AudioTask,
	    0);

	// Try to start the DAC
	if (dac.begin())
		Serial.println("DAC init succes");
}


// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
void AudioThread(void *pvParameter)
{
	unsigned int frameCount = 0;
	int16_t sample[2];

	Serial.print("Audio thread started at core: ");
	Serial.println(xPortGetCoreID());

	while(1)
	{
		// Random sample for both the left and right channel
		// Generates white noise
		for (unsigned s = 0; s < 2; ++s)
			sample[s] = random(-32768, 32767);

		float sampleFloat = static_cast<float>(sample[0]) / 32767.0f;

//		dac.writeSample(sample[0], sample[1]);

		dac.writeSample(sampleFloat, sampleFloat);

		// Pause thread after delivering 64 samples so that other threads can do stuff
		if (frameCount++ % 64 == 0)
			vTaskDelay(1);
	}

}


// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
void loop()
{
	// Do stuff that has less priority than audio
	vTaskDelay(1000 / portTICK_RATE_MS);
}

