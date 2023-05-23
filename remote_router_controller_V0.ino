#include "communication.h"
#include "configuration.h"
#include "mcp4151.h"
#include "my_spi.h"
#include "pins.h"

struct command
{
  char device;
  char number;
  char data[32];
  uint8_t data_len;
} command;

MCP4151 mcp4151;

void setup() 
{
  Serial.begin(115200);

  /* Start MCP4151 */
  mcp4151.begin();

  /* At last, print welcome message */
  welcomeMessage();
}


/*  =============================== Serial Event=================================
    serialEvent() automatically (quietly) called in the loop; in serial with
    State Machine's Queue.
    =============================================================================
*/
void serialEvent(void)
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

void loop() 
{

}

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
