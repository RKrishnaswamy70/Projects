# Documentation for gpiozero at:
#    https://gpiozero.readthedocs.io/en/stable
# import RPi.GPIO as GPIO
from gpiozero import LED

# pins = {"A" : 6, "B" : 13, "C" : 19, "D" : 26}
ledDict = {"A" : LED[6], "B" : LED[13], "C" : LED[19], "D" : LED[26]}


# This is the function you will need to turn on the led
def turnOn(led):
    # GPIO.output(pins[led], GPIO.HIGH)
    ledDict[led].on()
    return

# This is the function you will need to turn off the led
def turnOff(led):
    # GPIO.output(pins[led], GPIO.LOW)
    ledDict[led].off()
    return

# Normally we don't need to do this with gpiozero.  However, it is
# safer to programmatically shut down the pins.
def shutdown():
    for led in ledDict.values():
        led.close()
    return

# Put code here to initialize the Raspberry Pi.
# The 4 LEDs must initially be off.
#
# When using gpiozero, there is nothing to do here.  The gpiozero
# module will initialize automatically.



