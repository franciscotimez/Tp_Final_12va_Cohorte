import socket
import sys
import time
import thread
import datetime

def writeOutState(outNumber,state):
	outNumber = outNumber + 1
	fp = open("/tmp/out"+str(outNumber)+".txt","w+")
	if int(state)==1:
		fp.write("1")
	else:
		fp.write("0")
	fp.close()
	return True

def readOutState(outNumber):
	fp = open("/tmp/out"+str(outNumber)+".txt","r")
	val = fp.read()
	if val[0]=="1":
		val=True
	else:
		val=False
	fp.close()
	return val


def rcvThread(sock):
	global socketOk
	print("INICIO thread recepcion")
	while True:
		data = sock.recv(128)
		if len(data)==0:
			print("Se cerro la conexion")
			break
		print("LLEGO:"+data)
		data = data.split("SW:")
		for d in data:
			if len(d)>=3:
				inNumber = d[0]
				inValue = d[2]
				print("Nuevo estado de salida "+str(inNumber)+" valor:"+str(inValue))				
				writeOutState(int(inNumber),str(inValue))

	print("FIN thread recepcion")
	socketOk=False

socketOk=True

while True:
	try:
		# Creo TCP/IP socket
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		server_address = ('127.0.0.1', 10000)
		print >>sys.stderr, 'connecting to %s port %s' % server_address
		sock.connect(server_address)
		socketOk=True
		# Creo thread para escuchar paquetes
		thread.start_new_thread( rcvThread, (sock, ) )

		out0Old=False
		out1Old=False
		out2Old=False

		while socketOk:
			time.sleep(1)
			out0 = readOutState(1)
			if out0!=out0Old:
				out0Old=out0
				if out0:
					sock.send(">OUT:0,1\r\n")
				else:
					sock.send(">OUT:0,0\r\n")
			out1 = readOutState(2)
			if out1!=out1Old:
				out1Old=out1
				if out1:
					sock.send(">OUT:1,1\r\n")
				else:
					sock.send(">OUT:1,0\r\n")
			out2 = readOutState(3)
			if out2!=out2Old:
				out2Old=out2
				if out2:
					sock.send(">OUT:2,1\r\n")
				else:
					sock.send(">OUT:2,0\r\n")
		raise Exception

	except:			
		time.sleep(1)
		print("Socket invalido, reintento...")
