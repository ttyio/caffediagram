GXX    ?= g++

CXXFLAGS += -Wall 
CXXFLAGS += -fvisibility=hidden

ifeq ($(dbg), 1)
    subdir    = debug
    CXXFLAGS += -g -O0 -DDEBUG -D_DEBUG
else
    subdir    = release
    CXXFLAGS += -O2 -DNDEBUG
endif

os_arch = $(shell getconf LONG_BIT)
os_name    = $(shell uname -s 2>/dev/null)$(os_arch)
ARCHFLAGS = -Werror
ifeq ($(x64), 1)
    os_arch    = 64
    ARCHFLAGS += -m64
endif
ifeq ($(x32), 1)
    os_arch    = 32
    ARCHFLAGS += -m32
endif

target     = caffeDiagram
target_dir = $(os_name)/$(subdir)
CXXFLAGS += $(ARCHFLAGS)

LDFLAGS   += -lstdc++ 

INCLUDES +=     \
    -I.         \

sources =               \
    bmpwritter.cpp      \
    simplefont.cpp      \
    main.cpp            \


objects = $(patsubst %.cpp, $(target_dir)/%.o, $(notdir $(sources)))

.PHONY: default
default: $(target_dir)/$(target)

$(target_dir)/%.o: ./%.cpp
	mkdir -p $(dir $@)
	$(GXX) -c $(INCLUDES) $(CXXFLAGS) $< -o $@

$(target_dir)/$(target):$(objects)
	mkdir -p $(dir $@)
	$(GXX) $(objects) $(ARCHFLAGS) $(LDFLAGS) -Wall -o $@ 

clean:
	rm -f  $(objects) $(target_dir)/$(target)
	rm -rf $(target_dir)
