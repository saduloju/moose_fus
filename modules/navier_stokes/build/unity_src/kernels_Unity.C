
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/DistributedForce.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/DistributedPower.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADBoussinesqBodyForce.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADEnergyAdvection.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADEnergyAmbientConvection.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADEnergyMeshAdvection.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADEnergySUPG.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADEnergySource.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADGravityForce.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADHeatConductionTimeDerivative.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADMass.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADMassPSPG.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADMomentumAdvection.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADMomentumCoupledForce.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADMomentumMeshAdvection.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADMomentumPressure.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADMomentumSUPG.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADMomentumTimeDerivative.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADMomentumViscous.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSADSmagorinskyEddyViscosity.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSBase.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSChorinCorrector.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSChorinPredictor.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSChorinPressurePoisson.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSCompressibilityPenalty.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSFEFluidEnergyKernel.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSFEFluidKernelBase.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSFEFluidKernelStabilization.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSFEFluidMassKernel.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSFEFluidMomentumKernel.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSMass.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSMassRZ.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSMomentumBase.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSMomentumLaplaceForm.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSMomentumLaplaceFormRZ.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSMomentumTimeDerivative.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSMomentumTractionForm.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSMomentumTractionFormRZ.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSPressurePoisson.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSProjection.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSSplitMomentum.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSTemperature.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/INSTemperatureTimeDerivative.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/MassConvectiveFlux.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/MomentumConvectiveFlux.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSEnergyInviscidFlux.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSEnergyThermalFlux.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSEnergyViscousFlux.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSGravityForce.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSGravityPower.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSKernel.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSMassInviscidFlux.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSMomentumInviscidFlux.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSMomentumInviscidFluxWithGradP.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSMomentumViscousFlux.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSSUPGBase.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSSUPGEnergy.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSSUPGMass.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSSUPGMomentum.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/NSTemperatureL2.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/PINSFEFluidPressureTimeDerivative.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/PINSFEFluidTemperatureTimeDerivative.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/PINSFEFluidVelocityTimeDerivative.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/PressureGradient.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/TotalEnergyConvectiveFlux.C"
#include"/home/64a/projects/moose2/modules/navier_stokes/src/kernels/VectorMassMatrix.C"
