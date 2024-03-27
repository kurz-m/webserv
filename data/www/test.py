#!/usr/bin/python3
import cgi
import cgitb
import sys

cgi.test()
cgitb.enable()

data = sys.stdin.buffer.read()
with open('file.html', 'wb') as f:
    f.write(data)
print('Content-Type: text/plain\r\n\r\n', end='')
print('Success!')
