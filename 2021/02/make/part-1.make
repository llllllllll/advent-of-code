include ../../../make-utils/make-utils.make

input := $(strip $(file < /dev/stdin))

process = $(if $(call eq,$1,forward),$(int-add $3,$2) $4,$(if $(call eq,$1,down),$3 $(int-add $4,$2),$3 $(int-sub $4,$2)))

state := 0 0
step = state := $(call process,$1,$2,$(word 1,$(state)),$(word 2,$(state)))

iterate = $(foreach i,$(range 1,$(words $1),2),$(eval $(call step,$(word $i,$1),$(word $(int-add $i,1),$1))))
$(eval $(call iterate,$(input)))

$(info $(int-mul $(word 1,$(state)),$(word 2,$(state))))
