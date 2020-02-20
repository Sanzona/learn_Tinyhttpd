#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys, os
length = os.getenv('CONTENT_LENGTH')

if length:
	postdata = sys.stdin.read(int(length))
	print "Content-type: text/html;charset=utf-8"
	print ""
	print "<html>"
	print "<head>"
	print "<title>Your INFO</title>"
	print "</head>"
	print "<body>"
	print "<h1>你提交的信息</h1>"
	print "<ul>"
	for data in postdata.split('&'):
		print "<li>" + data + "</li>"
	print "</ul>"
	print "</body>"
	print "</html>"

else:
	print "Content-type: text/html"
	print ""
	print "not found"
