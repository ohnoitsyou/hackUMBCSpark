import urllib2
import time

from random import randrange
'''
for r in range(0,255,10):
  for b in range(0,255,10):
    for g in range(0,255,10):
      time.sleep(.05)
      try:
        response = urllib2.urlopen('http://lights.ohnoitsyou.net/oniy/setcolors/' + str(r) + "/" + str(b) + "/" + str(g))
      except: pass
'''
while(True):
  time.sleep(.10)
  response = urllib2.urlopen('http://lights.ohnoitsyou.net/oniy-lights/setcolors/' + str(randrange(0,255)) + "/" + str(randrange(0,255)) + "/" + str(randrange(0,255)))
