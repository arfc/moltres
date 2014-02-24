#!/usr/bin/env python

# This script is for creating a new herd animal.  Just run this script
# from the "stork" directory supplying a new animal name and it should
# create a complete application template built with support for both
# MOOSE and ELK.  Enjoy!

import os, sys, string, re, subprocess
from optparse import OptionParser
from shutil import copytree, ignore_patterns

global_ignores = ['.svn', '.git']
global_app_name = ''

def renameFiles(app_path):
  pattern = re.compile(r'(stork)(.*)', re.I)

  for dirpath, dirnames, filenames in os.walk(app_path):
    # Don't traverse into ignored directories
    for ignore in global_ignores:
      if ignore in dirnames:
        dirnames.remove(ignore)

    for file in filenames:
      match = pattern.match(file)

      # Replace 'stork' in the contents
      replaceNameInContents(dirpath + '/' + file)

      # See if the file needs to be renamed and rename
      if match != None:
        replace_string = replacementFunction(match)
        os.rename(dirpath + '/' + file, dirpath + '/' + replace_string + match.group(2))


def replaceNameInContents(filename):
  f = open(filename)
  text = f.read()
  f.close()

  # Replace all instances of the word stork with the right case
  pattern = re.compile(r'(stork)', re.I)
  text = pattern.sub(replacementFunction, text)

  # Retrieve original file attribute to be applied later
  mode = os.stat(filename).st_mode

  # Now write the file back out
  f = open(filename + '~tmp', 'w')
  f.write(text)
  f.close()
  os.chmod(filename + '~tmp', mode)
  os.rename(filename + '~tmp', filename)

def replacementFunction(match):
  # There are 3 "case" cases
  # Case 1: all lower case
  if match.group(1) == 'stork':
    return global_app_name

  # Case 2: all upper case
  if match.group(1) == 'STORK':
    return string.upper(global_app_name)

  # Case 3: First letter is capitalized
  if match.group(1) == 'Stork':
    name = global_app_name.replace("_", " ")
    name = name.title()
    name = name.replace(" ", "")
    return name
  
  print match.group(0) + "\nBad Case Detected!"
  sys.exit(1)

def printUsage():
  print './make_new_application.py <animal name>'
  sys.exit()

if __name__ == '__main__':
  parser = OptionParser()
  (global_options, args) = parser.parse_args()

  # Make sure an animal name was supplied (args[0])
  if len(args) != 1:
    printUsage()
  
  global_app_name = string.lower(args[0])
  renameFiles('.')

  os.rename('Makefile.app', 'Makefile')
  try:
    os.remove('Makefile.module')
    os.remove('make_new_application.py')
    os.remove('make_new_module.py')
  except:
    pass

  # Add the newly created untracked files and delete the removed ones
  subprocess.check_output("git rm -f *.py Makefile.app Makefile.module", shell=True)
  subprocess.call("git add --all *", shell=True)

  print 'Your application should be ready!\nCommit this directory to your local repository and push.'
