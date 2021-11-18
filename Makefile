BIN="./bin"
bin_folder:
	mkdir -p ${BIN}
server:codigo03_serverstream.c
	gcc -o ${BIN}/$@ $<
client:codigo03_clientstream.c
	gcc -o ${BIN}/$@ $<
clean:server client bin_folder
	rm ${BIN}/*
build: bin_folder server client

run: build
	${BIN}/server 2> errores.log

run_client:build
	${BIN}/client localhost
