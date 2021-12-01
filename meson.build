project('aoc', 'cpp', version : '2021', default_options : ['cpp_std=c++2a'])
fs = import('fs')

years = ['2019', '2020', '2021']

utils_incdir = include_directories('include')
thread_dep = dependency('threads')
ncurses_dep = dependency('ncurses')

days = [
  '01',
  '02',
  '03',
  '04',
  '05',
  '06',
  '07',
  '08',
  '09',
  '10',
  '11',
  '12',
  '13',
  '14',
  '15',
  '16',
  '17',
  '18',
  '19',
  '20',
  '21',
  '22',
  '23',
  '24',
  '25',
]

parts = [1, 2]

foreach year : years
  foreach day : days
    input = year / day / 'input'
    foreach part : parts
      name = '@0@-@1@-@2@'.format(year, day, part)
      path = year / day / 'c++' / 'part-' + part.to_string() + '.cc'
      if fs.exists(path)
        exe = executable(
          name,
          [path],
          include_directories : utils_incdir,
          dependencies : [thread_dep, ncurses_dep],
        )
      endif
    endforeach
  endforeach
endforeach

subdir('tests')
