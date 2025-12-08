# Bluetooth Commands guide

In this code i have implented some commands that are to be transferred over Bluetooth
In general most commands are identified by the first letter of the string

> [!CAUTION]
> For any string sent, the first char will be attempted to be interpreted as a command, so if you wite "good day kind sir" it will be assumed to be command g
> Any other string will simply be printed to the ESPI_LOG

Here is the guide:

## Unlocks counter

This command will either get the ESP32's current unlock counter or set it to a new value

> [!WARNING]
> If the value is negative it cannot be unlocked

### Special Letter: 'u'

### Command structure

Sending just the special letter will return the number of unlocks left, this is done via notify, and reading again

Alternatively one can the a value along the special letter. which will set the internal counter of the ESP32 to the value given.
It only accepts intergers (AKA i have not tested floats, dont send them), there can be any amount of leading space after the special letter and the acualt int

### Examples

"u" -> returns: "47"
"u12" -> Counter changed to 12
"u 23" -> Counter changed to 23
"u -5" -> Counter changed to -5, It is now inoperable

## Google Find My Tools Key

This command will either get or set the ESP32's EID for google find my tools, refer to that repo if you care.
The EID is 40 chars of hex or 20 bytes, it can only be sent as chars

> [!WARNING]
> IF THE EID IS INVALID THE ESP32 WILL FAIL TO BROADCAST ITS LOCATION

### Special Letter: 'g'

### Command structure

Sending just the special letter will return the EID

Sending a string along with the special letter will set the internal EID, this will first take effect after it goes to sleep or reboots.
The command format allows for a space between the special letter and EID.
If the EID is not of correct length it will default to returning the internaly stored EID

### Examples

> [!NOTE]
> The example EID's are not valid, nor correct formatting, actual usecases assume 20 bytes or 40 hex chars

"g" -> returns: "BBQuB62qghlAxwRsspLt0fGraeIjczmWMU5VGU1E"
"gfgSWoJxkG5PxKzfakhjSnt8DOYKCnvHEh1w94oLM" -> EID is now "fgSWoJxkG5PxKzfakhjSnt8DOYKCnvHEh1w94oLM"
"g vGnhMsa3pggnSQ9iYUBsZUcf65ASkdSHVkx1oUjq" -> EID is now "vGnhMsa3pggnSQ9iYUBsZUcf65ASkdSHVkx1oUjq"
"g n8vSdQ6hYf4MWYXTeRIN" -> returns: "BBQuB62qghlAxwRsspLt0fGraeIjczmWMU5VGU1E"

## Rolling Codes Counter

> [!NOTE]
> This command is subject to be removed

This command will reset the ESP32's internal rolling codes counter

### Special Letter: 'r'

### Command structure

Sending the special letter will reset the internal counter

### Examples

"r" -> returns: "Counter was reset"
"r 2" -> returns: "Counter was reset", note: ignores '2'

## Open Briefcase

This command will unlock the briefcase if the code is correct

> [!NOTE]
> Not yet implented

### Special Letter: 'o'
