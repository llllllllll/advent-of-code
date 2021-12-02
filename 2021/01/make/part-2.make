include ../../../make-utils/make-utils.make

input := $(file < /dev/stdin)

a := $(list-tail $(list-tail $(list-tail $(input))))
b := $(list-init $(list-init $(list-init $(input))))

gt := $(foreach i,$(range $(words $a)),$(int-gt $(word $i,$a),$(word $i,$b)))
$(info $(words $(gt)))
