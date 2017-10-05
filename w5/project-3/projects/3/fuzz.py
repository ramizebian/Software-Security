import subprocess
from time import sleep
import sys

fuzzee = subprocess.Popen([sys.argv[1]],stdout=subprocess.PIPE,stderr=subprocess.PIPE,stdin=subprocess.PIPE)

enterWis = False
iter = 0
max = 1000
seed = 12458341
while fuzzee.returncode == None and iter < max:
  print "%d/%d" % (iter,max)
  #get fuzz input
  fuzzer = subprocess.Popen(["radamsa", "-s", str(seed), "fuzzinput"],stdout=subprocess.PIPE)
  seed = seed + 1
  inp, err = fuzzer.communicate()
  print "trying %s" % inp
  out = ""
  while True:
    c = fuzzee.stdout.read(1)
    out = out + c
    if c == ">":
      enterWis = False
      break
    r = out.find("Enter some wisdom")
    if r >= 0:
      enterWis = True
      break
  fuzzee.stdout.flush()
  fuzzee.stdin.flush()
  if enterWis == False:
    fuzzee.stdin.write("%s\n" % inp)
  else:
    fuzzee.stdin.write("%s\n" % inp)
  fuzzee.stdin.flush()
  fuzzee.poll()
  iter = iter + 1

if fuzzee.returncode == None:
  print "did not crash"
else:
  print "crashed with %s" % inp
