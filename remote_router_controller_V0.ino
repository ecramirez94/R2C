#include "communication.h"
#include "configuration.h"
#include "mcp4151.h"
#include "my_spi.h"
#include "pins.h"
#include "timers.h"

#define DISPLAY_UPDATE_INTERVAL(MS) MS / 10

volatile bool update_display = false;
volatile bool serial_enabled = false;
volatile uint16_t tick_count = false;
uint16_t display_update_interval = DISPLAY_UPDATE_INTERVAL(250); // In integer milliseconds in multiples of 10mS

struct command
{
  char device;
  char number;
  char data[32];
  uint8_t data_len;
} command;

MCP4151 mcp4151;
Timers timers;

void setup()
{
  /* Enable/Disable peripherals 0 = Enabled, 1 = Disabled */
  PRR = (0 << PRTWI) | (1 << PRTIM2) | (0 << PRTIM0) | (1 << PRTIM1) | (0 << PRSPI) | (1 << PRUSART0) | (0 << PRADC);

  pinMode(ROUTER_SPEED_CONTROL_PIN, INPUT);
  pinMode(ROUTER_LOAD_PIN, INPUT);

  /* Initialize MCP4151 Digipot */
  mcp4151.begin();

  /*
      ENABLE_SERIAL_PIN is a user selectable input (active LOW) on the PCB to allow for Serial communication with a host computer.
      This is optional and considered an advanced option; and is not required for normal system operation.
  */
  pinMode(ENABLE_SERIAL_PIN, INPUT_PULLUP);

  if (!digitalRead(ENABLE_SERIAL_PIN))
  {
    PRR = (0 << PRUSART0); // Turn on USART Peripheral
    Serial.begin(115200);
    serial_enabled = true;
  }

  /* Print welcome message over Serial interface if enabled. */
  if (serial_enabled)
    welcomeMessage();

  /* Initialize 128x64 2.42" OLED Display */

  /* Print Splash screen on OLED Display and wait for about 3 seconds. */

  /* Next, clear splash screen and build display frame */

  /* Finally, start display update timer and drop into normal program loop */
  timers.startDisplayTimer();
}


/*  =============================== Serial Event=================================
    serialEvent() automatically (quietly) called in the loop; in serial with
    State Machine's Queue.
    =============================================================================
*/
void serialEvent(void)
{
  if (serial_enabled)
  {
    String s = Serial.readStringUntil('\n');  // Read until newline char. The newline char is truncated.
  
    parseCommand(&command, s);
  
    /* The first two characters of the new command string direct the command data to the correct function.
        Then, the remaining payload and its length are given as arguments to their handling function.
        It is the responsibility of the individual functions to further unpack, convert, and otherwise
        interpret the data.
    */
    switch (command.device)
    {
      case dev_MCP4151:
        switch (command.number)
        {
          case MCP4151_WRITE_VALUE: /* B0<value> */
            mcp4151.setCount(command.data, command.data_len) ? Serial.println(F("ok")) : Serial.println(F("Invalid count"));
            break;
          case MCP4151_INCREMENT: /* "B1" */
            Serial.println("Count: " + String(mcp4151.increment()));
            break;
          case MCP4151_DECREMENT: /* "B2" */
            Serial.println("Count: " + String(mcp4151.decrement()));
            break;
        }
        break;
    }
  }
}

void loop()
{
  /* Measure and calculate values */

  /* Update display on each timer interrupt */
  if (update_display)
  {

    update_display = false;
  }

  /* Serial Event */
}

/* ==== Interrupt Service Routines ==== */
ISR(vect_TCNT0_OCR0A)
{
  tick_count++;

  if (tick_count >= display_update_interval)
  {
    update_display = true;
    tick_count = 0; 
  }
}

/* ===================================== */

void parseCommand(struct command *c, String s)
{
  c->device = s[0];
  c->number = s[1];
  c->data_len = (s.length() - 2);

  for (uint8_t i = 0; i < c->data_len; i++)
    c->data[i] = s[i + 2];

  /* Manually add char array terminator */
  c->data[c->data_len] = '\0';
}

void welcomeMessage(void)
{
  Serial.println(F("Remote Router Controller (R2C) "));
  Serial.println(F("***********************************\n"));
  Serial.print(F("Configured for: "));
  Serial.println(F("Makita RT0701C"));
  Serial.println(F("Please visit Github Wiki for command set and other information:"));
  Serial.println(F("www.github.com"));
}
