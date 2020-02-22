#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys, os, cgi, datetime
import traceback, logging

length = os.getenv('CONTENT_LENGTH')
logging.basicConfig(filename='htdocs/cgi.log')

def solve():
	# 获取系统时间
	date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
	# 获取表单信息
	formdata = cgi.FieldStorage()
	num = "学号: " + formdata['num'].value
	name = "姓名: " + formdata['name'].value
	sex = "性别: " + ("男" if formdata['sex'].value == "male" else "女")
	re = "是否发热: " + ("是" if formdata['re'].value == "yes" else "否")
	virus = "是否感染新型冠状病毒: " + ("是" if formdata['virus'].value == "yes" else "否")
	# 保存到对应文件
	out = num + "\n" + name + "\n" + sex + "\n" + re + "\n" + virus + "\n\n"
	filename = formdata['num'].value + '-' + formdata['name'].value;
	fp = open('htdocs/info/'+filename, 'a')
	fp.write(date + "\n")
	fp.write(out)

	print "Content-type: text/html;charset=utf-8"
	print ""
	print "<html>"
	print "<head>"
	print "<title>Your INFO</title>"
	print "</head>"
	print '<body background="img/two.jpg" style="background-repeat:no-repeat;background-size:100% 100%;background-attachment:fixed;">'
	print '<div style="margin:auto;position:absolute;top:0;left:0;right:0;bottom:0;width:300px;height:300px;">'
	print "<h1>你提交的信息</h1>"
	print "<ul>"
	print "<li>" + name + "</li>";
	print "<li>" + num + "</li>";
	print "<li>" + sex + "</li>";
	print "<li>" + re + "</li>";
	print "<li>" + virus + "</li>";
	print "</ul>"
	print '</div>'
	print "</body>"
	print "</html>"

	fp.close()

if __name__ == '__main__':
	try:
		solve()
	except:
		s = traceback.format_exc()
		logging.error(s)
		print "Content-type: text/html;charset=utf-8"
		print ""
		print "<html>"
		print "<head>"
		print "<title>Your INFO</title>"
		print "</head>"
		print '<body>'
		print "<h1>输入信息不完整！</h1>"
		print '</body>'
		print '</html>'
