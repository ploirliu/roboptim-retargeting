#ifndef ROBOPTIM_RETARGETING_DEFORMATION_ENERGY_HH
# define ROBOPTIM_RETARGETING_DEFORMATION_ENERGY_HH
# include <roboptim/core/function.hh>

# include <roboptim/retargeting/interaction-mesh.hh>

namespace roboptim
{
  namespace retargeting
  {
    class LaplacianDeformationEnergy : public roboptim::Function
    {
    public:
      explicit LaplacianDeformationEnergy
      (InteractionMeshShPtr_t mesh) throw ();

      virtual ~LaplacianDeformationEnergy () throw ();
      void impl_compute (result_t& result, const argument_t& x)
	const throw ();
    private:
      InteractionMeshShPtr_t mesh_;
    };
  } // end of namespace retargeting.
} // end of namespace roboptim.

#endif //! ROBOPTIM_RETARGETING_DEFORMATION_ENERGY_HH
