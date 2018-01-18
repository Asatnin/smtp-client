C = gcc
CFLAGS = -std=c99 -D_BSD_SOURCE
CLIBS = -lpcre -pthread -lresolv -lrt -lm -lssl -lcrypto
CLIBS_TEST = -lpcre -pthread -lresolv -lrt -lm -lssl -lcrypto -lcunit
SRC = client.c common_structs.c converter.c files_crawler.c logger.c main.c network_helper.c smtp.c str_helper.c
SRC_TEST = client.c common_structs.c converter.c files_crawler.c logger.c network_helper.c smtp.c str_helper.c test/test.c

client:
	$(C) $(CFLAGS) -o main $(SRC) $(CLIBS)

test_build:
	$(C) $(CFLAGS) -o test/test $(SRC_TEST) $(CLIBS_TEST)

clean:
	rm -rf *.o main test/test

test_memory:
	@valgrind --tool=memcheck --leak-check=full ./main

test_system:
	@echo "Prepare system tests..."
	@echo "Run our client program"
	./main &
	@echo "Start local SMTP-server"
	python test/server.py &
	@sleep 5
	@echo "Run system tests"
	python test/test_system.py
