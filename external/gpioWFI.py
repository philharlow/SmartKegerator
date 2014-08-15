import sys
import RPi.GPIO as GPIO

def leftFlowMeterTick(signal):
	print "Left beer tick!"
	sys.stdout.flush()

def rightFlowMeterTick(signal):
	print "Right beer tick!"
	sys.stdout.flush()

def main(argv):
	if len(argv) != 2:
		print "Usage: gpioWFI flowMeter1Pin flowMeter2Pin"
		return

	# get params
	leftPin = int(float(argv[0]))
	rightPin = int(float(argv[1]))

	# Setup GPIO
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(leftPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	GPIO.setup(rightPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	
	GPIO.add_event_detect(leftPin, GPIO.RISING, callback=leftFlowMeterTick, bouncetime=20)
	GPIO.add_event_detect(rightPin, GPIO.RISING, callback=rightFlowMeterTick, bouncetime=20)
	
	try:
		raw_input()
	except KeyboardInterrupt:
		print "Keyboard interrupt!"
	except:
		print "Other interrupt!"
	finally:
		GPIO.cleanup()
		print "Cleaned up GPIO!"

	print "Exiting!"


if __name__ == "__main__":
	main(sys.argv[1:])