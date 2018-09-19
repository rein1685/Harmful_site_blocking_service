#obj-m := test.o
#test-y := conn.o
#test-y += userlibrary.o

obj-m := imsi.o
imsi-objs := test.o conn.o userlibrary.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

#install:
	#sudo rmmod hello; \
	#sudo mkdir -p /lib/modules/$(KVERSION)/kernel/drivers/hello
	#sudo cp hello.ko /lib/modules/$(KVERSION)/kernel/drivers/hello
	#sudo depmod -a
	#sudo modprobe hello
