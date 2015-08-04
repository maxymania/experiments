obj-m += rock_lsm.o

all:
	echo "Hello World"
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	echo "Hello World"
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
