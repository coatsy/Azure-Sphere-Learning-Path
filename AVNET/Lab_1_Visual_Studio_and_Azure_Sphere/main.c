﻿#include "../shared/azure_iot.h"
#include "../shared/globals.h"
#include "../shared/peripheral.h"
#include "../shared/terminate.h"
#include "../shared/timer.h"
#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "../shared/avnet_sk/avnet_sk.h"


#define JSON_MESSAGE_BYTES 128  // Number of bytes to allocate for the JSON telemetry message for IoT Central
static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

// Forward signatures
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);


static Peripheral builtinLed = {
	.fd = -1, .pin = BUILTIN_LED, .initialState = GPIO_Value_High, .invertPin = true, .initialise = OpenPeripheral, .name = "SendStatus"
};

static Timer measureSensorTimer = {
	.period = { 5, 0 },
	.name = "MeasureSensor",
	.timerEventHandler = &MeasureSensorHandler
};

#pragma region define sets for auto initialization and close

Peripheral* peripherals[] = { &builtinLed };
Timer* timers[] = { &measureSensorTimer };

#pragma endregion
// end define sets for auto initialization and close


int main(int argc, char* argv[])
{
	Log_Debug("Application starting.\n");
	RegisterTerminationHandler();

	if (InitPeripheralsAndHandlers() != 0) {
		Terminate();
	}

	// Main loop
	while (!IsTerminationRequired()) {
		int result = EventLoop_Run(GetTimerEventLoop(), -1, true);
		// Continue if interrupted by signal, e.g. due to breakpoint being set.
		if (result == -1 && errno != EINTR) {
			Terminate();
		}
	}

	ClosePeripheralsAndHandlers();
	Log_Debug("Application exiting.\n");
	return 0;
}


/// <summary>
/// Azure timer event:  Check connection status and send telemetry
/// </summary>
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	GPIO_ON(builtinLed); // blink built in LED

	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
		Log_Debug("%s\n", msgBuffer);
	}

	GPIO_OFF(builtinLed);
}


/// <summary>
///     Set up SIGTERM termination handler, initialize peripherals, and set up event handlers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static int InitPeripheralsAndHandlers(void)
{
	InitializeDevKit();  // Avnet Starter kit

	OpenPeripheralSet(peripherals, NELEMS(peripherals));
	StartTimerSet(timers, NELEMS(timers));

	return 0;
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	StopTimerSet();

	ClosePeripheralSet();
	CloseDevKit();	// Avnet Starter kit

	StopTimerEventLoop();
}