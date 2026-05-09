# Flags
CC = gcc
CFLAGS = -Wall

# Sorgenti
SOURCES = $(wildcard *.c)

# Outputs
OUTPUT = client.exe
OBJ_OUTPUT = client.debug

# Regola di default
all: $(OUTPUT)

# Combina tutti i .c in file oggetto (.o)
$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)
	
# Pulizia
clean:
	rm -f $(OUTPUT)

# Debugging	
debug:
	$(CC) -g $(CFLAGS) $(SOURCES) -o $(OBJ_OUTPUT)

