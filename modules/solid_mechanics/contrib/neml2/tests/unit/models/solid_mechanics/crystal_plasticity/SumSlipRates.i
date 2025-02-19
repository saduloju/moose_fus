[Drivers]
  [unit]
    type = ModelUnitTest
    model = 'model'
    batch_shape = '(10,3)'
    input_batch_tensor_names = 'state/internal/slip_rates'
    input_batch_tensor_values = 'rates'
    output_scalar_names = 'state/internal/sum_slip_rates'
    output_scalar_values = 'sum'
  []
[]

[Tensors]
  [a]
    type = Scalar
    values = '1.2'
  []
  [sdirs]
    type = FillMillerIndex
    values = '1 1 0'
  []
  [splanes]
    type = FillMillerIndex
    values = '1 1 1'
  []
  [sum]
    type = Scalar
    values = '1.91'
  []
  [rates]
    type = BatchTensor
    values = '-0.2 -0.15 -0.1 -0.05 0.01 0.05 0.1 0.15 0.2 0.25 0.30 0.35'
    base_shape = '(12)'
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
  [model]
    type = SumSlipRates
  []
[]
