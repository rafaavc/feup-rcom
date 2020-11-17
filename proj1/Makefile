CC=gcc
_DEPS = protocol/protocol.h protocol/stateMachine.h tests/efficiency.h utilities/utilities.h protocol/defines.h protocol/iPA.h protocol/receiverFuncs.h protocol/linkLayer.h protocol/transmitterFuncs.h application/transmitter.h application/receiver.h application/application.h application/defines.h application/stateMachine.h utilities/globalDefines.h utilities/ui.h
_mainOBJ = main.o application/application.o tests/efficiency.o application/transmitter.o application/receiver.o application/stateMachine.o protocol/linkLayer.o protocol/iPA.o protocol/protocol.o protocol/stateMachine.o protocol/transmitterFuncs.o protocol/receiverFuncs.o utilities/utilities.o utilities/ui.o
OBJDIR = obj
SRCDIR = src
MainName = app
CFLAGS = -Wall -lm

mainOBJ = $(patsubst %,$(OBJDIR)/%,$(_mainOBJ))
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))


$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/protocol
	@mkdir -p $(OBJDIR)/utilities
	@mkdir -p $(OBJDIR)/application
	@mkdir -p $(OBJDIR)/tests
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: all
all: $(MainName)
	@echo done

$(MainName): $(mainOBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean: 
	@rm -rf $(OBJDIR) $(MainName) $(ReceiverName) cable

.PHONY: cable
cable:
	gcc -o cable cable.c
