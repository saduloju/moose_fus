[Tensors]
  [end_time]
    type = FullScalar
    value = 5000
    batch_shape = '(${nbatch})'
  []
  [times]
    type = LinspaceScalar
    start = 0
    end = end_time
    nstep = ${ntime}
  []
  [dxx]
    type = FullScalar
    batch_shape = '(${nbatch})'
    value = 0.0
  []
  [dyy]
    type = FullScalar
    batch_shape = '(${nbatch})'
    value = 0.0001
  []
  [dzz]
    type = FullScalar
    batch_shape = '(${nbatch})'
    value = -0.0001
  []
  [deformation_rate_single]
    type = FillSR2
    values = 'dxx dyy dzz'
  []
  [deformation_rate]
    type = LinspaceSR2
    start = deformation_rate_single
    end = deformation_rate_single
    nstep = ${ntime}
  []

  [w1]
    type = FullScalar
    batch_shape = '(${nbatch})'
    value = 0.0
  []
  [w2]
    type = FullScalar
    batch_shape = '(${nbatch})'
    value = 0.0
  []
  [w3]
    type = FullScalar
    batch_shape = '(${nbatch})'
    value = 0.0
  []
  [vorticity_single]
    type = FillWR2
    values = 'w1 w2 w3'
  []
  [vorticity]
    type = LinspaceWR2
    start = vorticity_single
    end = vorticity_single
    nstep = ${ntime}
  []
  [a]
    type = Scalar
    values = '1.0'
  []
  [sdirs]
    type = FillMillerIndex
    values = '1 1 0'
  []
  [splanes]
    type = FillMillerIndex
    values = '1 1 1'
  []
  [initial_orientation]
    type = Orientation
    quantity = ${nbatch}
    values = '30 60 45'
    normalize = true
  []
[]

[Drivers]
  [driver]
    type = LargeDeformationIncrementalSolidMechanicsDriver
    model = 'model_with_stress'
    times = 'times'
    prescribed_deformation_rate = 'deformation_rate'
    prescribed_vorticity = 'vorticity'
    provide_vorticity = true
    ic_rot_names = 'state/orientation'
    ic_rot_values = 'initial_orientation'
    predictor = 'CP_PREVIOUS_STATE'
    device = ${device}
    cp_elastic_scale = 0.05
  []
[]

[Solvers]
  [newton]
    type = NewtonWithLineSearch
    max_its = 500
    linesearch_cutback = 2.0
    linesearch_stopping_criteria = 1.0e-3
    max_linesearch_iterations = 5
    rel_tol = 1e-4
  []
[]

[Data]
  [crystal_geometry]
    type = CubicCrystal
    lattice_parameter = "a"
    slip_directions = "sdirs"
    slip_planes = "splanes"
  []
[]

[Models]
  [euler_rodrigues]
    type = RotationMatrix
    from = 'state/orientation'
    to = 'state/orientation_matrix'
  []
  [elasticity]
    type = LinearIsotropicElasticity
    youngs_modulus = 1e5
    poisson_ratio = 0.3
    strain = "state/elastic_strain"
    stress = "state/internal/cauchy_stress"
  []
  [resolved_shear]
    type = ResolvedShear
  []
  [elastic_stretch]
    type = ElasticStrainRate
  []
  [plastic_spin]
    type = PlasticVorticity
  []
  [plastic_deformation_rate]
    type = PlasticDeformationRate
  []
  [orientation_rate]
    type = OrientationRate
  []
  [sum_slip_rates]
    type = SumSlipRates
  []
  [slip_rule]
    type = PowerLawSlipRule
    n = 6.0
    gamma0 = 1.0
  []
  [slip_strength]
    type = SingleSlipStrengthMap
    constant_strength = 30.0
  []
  [voce_hardening]
    type = VoceSingleSlipHardeningRule
    initial_slope = 500.0
    saturated_hardening = 10.0
  []
  [integrate_slip_hardening]
    type = ScalarBackwardEulerTimeIntegration
    variable = 'internal/slip_hardening'
  []
  [integrate_elastic_strain]
    type = SR2BackwardEulerTimeIntegration
    variable = 'elastic_strain'
  []
  [integrate_orientation]
    type = WR2ImplicitExponentialTimeIntegration
    variable = 'orientation'
  []

  [implicit_rate]
    type = ComposedModel
    models = 'euler_rodrigues elasticity orientation_rate resolved_shear elastic_stretch plastic_deformation_rate plastic_spin sum_slip_rates slip_rule slip_strength voce_hardening integrate_slip_hardening integrate_elastic_strain integrate_orientation'
    automatic_scaling = true
  []
  [model]
    type = ImplicitUpdate
    implicit_model = 'implicit_rate'
    solver = 'newton'
  []
  [model_with_stress]
    type = ComposedModel
    models = 'model elasticity'
    additional_outputs = 'state/elastic_strain'
  []
[]
