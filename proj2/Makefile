CC=gcc
_DEPS = commandManager.h connection.h fileManager.h
_mainOBJ = main.o commandManager.o fileManager.o connection.o
OBJDIR = obj
SRCDIR = src
MainName = download
CFLAGS = -Wall -lm

mainOBJ = $(patsubst %,$(OBJDIR)/%,$(_mainOBJ))
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))


$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: all
all: $(MainName)
	@echo done

$(MainName): $(mainOBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean: 
	@rm -rf $(OBJDIR) $(MainName) $(ReceiverName) cable
