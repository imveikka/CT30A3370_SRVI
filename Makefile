CC = gcc
FLAGS = -Wall -Werror -std=c99

all: reverse my-cat my-grep my-zip my-unzip wish

%: %.c
	$(CC) $(FLAGS) -o $@ $^

clean:
	rm reverse my-cat my-grep my-zip my-unzip wish

test:
# create files where "my-" is changed to "w" for testing
	$(CC) $(FLAGS) -o wcat my-cat.c
	$(CC) $(FLAGS) -o wgrep my-grep.c
	$(CC) $(FLAGS) -o wzip my-zip.c
	$(CC) $(FLAGS) -o wunzip my-unzip.c
