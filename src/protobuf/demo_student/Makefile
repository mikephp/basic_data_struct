.PHONY:all clean
all:
	protoc-c --c_out=.  student.proto
	gcc student.pb-c.c main.c -o main -lprotobuf-c -I.

clean:
	rm *.o *.pb-c.c *.pb-c.h
