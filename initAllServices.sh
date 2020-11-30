touch /tmp/out1.txt
touch /tmp/out2.txt
touch /tmp/out3.txt
gnome-terminal -- python2 ./InterfaceService/Main.py
./SerialServiceEsqueleto/serialService
cd web
gnome-terminal -- ./runServer.sh
