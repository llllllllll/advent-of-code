include ../../../gmsl/gmsl

input := $(file < /dev/stdin)

diff = $(foreach i,$(call sequence,1,$(call dec,$(words $(input)))),$(if $(call lt,$(word $(call plus,$i,1),$1),$(word $i,$1)),0,$(call subtract,$(word $(call plus,$i,1),$1),$(word $i,$1))))
increases = $(filter-out 0,$1)

$(info $(words $(call increases,$(call diff,$(input)))))

.PHONY: all
all:
	@true
