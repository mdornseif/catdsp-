# $Id: Makefile,v 1.1 2001/07/10 20:54:50 robotnik Exp $
CC=gcc
TARGET=catdsp+
OBJS=catdsp+.o
CFLAGS=-g -Wall
RM=rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

default: $(TARGET)

clean: $(TARGET)
	$(RM) $(TARGET) $(OBJS)
