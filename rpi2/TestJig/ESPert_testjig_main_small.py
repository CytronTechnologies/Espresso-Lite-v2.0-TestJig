import RPi.GPIO as GPIO
import time
import os
from oled.device import ssd1306, sh1106
from oled.render import canvas
from PIL import ImageFont, ImageDraw, Image

filepath = '/home/pi/TestJig'
test_program = 'Espresso-Lite-Testjig-small-rpi2.ino.bin'
test_program_asus = 'Espresso-Lite-Testjig-small-rpi2-asus.ino.bin'
user_program = '_2000_ESPert_workshop.ino.bin'

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(18, GPIO.OUT)
GPIO.setup(21, GPIO.IN, GPIO.PUD_UP)
GPIO.setup(23, GPIO.IN, GPIO.PUD_UP)
GPIO.setup(24, GPIO.IN, GPIO.PUD_UP)
GPIO.setup(25, GPIO.IN, GPIO.PUD_UP)

device = ssd1306(port=1, address=0x3C)

#font type
font_dir = '/home/pi/ssd1306/fonts/C&C Red Alert [INET].ttf'
font_dir2 = '/home/pi/ssd1306/fonts/arial.ttf'
#font = ImageFont.load_default()
font1 = ImageFont.truetype(font_dir, 18)
font2 = ImageFont.truetype(font_dir, 12)
font3 = ImageFont.truetype(font_dir2, 16)
font4 = ImageFont.truetype(font_dir2, 12)

stage = 0
current_stage = 1
continue_bit = 0
wifi = 0
err=0

GPIO.output(18, GPIO.HIGH)
with canvas(device) as draw:
  logo = Image.open('ESPert_logo.bmp')
  draw.bitmap((32, 0), logo, fill=1)
time.sleep(2)

try:
 while True:
  if (GPIO.input(21) == 0):
   wifi = wifi + 1
   if(wifi > 1):
    wifi = 0
   with canvas(device) as draw:
    draw.text((0,16), 'WiFi selected:', font=font3, fill=255)
    if(wifi==0):
     draw.text((0,33), 'Cytron Yes', font=font3, fill=255)
    elif(wifi==1):
     draw.text((0,33), 'Cytron-Asus', font=font3, fill=255)
   time.sleep(1.5)
   stage = 0
  if (GPIO.input(23) == 0 or continue_bit == 1) and not err:
   with canvas(device) as draw:
    draw.text((0,16),'Uploading',font=font3,fill=255)
    draw.text((0,32),'Test Program', font=font3,fill=255)
   time.sleep(0.1)
   if(wifi==0):
    os.system(filepath+'/batch_upload.sh '+test_program)
   elif(wifi==1):
    os.system(filepath+'/batch_upload.sh '+test_program_asus)
   time.sleep(0.5)
  if GPIO.input(24) == 0:
   with canvas(device) as draw:
    draw.text((0,16), 'Uploading', font=font3, fill=255)
    draw.text((0,32), 'User Program', font=font3, fill=255)
   os.system(filepath+'/batch_upload.sh '+user_program)
   time.sleep(0.5)
  if GPIO.input(25) == 0:
   count = 3
   while GPIO.input(25) == 0:
    time.sleep(1)
    count = count - 1
    if count == 0:
     os.system('sudo python msg_oled.py "Shutdown now..." "" "Wait till green LED" "off"')
     #os.system('echo entering system shutdown...')
     time.sleep(1)
     os.system('sudo shutdown -hP now')
     raw_input()
  if(stage!=current_stage):
   with canvas(device) as draw:
    draw.text((0, 0), 'Currently at', font=font3, fill=255)
    draw.text((0, 16), 'STANDBY', font=font4, fill=255)
    draw.text((0, 38), 'Press button 1 - test', font=font4, fill=255)
    draw.text((0, 50), 'Press button 2 - user', font=font4, fill=255)
   stage = current_stage

except:
 GPIO.cleanup()
