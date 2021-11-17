BIN="./bin"
server:codigo03_serverstream.c
	gcc -o ${BIN}/$@ $<
client:codigo03_clientstream.c
	gcc -o ${BIN}/$@ $<
clean:server client
	rm ${BIN}/*
build: server client

run: build
	${BIN}/server
