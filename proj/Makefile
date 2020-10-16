CC=gcc
_DEPS = protocol.h utilities.h defines.h
_EmitterOBJ = emitter.o protocol.o emitterFuncs.o utilities.o
_ReceiverOBJ = receiver.o protocol.o receiverFuncs.o utilities.o
OBJDIR = obj
SRCDIR = src
EmitterName = emitter
ReceiverName = receiver
CFLAGS = -Wall #-pthread -lrt -lm

EmitterOBJ = $(patsubst %,$(OBJDIR)/%,$(_EmitterOBJ))
ReceiverOBJ = $(patsubst %,$(OBJDIR)/%,$(_ReceiverOBJ))
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))


$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all
all: $(EmitterName) $(ReceiverName)
	@echo done

$(EmitterName): $(EmitterOBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(ReceiverName): $(ReceiverOBJ)
	$(CC) -o $@ $^ $(CFLAGS)


.PHONY: clean
clean: 
	@rm -f -r $(OBJDIR) $(EmitterName) $(ReceiverName)
