# paths
LIB = ../../lib
INCLUDE = ../../include
MODULES=../../modules

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall  -g -I$(INCLUDE)
LDFLAGS = -lm

# Αρχεία .o
OBJS = myex.o $(MODULES)/life.o $(LIB)/k08.a $(LIB)/libbmp.a 

# Το εκτελέσιμο πρόγραμμα
EXEC = myex

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS = test.RLE 100 -100 -100 300 500 1 1 10 final.gif

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

# Για να φτιάξουμε το k08.a, τρέχουμε το make στο lib directory.
$(LIB)/k08.a:
	$(MAKE) -C $(LIB) k08.a

# το ίδιο για το libbmp.a
$(LIB)/libbmp.a:
	$(MAKE) -C $(LIB) libbmp.a

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)
