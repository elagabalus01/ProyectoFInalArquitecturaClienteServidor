BIN="./bin"
SRC=./src
bin_folder:
	mkdir -p ${BIN}
server:${SRC}/serverstream.c
	gcc -o ${BIN}/$@ $<
client:${SRC}/clientstream.c
	gcc -o ${BIN}/$@ $<
clean:server client bin_folder
	rm ${BIN}/*
build: bin_folder server client

run: build
	${BIN}/server 2> errores.log

run_client:build
	${BIN}/client localhost
