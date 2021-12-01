include ../../../gmsl/gmsl

sortpair = $(foreach a,$(word 1,$1),$(foreach b,$(word 2,$1),$(if $(call lte,$a,$b),$a $b,$b $a)))

pair := 2 1

$(info $(call sortpair,$(pair)))

.PHONY: all
all:
	@true
