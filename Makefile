BOARD:=at90usb1286
CLOCK:=8000000
AVR_GCC:=avr-gcc
AVR_OBJCOPY:=avr-objcopy
DFU:=dfu-programmer

AVR_GCC_OPTS:=$(AVR_GCC_OPTS) -mmcu=$(BOARD) -DF_CPU=$(CLOCK) -Wall -Os
AVR_OBJCOPY_OPTS:=$(AVR_OBJCOPY_OPTS)
SOURCE:=$(wildcard *.c)

OBJDIR:=build/
OBJECTS:=$(addprefix $(OBJDIR),$(SOURCE:.c=.o))

DIR:=$(lastword $(subst /, ,$(CURDIR)))
OUTPUT_NAME:=$(addsuffix .hex,$(addprefix $(OBJDIR),$(DIR)))

$(OBJDIR)%.o: %.c | $(OBJDIR).dirtag
	@echo "Compiling $< into $@"
	@$(AVR_GCC) $(AVR_GCC_OPTS) -c $< -o $@

$(OUTPUT_NAME:.hex=.elf): $(OBJECTS)
	@echo "Linking $^ into $@"
	@$(AVR_GCC) $(AVR_GCC_OPTS) -o $@ $^

$(OUTPUT_NAME): $(OUTPUT_NAME:.hex=.elf)
	@echo "Creating hex $@"
	@$(AVR_OBJCOPY) $(AVR_OBJCOPY_OPTS) -O ihex $^ $@

build: $(OUTPUT_NAME)
	@echo "Build $(DIR) completed successfully."

.PHONY:upload
upload: $(OUTPUT_NAME)
	@echo "Uploading hex $<"
	@$(DFU) $(BOARD) erase 
	@$(DFU) $(BOARD) flash $^
	@$(DFU) $(BOARD) start

$(OBJDIR).dirtag:
	@echo "Creating build directory $(OBJDIR)"
	@-mkdir $(OBJDIR)
	@touch $@

.PHONY:clean
clean:
	@echo "Deleting all compiled files and removing build directory $(OBJDIR)"
	@rm -rf $(OBJDIR)