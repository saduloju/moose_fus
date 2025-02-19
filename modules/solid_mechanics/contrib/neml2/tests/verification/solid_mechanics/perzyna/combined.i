[Tensors]
  [times]
    type = VTestTimeSeries
    vtest = 'combined.vtest'
    variable = 'time'
    variable_type = 'SCALAR'
  []
  [strains]
    type = VTestTimeSeries
    vtest = 'combined.vtest'
    variable = 'strain'
    variable_type = 'SYMR2'
  []
  [stresses]
    type = VTestTimeSeries
    vtest = 'combined.vtest'
    variable = 'stress'
    variable_type = 'SYMR2'
  []
[]

[Drivers]
  [driver]
    type = SolidMechanicsDriver
    model = 'model'
    times = 'times'
    prescribed_strains = 'strains'
  []
  [verification]
    type = VTestVerification
    driver = 'driver'
    variables = 'output.state/S'
    references = 'stresses'
    rtol = 1e-5
    atol = 1e-8
  []
[]

[Solvers]
  [newton]
    type = Newton
  []
[]

[Models]
  [isoharden]
    type = LinearIsotropicHardening
    hardening_modulus = 2500
  []
  [kinharden]
    type = LinearKinematicHardening
    hardening_modulus = 1000
  []
  [mandel_stress]
    type = IsotropicMandelStress
  []
  [overstress]
    type = OverStress
  []
  [vonmises]
    type = SR2Invariant
    invariant_type = 'VONMISES'
    tensor = 'state/internal/O'
    invariant = 'state/internal/s'
  []
  [yield]
    type = YieldFunction
    yield_stress = 10
    isotropic_hardening = 'state/internal/k'
  []
  [flow]
    type = ComposedModel
    models = 'overstress vonmises yield'
  []
  [normality]
    type = Normality
    model = 'flow'
    function = 'state/internal/fp'
    from = 'state/internal/M state/internal/k state/internal/X'
    to = 'state/internal/NM state/internal/Nk state/internal/NX'
  []
  [flow_rate]
    type = PerzynaPlasticFlowRate
    reference_stress = 500
    exponent = 5
  []
  [eprate]
    type = AssociativeIsotropicPlasticHardening
  []
  [Kprate]
    type = AssociativeKinematicPlasticHardening
  []
  [Eprate]
    type = AssociativePlasticFlow
  []
  [Erate]
    type = SR2ForceRate
    force = 'E'
  []
  [Eerate]
    type = ElasticStrain
    rate_form = true
  []
  [elasticity]
    type = LinearIsotropicElasticity
    youngs_modulus = 124000
    poisson_ratio = 0.32
    rate_form = true
  []
  [integrate_ep]
    type = ScalarBackwardEulerTimeIntegration
    variable = 'internal/ep'
  []
  [integrate_Kp]
    type = SR2BackwardEulerTimeIntegration
    variable = 'internal/Kp'
  []
  [integrate_stress]
    type = SR2BackwardEulerTimeIntegration
    variable = 'S'
  []
  [implicit_rate]
    type = ComposedModel
    models = 'isoharden kinharden mandel_stress overstress vonmises yield normality flow_rate eprate Eprate Kprate Erate Eerate elasticity integrate_stress integrate_ep integrate_Kp'
  []
  [model]
    type = ImplicitUpdate
    implicit_model = 'implicit_rate'
    solver = 'newton'
  []
[]
