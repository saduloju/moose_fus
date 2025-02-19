[Tensors]
  [times]
    type = VTestTimeSeries
    vtest = 'voceiso.vtest'
    variable = 'time'
    variable_type = 'SCALAR'
  []
  [strains]
    type = VTestTimeSeries
    vtest = 'voceiso.vtest'
    variable = 'strain'
    variable_type = 'SYMR2'
  []
  [stresses]
    type = VTestTimeSeries
    vtest = 'voceiso.vtest'
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

[Predictors]
  [simple]
    type = LinearExtrapolationPredictor
  []
[]

[Models]
  [isoharden]
    type = VoceIsotropicHardening
    saturated_hardening = 100
    saturation_rate = 10.0
  []
  [elastic_strain]
    type = ElasticStrain
  []
  [elasticity]
    type = LinearIsotropicElasticity
    youngs_modulus = 120000
    poisson_ratio = 0.3
  []
  [mandel_stress]
    type = IsotropicMandelStress
  []
  [vonmises]
    type = SR2Invariant
    invariant_type = 'VONMISES'
    tensor = 'state/internal/M'
    invariant = 'state/internal/s'
  []
  [yield]
    type = YieldFunction
    yield_stress = 100
    isotropic_hardening = 'state/internal/k'
  []
  [flow]
    type = ComposedModel
    models = 'vonmises yield'
  []
  [normality]
    type = Normality
    model = 'flow'
    function = 'state/internal/fp'
    from = 'state/internal/M state/internal/k'
    to = 'state/internal/NM state/internal/Nk'
  []
  [eprate]
    type = AssociativeIsotropicPlasticHardening
  []
  [Eprate]
    type = AssociativePlasticFlow
  []
  [integrate_ep]
    type = ScalarBackwardEulerTimeIntegration
    variable = 'internal/ep'
  []
  [integrate_Ep]
    type = SR2BackwardEulerTimeIntegration
    variable = 'internal/Ep'
  []
  [consistency]
    type = RateIndependentPlasticFlowConstraint
  []
  [surface]
    type = ComposedModel
    models = "isoharden elastic_strain elasticity
              mandel_stress vonmises
              yield normality eprate Eprate
              consistency integrate_ep integrate_Ep"
  []
  [return_map]
    type = ImplicitUpdate
    implicit_model = 'surface'
    solver = 'newton'
  []
  [model]
    type = ComposedModel
    models = 'return_map elastic_strain elasticity'
    additional_outputs = 'state/internal/Ep state/internal/ep'
  []
[]
