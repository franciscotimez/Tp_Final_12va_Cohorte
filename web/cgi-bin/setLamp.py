#! /usr/bin/env python
# 	 PHP Conference 2014. Demo Desarrollo con Python sobre Sistemas Embebidos. 
#    Copyright (C) 2014  Ernesto Gigliotti. Laboratorio de Software Libre UTN-FRA
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
print("Content-Type: text/html")
print("")
import cgi


arguments = cgi.FieldStorage()

try:
	lampNum = int(arguments["id"].value.split("_")[1])
	if arguments["state"].value=="true":
		lampVal = True
	else:
		lampVal = False

	fp = open("/tmp/out"+str(lampNum)+".txt", 'w+')
	if lampVal:
		fp.write("1")
	else:
		fp.write("0")
	fp.close()
	print("["+str(lampNum)+","+str(lampVal)+"]")

except Exception as e:
	print(e)
