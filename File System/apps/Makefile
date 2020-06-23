CC := gcc
CFLAGS := -g -Wall -Wno-deprecated-declarations -Werror

TESTS := test_1 test_2 test_3 test_4 test_5 test_6

all: $(TESTS)

clean:
	rm -rf $(TESTS) *.dSYM

test%: test%.c ../io/file.c 
	$(CC) $(CFLAGS) -I.. -o $@ $< ../io/file.c -lm

    
    