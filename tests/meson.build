gtest = subproject('gtest')

test_suites = [
  'parse',
  'string',
]

foreach suite : test_suites
  runner = executable(
    '@0@-runner'.format(suite),
    ['main.cc', 'test_@0@.cc'.format(suite)],
     dependencies : [
       gtest.get_variable('gtest_dep'),
       gtest.get_variable('gmock_dep'),
       range_dep,
     ],
     include_directories : utils_incdir,
  )
  test(
    suite,
    runner,
    protocol : 'gtest',
  )
endforeach
