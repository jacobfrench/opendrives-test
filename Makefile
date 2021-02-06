#==========================================
#    Makefile: makefile for sl 5.1
#	Copyright 1993, 1998, 2014
#                 Toyoda Masashi
#		  (mtoyoda@acm.org)
#	Last Modified: 2021/02/06
#==========================================

CC=g++
CFLAGS=-O -Wall

all: sl

sl: sl.cpp sl.h
	$(CC) $(CFLAGS) -o sl sl.cpp -lncurses

clean:
	rm -f sl

distclean: clean

package:
	make clean
	make
	mkdir sl_package
	mkdir sl_package/DEBIAN
	mkdir sl_package/usr
	mkdir sl_package/usr/bin
	touch sl_package/DEBIAN/control
	cat package_control.txt > sl_package/DEBIAN/control

	cp sl sl_package/usr/bin
	cp -r ascii/ sl_package/usr/bin

	dpkg -b sl_package
	rm -rf sl_package/
