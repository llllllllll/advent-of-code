include ../../../make-utils/make-utils.make

input := $(file < /dev/stdin)

a := $(call list-tail,$(call list-tail,$(call list-tail,$(input))))
b := $(call list-init,$(call list-init,$(call list-init,$(input))))

gt := $(foreach i,$(range $(words $a)),$(int-gt $(word $i,$a),$(word $i,$b)))
$(info $(words $(gt)))
