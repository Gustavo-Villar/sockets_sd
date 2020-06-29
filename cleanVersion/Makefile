compile:
	gcc master.c -o master
	gcc slave.c -o slave -lm
	
slave1:
	./master&
	sleep 2
	./slave
	
slave2:
	./master&
	sleep 2
	./slave& # Slave1
	./slave  # Slave2
	
slave4:
	./master&
	sleep 2
	./slave& # Slave1
	./slave& # Slave2
	./slave& # Slave3
	./slave # Slave4
	
slave10:
	./master&
	sleep 2
	./slave& # Slave1
	./slave& # Slave2
	./slave& # Slave3
	./slave& # Slave4
	./slave& # Slave5
	./slave& # Slave6
	./slave& # Slave7
	./slave& # Slave8
	./slave& # Slave9
	./slave  # Slave10
	
