include ../../../gmsl/gmsl

input := $(file < /dev/stdin)

sum-3 = $(call int_decode,$(foreach i,$(call sequence,$1,$(call plus,$1,2)),$(call int_encode,$(word $i,$2))))

sliding-sum-3 = $(foreach i,$(call sequence,1,$(call subtract,$(words $1),2)),$(call sum-3,$i,$1))

sums := $(call sliding-sum-3,$(input))
a := $(call rest,$(sums))
b := $(call chop,$(sums))
result := $(call int_decode,$(foreach i,$(call sequence,1,$(call dec,$(words $a))),$(if $(call lt,$(word $i,$a),$(word $i,$b)),,x)))

$(info $(result))

.PHONY: all
all:
	@true
