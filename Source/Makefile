## Makefile for BlackBerry PlayBook Template Project
## Date: 03|04|2012

# Check for environment variables that should be set
ifeq ($(strip $(QNX_TARGET)),)
$(error Please run: $$ source /path/to/bbndk/bbndk-env.sh to set up your environment variables )
endif

ifeq ($(strip $(QNX_HOST)),)
$(error Please run: $$ source /path/to/bbndk/bbndk-env.sh to set up your environment variables )
endif

ifeq ($(strip $(QNX_CONFIGURATION)),)
$(error Please run: $$ source /path/to/bbndk/bbndk-env.sh to set up your environment variables )
endif

# These need to be set up by the user manually
ifeq ($(strip $(PLAYBOOK_TARGET)),)
$(error "You need to define your PlayBook's IP Address.  Use: export PLAYBOOK_TARGET=<IP address>")
endif

ifeq ($(strip $(PLAYBOOK_PASSWORD)),)
$(error "You need to define your PlayBook's password.  Use: read -esp 'Password: ' PLAYBOOK_PASSWORD  Then enter your password and type: export PLAYBOOK_PASSWORD")
endif

ifeq ($(strip $(PLAYBOOK_PIN)),)
$(error "You need to define your PlayBook's PIN.  Use: export PLAYBOOK_PIN=<PIN>")
endif

ifeq ($(strip $(KEYSTORE_PASSWORD)),)
$(error "You need to define your Keystore password.  Use: read -esp 'Password: ' KEYSTORE_PASSWORD  Then enter your password and type: export KEYSTORE_PASSWORD")
endif

## Replace Template with your executable's name
TARGET = Template
TARGETDIR	= ../Bin
OBJSDIR		= ../Obj/$(BUILD_PLATFORM)/$(BUILD)
SOURCEDIR	= Source
CONNECTIONPID = `cat connpid`

# Define the C and C++ compilers
CC = qcc
CXX = QCC

# Either the NTO x86 or ARM compiler is used based on passing x86 or arm to
# make.
# If no processor is defined, ARM is assumed
ifndef PLATFORM
PLATFORM=arm
endif

ifndef SSHKEY
SSHKEY=~/.rim/bbt_id_rsa.pub
endif

ifdef PLATFORM
ifeq ($(PLATFORM),x86)
CFLAGS = -Vgcc_ntox86
INCPATH =
BUILD_PLATFORM = PlayBook_x86
CONFIGURATION = Simulator-
TARGET := $(TARGET)Simulator
else
CFLAGS = -Vgcc_ntoarmv7le
INCPATH = -I$(QNX_TARGET)../target-override/usr/include -I$(QNX_TARGET)/usr/include
BUILD_PLATFORM = PlayBook_ARM
CONFIGURATION = Device-
TARGET := $(TARGET)
endif
endif

# Prevent commands from being displayed (mainly to prevent the PlayBook's
# password from being displayed in the terminal)
#.SILENT:
# Set up the compiler's flags and alternate libraries for debugging
debug: TARGET := $(TARGET)D
debug: CFLAGS += -g -ggdb -Wall
debug: LFLAGS = -Wl,--no-keep-memory,-z,relro,-z,now
debug: BUILD := Debug
debug: CONFIGURATION := $(addsuffix Debug, $(CONFIGURATION))
ifdef GDB
debug: $(TARGET) gdb
else
debug: $(TARGET)
endif

# Set up the compiler's flags and libraries for release
release: TARGET := $(TARGET)
release: BUILD := Release
release: CONFIGURATION := $(addsuffix Release, $(CONFIGURATION))
ifdef GDB
release: $(TARGET) gdb
else
release: $(TARGET)
endif

# Set up the compiler's flags and alternate libraries for profiling
profile: TARGET := $(TARGET)P
profile: BUILD := Profile
profile: CONFIGURATION := $(addsuffix Profile, $(CONFIGURATION))
ifdef GDB
profile: $(TARGET) gdb
else
profile: $(TARGET)
endif

debugtoken:
	@# If no debug token is found, create one
	@if [ ! -e $(TARGETDIR)/$(TARGET)_tok.bar ]; then\
		blackberry-debugtokenrequest -storepass $$KEYSTORE_PASSWORD -devicepin $(PLAYBOOK_PIN) $(TARGETDIR)/$(TARGET)_tok.bar;\
	fi

# Deploy will only install the debug token and application to the PlayBook
deploy: debugtoken
	@blackberry-deploy -package $(TARGETDIR)/$(TARGET).bar -installDebugToken $(TARGETDIR)/$(TARGET)_tok.bar $(PLAYBOOK_TARGET) -password $(PLAYBOOK_PASSWORD)
	@export Installed=$$(echo $$(blackberry-nativepackager -installApp -device $(PLAYBOOK_TARGET) -password $(PLAYBOOK_PASSWORD) $(TARGETDIR)/$(TARGET).bar | grep result:: | sed 's/result:://'))

# Launch will cause the PlayBook to just launch the application
# Launch starter is required because the second command will run in parallel
# (I think) so the file won't be available
launch_starter: deploy
	@# If the program is running, don't attempt to launch, otherwise launch the program
	@# If the program isn't running, launch it and save the PID
	@export Launched=$$(echo $$(blackberry-deploy -isAppRunning -package $(TARGETDIR)/$(TARGET).bar $(PLAYBOOK_TARGET) -password $(PLAYBOOK_PASSWORD) | grep 'result::' | sed 's/result:://'));\
	if [ $$Launched = true ]; then \
		echo Already running.  Attach your debugger to process $$(cat pid); \
	else \
		printf "\nLaunching $(TARGET)...\n\n";\
		rm -rf pid;\
		blackberry-deploy -debugNative -launchApp -device $(PLAYBOOK_TARGET) -password $(PLAYBOOK_PASSWORD) $(TARGETDIR)/$(TARGET).bar | grep result:: | sed 's/result:://' > pid;\
	fi

connect:
	@# Open the connection to launch on if the connpid file isn't found
	@# (which would suggest it is already running)
	@if [ -e connpid ]; then\
		echo Connect running;\
	else\
		echo Starting connection...;\
		(java -Xmx512M -jar $(QNX_HOST)/usr/lib/Connect.jar $(PLAYBOOK_TARGET) -password $(PLAYBOOK_PASSWORD) -sshPublicKey $(SSHKEY) > /dev/null &);\
		export CONN=$$$$;export CONN=`expr $$CONN + 2`;echo $$CONN > connpid;\
	fi

launch: launch_starter connect
	@export Launched=$$(echo $$(blackberry-deploy -isAppRunning -package $(TARGETDIR)/$(TARGET).bar $(PLAYBOOK_TARGET) -password $(PLAYBOOK_PASSWORD) | grep 'result::' | sed 's/result:://'));\
	if [ $$Launched ]; then \
		printf "Process $$(cat pid) started\n\n";\
	fi

# GDB invokes the install and launch commands to ensure the latest build is
# being debugged
gdb: launch
	@echo target qnx $(PLAYBOOK_TARGET):8000 > gdbrun
	@echo sym $(TARGETDIR)/$(TARGET) >> gdbrun
	@# Use printf to avoid putting '\n' at the end of the line
	@printf "attach " >> gdbrun
	@cat pid >> gdbrun
	@ntoarm-gdb -command=gdbrun
	@# Finished with the PID
	@rm -f pid
	@export StoredConn=$(CONNECTIONPID);\
	export RunningConn=`ps o pid | grep $$StoredConn`;\
	if [ "$$RunningConn" ]; then\
		printf "\nClosing PlayBook [$(PLAYBOOK_TARGET)] Connection...\n";\
		kill `cat connpid`;\
	fi
	@rm connpid

ifneq ($(OBJSDIR), $(notdir $(CURDIR)))

CPPFILES	:= $(foreach dir,$(SOURCEDIR),$(notdir $(wildcard $(dir)/*.cpp)))
OBJS		= $(CPPFILES:.cpp=.o)

$(TARGETDIR):
	-mkdir -p $(TARGETDIR)

$(TARGET): $(OBJS) $(TARGETDIR) debugtoken
	cd $(OBJSDIR) && $(CXX) $(CFLAGS) -o ../../$(TARGETDIR)/$(TARGET) $(OBJS)
	@# Make the BAR file
	@printf "\n\n"
	blackberry-nativepackager -configuration $(CONFIGURATION) -package $(TARGETDIR)/$(TARGET).bar -list -target BAR-DEBUG -devMode -debugToken $(TARGETDIR)/$(TARGET)_tok.bar BAR.xml -e $(TARGETDIR)/$(TARGET) $(TARGET) -e ../Media/Icons/BBPlayBookIcon.png icon.png

%.o: $(SOURCEDIR)/%.cpp
	@mkdir -p $(OBJSDIR)
	$(CXX) $(CFLAGS) -c -IHeaders $< -o $(OBJSDIR)/$@

endif

