ifneq ($(MAKEFILE_LIST),)
make-utils-root := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

# If there are any spaces in the path in make-utils-root then give up

ifeq (1,$(words $(make-utils-root)))
make-utils-root := $(patsubst %make-utils.make,%,$(make-utils-root))
endif
endif

.PHONY: all
all:
	@true

-load $(make-utils-root)make_utils.so

$(make-utils-root)make_utils.so: $(make-utils-root)make-utils.cc
	$(CXX) -I$(make-utils-root)../include -shared -fPIC -Wall -Wextra \
		-std=c++20 -O3 $^ -g -o $@
