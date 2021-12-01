include ../../../make-utils/make-utils.make

input := $(file < /dev/stdin)

sliding-sum-3 = $(foreach i,$(range $(int-sub $(words $1),2)),$(int-sum $(wordlist $i,$(int-add $i,2),$1)))

sums := $(call sliding-sum-3,$(input))
a := $(list-init $(sums))
b := $(list-tail $(sums))
result := $(words $(foreach i,$(range $(words $a)),$(int-lt $(word $i,$a),$(word $i,$b))))

$(info $(result))
