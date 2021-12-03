# Makefile for the CS:APP Shell Lab

CC = gcc
CFLAGS = -Wall -O2

tsh: wrapper.h job.h sighandler.h tinyshell.h wrapper.c job.c sighandler.c tinyshell.c
	$(CC) $(CFLAGS) -o tsh tsh.c wrapper.c job.c sighandler.c tinyshell.c
