
# MCU name
MCU = atmega328p

# Processor frequency.
#     This will define a symbol, F_CPU, in all source code files equal to the 
#     processor frequency. You can then use this symbol in your source code to 
#     calculate timings. Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
#     Typical values are:
#         F_CPU =  1000000
#         F_CPU =  1843200
#         F_CPU =  2000000
#         F_CPU =  3686400
#         F_CPU =  4000000
#         F_CPU =  7372800
#         F_CPU =  8000000
#         F_CPU = 11059200
#         F_CPU = 14745600
#         F_CPU = 16000000
#         F_CPU = 18432000
#         F_CPU = 20000000
F_CPU = 16000000


# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Target file name (without extension).

APP_NAME = AvrRTOSDemo

RTOS_INC_DIR = ../FreeRTOS/Source/include
RTOS_SRC_DIR = ../FreeRTOS/Source
RTOS_PORT_DIR = ../FreeRTOS/user
#RTOS_PORT_MEM_DIR = FreeRTOS/Source/Portable/MemMang

# Directory structure
APP_INC_DIR = ../headers
APP_SRC_DIR = ../src
APP_ASM_DIR = ../startup


VPATH = $(APP_SRC_DIR) $(APP_ASM_DIR)
VPATH += $(RTOS_SRC_DIR) $(RTOS_PORT_DIR)
#VPATH +=  $(RTOS_PORT_MEM_DIR)

OUTPUTDIR = ./
# Object files directory
OBJDIR = obj


ALL_DIR = $(APP_SRC_DIR) $(APP_INC_DIR) $(APP_ASM_DIR)
ALL_DIR += $(RTOS_SRC_DIR) $(RTOS_INC_DIR) $(RTOS_PORT_DIR)
ALL_DIR += $(OBJDIR) $(OUTPUTDIR)

# Executables
#EXE = $(APP_NAME).elf
# AVR studio debugger requires TARGET defined as equal to project name
# must implement "clean"
# make file and target must exist in same folder
TARGET = $(APP_NAME).elf 

RTOSSRC = $(RTOS_SRC_DIR)/tasks.c
RTOSSRC += $(RTOS_SRC_DIR)/queue.c
RTOSSRC += $(RTOS_SRC_DIR)/list.c

RTOSSRC += $(RTOS_PORT_DIR)/port.c
# RTOSSRC += $(RTOS_PORT_MEM_DIR)/heap_1.c

RTOSSRCOPTIONAL = $(RTOS_SRC_DIR)/timers.c
RTOSSRCOPTIONAL += $(RTOS_SRC_DIR)/event_groups.c
RTOSSRCOPTIONAL += $(RTOS_SRC_DIR)/stream_buffer.c
RTOSSRCOPTIONAL += $(RTOS_SRC_DIR)/croutine.c

# List C source files here. (C dependencies are automatically generated.)
CSRC = $(wildcard $(APP_SRC_DIR)/*.c)
CSRC += $(RTOSSRC)

# List C++ source files here. (C dependencies are automatically generated.)
CPPSRC = 

# List Assembler source files here.
#     Make them always end in a capital .S.  Files ending in a lowercase .s
#     will not be considered source files but generated files (assembler
#     output from the compiler), and will be deleted upon "make clean"!
#     Even though the DOS/Win* filesystem matches both .s and .S the same,
#     it will preserve the spelling of the filenames, and gcc itself does
#     care about how the name is spelled on its command-line.
ASRC = 
#$(wildcard $(APP_ASM_DIR)/*.S)


# Optimization level, can be [0, 1, 2, 3, s]. 
#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = 0

# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DEBUG = dwarf-2

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
INCLDIRS = $(APP_INC_DIR)
INCLDIRS += $(RTOS_PORT_DIR)
INCLDIRS += $(RTOS_INC_DIR)
INCLFLAGS = $(patsubst %,-I%,$(INCLDIRS))


# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     c99   = ISO C99 standard (not yet fully implemented)
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here for C sources
CDEFS = -DF_CPU=$(F_CPU)UL

# Place -D or -U options here for ASM sources
ADEFS = -DF_CPU=$(F_CPU)

# Place -D or -U options here for C++ sources
CPPDEFS = -DF_CPU=$(F_CPU)UL
#CPPDEFS += -D__STDC_LIMIT_MACROS
#CPPDEFS += -D__STDC_CONSTANT_MACROS

# Linker files
LNKR            = -T $(APP_NAME).ld


#---------------- Compiler Options C ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
CFLAGS += -g3
CFLAGS += $(CDEFS)
CFLAGS += $(CSTANDARD)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
#CFLAGS += -fno-builtin
#CFLAGS += $(INCLFLAGS)

#---------------- Compiler Options C++ ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
CPPFLAGS = -g$(DEBUG)
CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -O$(OPT)
CPPFLAGS += -funsigned-char
CPPFLAGS += -funsigned-bitfields
CPPFLAGS += -fpack-struct
CPPFLAGS += -fshort-enums
CPPFLAGS += -fno-exceptions
CPPFLAGS += -Wall
CPPFLAGS += -Wundef
#CPPFLAGS += -mshort-calls
#CPPFLAGS += -fno-unit-at-a-time
#CPPFLAGS += -Wstrict-prototypes
#CPPFLAGS += -Wunreachable-code
#CPPFLAGS += -Wsign-compare
#CPPFLAGS += $(INCLFLAGS)
#CPPFLAGS += $(CSTANDARD)

#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -aldghs:   create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
#  -listing-cont-lines: Sets the maximum number of continuation lines of hex 
#       dump that will be displayed for a given single line of source input.
ASFLAGS = $(ADEFS) -Wa,-aldnhs=$(<:%.S=%.lst),-gstabs,--listing-cont-lines=100


#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB =
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)


#MATH_LIB =
MATH_LIB = -lm


# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS =

#---------------- External Memory Options ----------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--section-start,.data=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(APP_NAME).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
#LDFLAGS += -T linker_script.x
#LDFLAGS += -nodefaultlibs #bypasses libgcc.a
#LDFLAGS += -nostartfiles
#LDFLAGS += -lgcc # links libgcc.a in spite of nodefaultlibs
#LDFLAGS += -nostdlib # same as nodefaultlibs and nostartfiles
#LDFLAGS += -
#LDFLAGS += -



#============================================================================


# Define programs and commands.
SHELL = sh
CC = avr-gcc
AS = avr-as
LNK = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
AR = avr-ar rcs
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp
WINSHELL = cmd


# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_FLASH = Creating load file for Flash:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_CPP = Compiling C++:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:


# Define all object files.
#these all work, just examples of different ways to do this
OBJC = $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(CSRC)))
OBJCPP = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(CPPSRC)))
OBJASM = $(patsubst %.S, $(OBJDIR)/%.o, $(notdir $(ASRC)))

OBJ = $(OBJC) $(OBJCPP) $(OBJASM)

# Define list of assembly output files (for no compile assemble only)
ASMS = $(CSRC:%.c=%.s) $(CPPSRC:%.cpp=%.s)

# Define all listing files.
LST = $(OBJ:%.o=%.lst)
 
# pre processed source files
PPS =  $(CSRC:%.c=%.i) $(CPPSRC:%.cpp=%.i) 
PPHINCL = $(PPS:%.i=%.hincl)

# Compiler flags to generate dependency files. https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html
# -M  : output a rule suitable for make describing the dependencies of the main source file
# -MF file : specifies a file to write the dependencies to
# -MD : equivalent to -M -MF file, except that -E is not implied.
# -MMD : -MD except mention only user header files, not system header files
# -MP : This option instructs CPP to add a phony target for each dependency other than the main file, causing each to depend on nothing. These dummy rules work around errors make gives if you remove header files without updating the Makefile to match
GENDEPFLAGS = -MD -MP -MF dep/$(@F).d -MT $(*F).o


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) $(CFLAGS) $(INCLFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS = -mmcu=$(MCU) -x c++ $(CPPFLAGS) $(INCLFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -x assembler-with-cpp $(ASFLAGS)
ALL_LFLAGS = -mmcu=$(MCU) $(LDFLAGS)

HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

# -n : sort symbols numerically by address
NM_FLAGS = -n
#NM_FLAGS += --demangle=gnu
#NM_FLAGS += --recursion-limit
NM_FLAGS += -l
NM_FLAGS += -S
NM_FLAGS += --target=elf32-avr
#NM_FLAGS += -
	
	
#	$(OBJDUMP) -afphxDStr --target=coff-avr --architecture=avr5 --demangle=gnu -w -z c:/winavr-20100110/lib/gcc/avr/4.3.3/avr5/libgcc.a > zlibgcctest.lss
#	$(OBJDUMP) -afphxDStr --target=coff-avr --architecture=avr5 --demangle=gnu -w -z --prefix-addresses c:/winavr-20100110/avr/lib/avr5/crtm328p.o > zcrtm328ptest.lss
	

all: gccversion build

# Change the build target to build a HEX file or a library.
build: elf hex lss sym eep

elf: $(APP_NAME).elf
hex: $(APP_NAME).hex
lss: $(APP_NAME).lss
sym: $(APP_NAME).sym
eep: $(APP_NAME).eep

# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(APP_NAME).hex
ELFSIZE = $(SIZE) --mcu=$(MCU) --format=avr $(APP_NAME).elf

# Display compiler version information.
gccversion : 
	@$(CC) --version
	

# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) $(HEX_FLASH_FLAGS) $< $@

%.eep: %.elf
	@echo
	@echo eeprom object copy into binary file
	$(OBJCOPY) $(HEX_EEPROM_FLAGS) -O $(FORMAT) $< $@ || exit 0

# Create extended listing file from ELF output file.
# -d : dissassemble
# -S : intermix source code with assembly
# -w : wide, format output for more than 80 columns
# -l : include line numbers and filenames in output
# -h : display contents of section headers
# -z : Do not skip blocks of zeroes when disassembling
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -dSlwz $(patsubst %,-I%,$(ALL_DIR)) -C $< > $@

# Create a symbol table from ELF output file.
# -n : sort symbols numerically by address
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) $(NM_FLAGS) $< > $@

# Link: create ELF output file from object files.
.SECONDARY : $(APP_NAME).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_LFLAGS) $(OBJ) --output $@ 
#	$(CC) $(ALL_CFLAGS) $(INCLFLAGS) -v $(OBJ) --output $@ $(ALL_LFLAGS) 

testString:
	@echo $(OBJ)
	@echo $(LST)
	@echo $(ASMS)
	@echo $(PPS)
	@echo $(PPHINCL)

makeObjects: $(OBJ)

makeAssembly: $(ASMS)

makeListing: $(OBJ) $(LST)

preprocess: $(PPS)

assemble: $(OBJASM)

# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c
	@echo
	@echo $@ $<
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $(INCLFLAGS) $< -o $@

# Compile: create object files from C++ source files.
$(OBJDIR)/%.o : %.cpp
	@echo
	@echo $@ $<
	@echo $(MSG_COMPILING_CPP) $<
	$(CC) -c $(ALL_CPPFLAGS) $(INCLFLAGS) $< -o $@ 

# Compile: create assembler files from C source files.
%.s : %.c
	@echo $@ $<
	$(CC) -S $(ALL_CFLAGS) $(INCLFLAGS) $< -o $@

# Compile: create assembler files from C++ source files.
%.s : %.cpp
	@echo $@ $<
	$(CC) -S $(ALL_CPPFLAGS) $(INCLFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : %.S
	@echo
	@echo $@ $<
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $(INCLFLAGS) $< -o $@

# Create preprocessed source for use in sending a bug report.
%.i : %.c
	@echo $@ $<
	$(CC) -E -mmcu=$(MCU) -H -I. $(CFLAGS) $(INCLFLAGS) $< -o $@ 2> $(<:%.c=%.hincl)
%.i : %.cpp
	@echo $@ $<
	$(CC) -E -mmcu=$(MCU) -H -I. -x c++ $(CPPFLAGS) $(INCLFLAGS) $< -o $@ 2> $(<:%.cpp=%.hincl)

# Create extended listing file from ELF output file.
# -d : dissassemble
# -S : intermix source code with assembly
# -w : wide, format output for more than 80 columns
# -l : include line numbers and filenames in output
# -h : display contents of section headers
# -a : archive headers
# -f : file headers
# -p : private headers
# -x : all headers
# -s : full contents of all sections
# -t : symbols
# -z : Do not skip blocks of zeroes when disassembling
# -EL : little endian
%.lst : %.o
	@echo $@ $<
	$(OBJDUMP) -dSlzhapt $(patsubst %,-I%,$(ALL_DIR)) -C $< > $@
	
# Target: clean project.
clean: clean_list

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(APP_NAME).elf
	$(REMOVE) $(APP_NAME).hex
	$(REMOVE) $(APP_NAME).eep
	$(REMOVE) $(APP_NAME).cof
	$(REMOVE) $(APP_NAME).map
	$(REMOVE) $(APP_NAME).sym
	$(REMOVE) $(APP_NAME).lss
	$(REMOVE) $(OBJ)
	$(REMOVE) -r ./*.d
	$(REMOVE) $(LST)
	$(REMOVE) $(ASMS)
	$(REMOVE) $(PPS)
	$(REMOVE) $(PPHINCL)
	$(REMOVEDIR) $(OBJDIR)
	$(REMOVEDIR) dep

# code_coverage : $(CSRC)
#	@echo
#	@echo code coverage test
#	avr-gcov -abcfu $< > $(notdir $(<:%.c=%.cov))


# Create object files directory
$(shell mkdir $(OBJDIR) 2>/dev/null)
$(shell mkdir dep 2>/dev/null)
#$(shell mkdir lst 2>/dev/null)


# Include the dependency files.
#-include $(shell mkdir $(OBJDIR) 2>/dev/null) $(wildcard $(OBJDIR)/*)
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)
#-include $(shell mkdir lst 2>/dev/null) $(wildcard lst/*)
