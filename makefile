all: prog2 

prog1:
	gcc -std=gnu99 -Wall -fsanitize=address,undefined  serverudp.c -lpthread -lm -lrt -o myserver

prog2:
	gcc -std=gnu99 -Wall -fsanitize=address,undefined  serverTcp.c -lpthread -lm -lrt -o myclient

matar:
	killall -s SIGINT myserver
mataruno:
	pkill myserver

hablar:
	nc -u 127.0.0.1 2000

mostrarProcesos:
	ps -e -f

zip:
	tar -cjf espins.tar.bz2 simple.c
	tar -tjf espins.tar.bz2
upload:
	cp espins.tar.bz2 /home2/samba/sobotkap/unix

