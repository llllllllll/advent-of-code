include ../../../make-utils/make-utils.make

input := $(file < /dev/stdin)

diff = $(foreach i,$(range $(int-dec $(words $(input)))),$(int-sub $(word $(int-add $i,1),$1),$(word $i,$1)))
increases = $(filter-out -%,$1)

$(info $(words $(call increases,$(call diff,$(input)))))
