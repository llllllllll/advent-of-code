include ../../../make-utils/make-utils.make

input := $(strip $(file < /dev/stdin))

process = $(if $(call eq,$1,forward),$(int-add $3,$2) $(int-add $(int-mul $5,$2),$4) $5,$(if $(call eq,$1,down),$3 $4 $(int-add $5,$2),$3 $4 $(int-sub $5,$2)))

state := 0 0 0
step = state := $(call process,$1,$2,$(word 1,$(state)),$(word 2,$(state)),$(word 3,$(state)))

iterate = $(foreach i,$(range 1,$(words $1),2),$(eval $(call step,$(word $i,$1),$(word $(int-add $i,1),$1))))
$(eval $(call iterate,$(input)))

$(info $(int-mul $(word 1,$(state)),$(word 2,$(state))))
