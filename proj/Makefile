CC=gcc
_DEPS = protocol/protocol.h utilities/utilities.h protocol/defines.h protocol/iPA.h protocol/receiverFuncs.h protocol/transmitterFuncs.h transmitter.h receiver.h application.h defines.h utilities/globalDefines.h ui.h tests/tests.h
_mainOBJ = main.o application.o transmitter.o receiver.o protocol/iPA.o protocol/protocol.o protocol/transmitterFuncs.o protocol/receiverFuncs.o utilities/utilities.o ui.o
#_EmitterOBJ = emitter.o protocol.o emitterFuncs.o receiverFuncs.o utilities.o iPA.o
#_ReceiverOBJ = receiver.o protocol.o receiverFuncs.o emitterFuncs.o utilities.o iPA.o
OBJDIR = obj
SRCDIR = src
MainName = app
#EmitterName = emitter
#ReceiverName = receiver
CFLAGS = -Wall #-pthread -lrt -lm

#EmitterOBJ = $(patsubst %,$(OBJDIR)/%,$(_EmitterOBJ))
#ReceiverOBJ = $(patsubst %,$(OBJDIR)/%,$(_ReceiverOBJ))
mainOBJ = $(patsubst %,$(OBJDIR)/%,$(_mainOBJ))
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))


$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/protocol
	@mkdir -p $(OBJDIR)/utilities
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: all
all: $(MainName) tests
	@echo done

$(MainName): $(mainOBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean: 
	@rm -f -r $(OBJDIR) $(MainName) $(ReceiverName)
	@rm -f tests

.PHONY: tests
tests: $(DEPS)
	gcc -o test src/tests/main.c src/tests/tests.c src/protocol/iPA.c src/protocol/protocol.c src/protocol/receiverFuncs.c src/protocol/transmitterFuncs.c src/application.c src/receiver.c src/transmitter.c src/ui.c src/utilities/utilities.c


.PHONY: cable
cable:
	gcc -o cable cable.c
