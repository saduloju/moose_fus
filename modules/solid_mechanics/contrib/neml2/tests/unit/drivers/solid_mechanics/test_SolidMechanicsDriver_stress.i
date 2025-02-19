[Tensors]
  [end_time]
    type = LogspaceScalar
    start = -1
    end = 5
    nstep = 20
  []
  [times]
    type = LinspaceScalar
    start = 0
    end = end_time
    nstep = 100
  []
  [sxx]
    type = FullScalar
    batch_shape = '(20)'
    value = 0.1
  []
  [syy]
    type = FullScalar
    batch_shape = '(20)'
    value = -0.05
  []
  [szz]
    type = FullScalar
    batch_shape = '(20)'
    value = -0.05
  []
  [max_stress]
    type = FillSR2
    values = 'sxx syy szz'
  []
  [stresses]
    type = LinspaceSR2
    start = 0
    end = max_stress
    nstep = 100
  []
[]

[Drivers]
  [driver]
    type = SolidMechanicsDriver
    model = 'model'
    times = 'times'
    prescribed_stresses = 'stresses'
    control = 'STRESS'
    save_as = 'unit/drivers/solid_mechanics/test_SolidMechanicsDriver_stress.pt'
  []
[]

[Models]
  [force_rate]
    type = SR2ForceRate
    force = 'S'
  []
  [strain_rate]
    type = LinearIsotropicElasticity
    youngs_modulus = 1e5
    poisson_ratio = 0.3
    rate_form = true
    compliance = true
    stress = 'forces/S'
    strain = 'state/E'
  []
  [integrate]
    type = SR2ForwardEulerTimeIntegration
    variable = 'E'
  []
  [model]
    type = ComposedModel
    models = 'force_rate strain_rate integrate'
  []
[]
