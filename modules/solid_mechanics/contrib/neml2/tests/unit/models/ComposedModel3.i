[Drivers]
  [unit]
    type = ModelUnitTest
    model = 'model'
    batch_shape = '(1)'
    input_scalar_names = 'old_state/foo old_state/bar forces/temperature forces/t old_forces/t'
    input_scalar_values = '0 0 15 1.3 1.1'
    input_symr2_names = 'old_state/baz'
    input_symr2_values = '0'
    output_scalar_names = 'state/sum'
    output_scalar_values = '-3.9902'
    check_values = true
    check_first_derivatives = true
    check_second_derivatives = false
    check_AD_first_derivatives = false
    check_AD_second_derivatives = false
    check_AD_derivatives = false
  []
[]

[Solvers]
  [newton]
    type = Newton
    abs_tol = 1e-10
    rel_tol = 1e-08
    max_its = 100
  []
[]

[Models]
  [rate]
    type = SampleRateModel
  []
  [integrate_foo]
    type = ScalarBackwardEulerTimeIntegration
    variable = 'foo'
  []
  [integrate_bar]
    type = ScalarBackwardEulerTimeIntegration
    variable = 'bar'
  []
  [integrate_baz]
    type = SR2BackwardEulerTimeIntegration
    variable = 'baz'
  []
  [implicit_rate]
    type = ComposedModel
    models = 'rate integrate_foo integrate_bar integrate_baz'
  []
  [implicit_update]
    type = ImplicitUpdate
    implicit_model = 'implicit_rate'
    solver = 'newton'
  []
  [baz_tr]
    type = SR2Invariant
    tensor = 'state/baz'
    invariant = 'state/baz_tr'
    invariant_type = 'I1'
  []
  [sum]
    type = ScalarSumModel
    from_var = 'state/foo state/bar state/baz_tr'
    to_var = 'state/sum'
  []
  [model]
    type = ComposedModel
    models = 'implicit_update baz_tr sum'
  []
[]
