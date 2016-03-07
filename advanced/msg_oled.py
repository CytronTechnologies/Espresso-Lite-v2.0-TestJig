import sys
from oled.device import ssd1306, sh1106
from oled.render import canvas
from PIL import ImageFont, ImageDraw

args = sys.argv[1:]
if not args:
  sys.exit()

device = ssd1306(port=1, address=0x3C)
font=ImageFont.truetype('/home/pi/ssd1306/fonts/C&C Red Alert [INET].ttf', 12)

with canvas(device) as draw:
   count = 0
   for arg in args:
     draw.text((0, count),  str(arg),  font=font, fill=255)
     count = count + 12

