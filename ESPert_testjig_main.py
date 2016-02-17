import RPi.GPIO as GPIO
import time
import os
from oled.device import ssd1306, sh1106
from oled.render import canvas
from PIL import ImageFont, ImageDraw, Image

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(18, GPIO.OUT)
GPIO.setup(23, GPIO.IN, GPIO.PUD_UP)
GPIO.setup(24, GPIO.IN, GPIO.PUD_UP)
GPIO.setup(25, GPIO.IN, GPIO.PUD_UP)

device = ssd1306(port=1, address=0x3C)

#font type
font_dir = '/home/pi/ssd1306/fonts/C&C Red Alert [INET].ttf'
#font = ImageFont.load_default()
font1 = ImageFont.truetype(font_dir, 18)
font2 = ImageFont.truetype(font_dir, 12)

stage = 0
current_stage = 1
continue_bit = 0
err=0

GPIO.output(18, GPIO.HIGH)
with canvas(device) as draw:
  logo = Image.open('ssd1306/examples/images/ESPert_logo.bmp')
  draw.bitmap((32, 0), logo, fill=1)
time.sleep(2)

try:
 while True:
  if (GPIO.input(23) == 0 or continue_bit == 1) and not err:
   continue_bit = 1
   if stage == 0:
    stage = 1
   elif stage == 1:
    stage = 2
    retry = 2
    while retry:
      code = os.system('/home/pi/batch_upload.sh Blink.ino.bin')
      if code == 0:
        break
      retry = retry - 1
      if retry == 0:
        err=1
        continue_bit=0
        stage = 0
        current_stage = 0
   elif stage == 2:
    os.system('sudo python msg_oled.py "Sending command" "to online printer..."')
    stage = 3
   elif stage == 3:
    stage = 0
    retry = 2
    while retry:
      code = os.system('/home/pi/batch_upload.sh Blink.ino.bin')
      if code == 0:
        break
      retry = retry - 1
      if retry == 0:
        err=1
        continue_bit=0
        stage = 2
        current_stage = 2
    continue_bit = 0
   time.sleep(0.5)
  if GPIO.input(24) == 0:
   err=0
   stage = stage + 1
   if stage>3:
    stage = 0
   time.sleep(0.5)
  if GPIO.input(25) == 0:
   count = 3
   while GPIO.input(25) == 0:
    time.sleep(1)
    count = count - 1
    if count == 0:
     os.system('sudo python msg_oled.py "Entering system" "shutdown now..." "" "Wait until green LED off" "before turning off power"')
     os.system('echo entering system shutdown...')
     time.sleep(1)
     os.system('sudo shutdown -hP now')
     raw_input()
  if (current_stage != stage) and (err==0):
    with canvas(device) as draw:
     draw.text((0, 0), 'Currently at', font=font2, fill=255)
     if stage>0: 
      draw.text((0, 14), 'STAGE '+str(stage), font=font1, fill=255)
     else:
      draw.text((0, 14), 'STANDBY', font=font1, fill=255)
     if continue_bit == 0:
      draw.text((54, 38), 'Press button 1', font=font2, fill=255)
      draw.text((72, 50), 'to continue', font=font2, fill=255)
    current_stage = stage

except:
 GPIO.cleanup()
