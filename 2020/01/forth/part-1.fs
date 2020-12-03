#! /usr/bin/gforth

4096 constant buf-size
create buf buf-size allot

: parse-lines ( -- n1 n2 n3 ... nn num-ints )
  0 ( num-lines)
  begin
    buf buf-size stdin read-line throw
  while
    buf swap snumber? 0>= throw
    swap 1+
  repeat
  drop ;

: find-sum-matching-2020 ( n1 n2 n3 ... nn num-ints -- prod )
  dup 1- 0  do
    dup
    I 2 + pick
    0
    rot I 1+ do
      over
      I 3 + pick
      2dup + 2020 = if
        rot
        drop -1
        leave
      then
      2drop
    loop
    if
      leave
    then
    drop
  loop ;

parse-lines find-sum-matching-2020 * . cr
bye
