ifneq ($(MAKEFILE_LIST),)
make-utils-root := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

ifeq (1,$(words $(make-utils-root)))
make-utils-root := $(patsubst %make-utils.make,%,$(make-utils-root))
endif
endif

.PHONY: all
all:
	@true

$(make-utils-root)make_utils.so: $(make-utils-root)make-utils.cc
	$(CXX) -I$(make-utils-root)../include -shared -fPIC -Wall -Wextra \
		-std=c++20 -O3 $^ -g -o $@

-load $(make-utils-root)make_utils.so

# eq :: a -> a -> bool
eq = $(and $(findstring $1,$2),$(findstring $2,$1))

# map :: (a -> b) -> [a] -> [b]
map = $(foreach i,$(range $(words $2)),$(call $1,$(word $i,$2)))

# zipWith :: (a -> b -> c) -> [a] -> [b] -> [c]
zip-with = $(foreach i,$(range $(words $2)),$(call $1,$(word $i,$2),$(word $i,$3)))
