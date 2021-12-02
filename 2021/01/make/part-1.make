include ../../../make-utils/make-utils.make

input := $(file < /dev/stdin)

a := $(list-tail $(input))
b := $(list-init $(input))

gt := $(foreach i,$(range $(words $a)),$(int-gt $(word $i,$a),$(word $i,$b)))
$(info $(words $(gt)))
