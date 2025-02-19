[Drivers]
  [unit]
    type = ModelUnitTest
    model = 'model'
    batch_shape = '(10)'
    input_symr2_names = 'state/internal/M'
    input_symr2_values = 'M'
    output_scalar_names = 'state/internal/fp'
    output_scalar_values = '99.8876'
    check_second_derivatives = true
    derivatives_abs_tol = 1e-06
    check_parameter_derivatives = true
  []
[]

[Tensors]
  [M]
    type = FillSR2
    values = '40 120 80 10 10 90'
  []
[]

[Models]
  [vonmises]
    type = SR2Invariant
    invariant_type = 'VONMISES'
    tensor = 'state/internal/M'
    invariant = 'state/internal/s'
  []
  [yield]
    type = YieldFunction
    yield_stress = 50
  []
  [model]
    type = ComposedModel
    models = 'vonmises yield'
  []
[]
