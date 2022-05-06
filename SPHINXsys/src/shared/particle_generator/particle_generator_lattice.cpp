/**
 * @file 	particle_generator_lattice.cpp
 * @author	Luhui Han, Chi ZHang and Xiangyu Hu
 */
#include "particle_generator_lattice.h"

#include "complex_shape.h"
#include "base_body.h"
#include "solid_particles.h"
#include "adaptation.h"

namespace SPH
{
	//=================================================================================================//
	BaseParticleGeneratorLattice::BaseParticleGeneratorLattice(SPHBody &sph_body)
		: lattice_spacing_(sph_body.sph_adaptation_->ReferenceSpacing()),
		  domain_bounds_(sph_body.getSPHSystemBounds()),
		  body_shape_(*sph_body.body_shape_) {}
	//=================================================================================================//
	ParticleGeneratorLattice::ParticleGeneratorLattice(SPHBody &sph_body)
		: BaseParticleGeneratorLattice(sph_body), ParticleGenerator(sph_body) {}
	//=================================================================================================//
	ParticleGeneratorMultiResolution::ParticleGeneratorMultiResolution(SPHBody &sph_body)
		: ParticleGeneratorLattice(sph_body),
		  particle_adapation_(DynamicCast<ParticleSpacingByBodyShape>(this, sph_body.sph_adaptation_)),
		  h_ratio_(particle_adapation_->registerSmoothingLengthRatio(base_particles_))
	{
		lattice_spacing_ = particle_adapation_->MinimumSpacing();
	}
	//=================================================================================================//
	void ParticleGeneratorMultiResolution::
		initializePositionAndVolume(const Vecd &position, Real volume)
	{
		Real local_particle_spacing = particle_adapation_->getLocalSpacing(body_shape_, position);
		Real local_particle_volume_ratio = powerN(lattice_spacing_ / local_particle_spacing, Dimensions);
		if ((double)rand() / (RAND_MAX) < local_particle_volume_ratio)
		{
			ParticleGeneratorLattice::initializePositionAndVolume(position, volume / local_particle_volume_ratio);
			initializeSmoothingLengthRatio(local_particle_spacing);
		}
	}
	//=================================================================================================//
	void ParticleGeneratorMultiResolution::initializeSmoothingLengthRatio(Real local_spacing)
	{
		h_ratio_.push_back(particle_adapation_->ReferenceSpacing() / local_spacing);
	}
	//=================================================================================================//
	ShellParticleGeneratorLattice::
		ShellParticleGeneratorLattice(SPHBody &sph_body, Real global_avg_thickness)
		: BaseParticleGeneratorLattice(sph_body), SurfaceParticleGenerator(sph_body),
		  total_volume_(0), global_avg_thickness_(global_avg_thickness),
		  particle_spacing_(sph_body.sph_adaptation_->ReferenceSpacing()),
		  avg_particle_volume_(powerN(particle_spacing_, Dimensions - 1) * global_avg_thickness_),
		  number_of_cells_(0), planned_number_of_particles_(0)
	{
		lattice_spacing_ = 0.5 * global_avg_thickness_;
	}
	//=================================================================================================//
}
