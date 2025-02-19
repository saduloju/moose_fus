[Drivers]
  [unit]
    type = ModelUnitTest
    model = 'E'
    batch_shape = '(2,3)'
    input_scalar_names = 'forces/T'
    input_scalar_values = '300'
    output_scalar_names = 'E'
    output_scalar_values = '188911.6020499754'
    check_second_derivatives = true
  []
[]

[Models]
  [E]
    type = ScalarLinearInterpolation
    argument = 'forces/T'
    abscissa = 'T'
    ordinate = 'E'
  []
[]

[Tensors]
  [T]
    type = LinspaceScalar
    start = 273.15
    end = 2000
    nstep = 100
    dim = 0
  []
  [E]
    type = LinspaceScalar
    start = 1.9e5
    end = 1.2e5
    nstep = 100
    dim = 0
  []
[]
