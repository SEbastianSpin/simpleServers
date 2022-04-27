all: prog1 prog2

prog1:
	gcc -std=gnu99 -Wall -fsanitize=address,undefined  serverTcp.c -lpthread -lm -lrt -o myc

prog2:
	gcc -std=gnu99 -Wall -fsanitize=address,undefined  tcpMapServer.c -lpthread -lm -lrt -o myMap

matar:
	killall -s SIGINT myserver
mataruno:
	pkill myMap

hablar:
	nc -u 127.0.0.1 8080

call:
	telnet 127.0.0.1 8080
checkCon:
	nc -z -v -u 0.0.0.0 2000

mostrarProcesos:
	ps -e -f

zip:
	tar -cjf espins.tar.bz2 simple.c
	tar -tjf espins.tar.bz2
upload:
	cp espins.tar.bz2 /home2/samba/sobotkap/unix

