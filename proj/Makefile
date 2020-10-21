CC=gcc
_DEPS = protocol.h utilities.h defines.h iPA.h receiverFuncs.h emitterFuncs.h emitter.h receiver.h application.h
_mainOBJ = main.o application.o emitter.o receiver.o iPA.o protocol.o emitterFuncs.o receiverFuncs.o utilities.o
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
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all
all: $(MainName)
	@echo done

$(MainName): $(mainOBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean: 
	@rm -f -r $(OBJDIR) $(MainName) $(ReceiverName)
