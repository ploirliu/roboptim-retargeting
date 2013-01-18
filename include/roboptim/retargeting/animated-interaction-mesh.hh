#ifndef ROBOPTIM_RETARGETING_ANIMATED_INTERACTION_MESH
# define ROBOPTIM_RETARGETING_ANIMATED_INTERACTION_MESH
# include <vector>

# include <log4cxx/logger.h>

# include <boost/shared_ptr.hpp>

# include <roboptim/retargeting/interaction-mesh.hh>

namespace roboptim
{
  namespace retargeting
  {
    class AnimatedInteractionMesh;
    typedef boost::shared_ptr<AnimatedInteractionMesh>
    AnimatedInteractionMeshShPtr_t;

    /// \brief Stores a set of interaction mesh representing a motion.
    class AnimatedInteractionMesh
    {
    public:
      explicit AnimatedInteractionMesh ();
      ~AnimatedInteractionMesh ();

      static AnimatedInteractionMeshShPtr_t loadAnimatedMesh
      (const std::string& trajectoryFile,
       const std::string& characterFile);


      const std::vector<InteractionMeshShPtr_t>& meshes () const
      {
	return meshes_;
      }

      std::vector<InteractionMeshShPtr_t>& meshes ()
      {
	return meshes_;
      }

      const double& framerate () const
      {
	return framerate_;
      }

      std::size_t
      countFrames () const
      {
	return meshes ().size ();
      }

      std::size_t
      optimizationVectorSize () const
      {
	if (!countFrames ())
	  return 0;
	return 3
	  * boost::num_vertices (meshes ()[0]->graph ())
	  * countFrames ();
      }

      Eigen::Matrix<double, Eigen::Dynamic, 1>
      makeOptimizationVector () const
      {
	return Eigen::Matrix<double, Eigen::Dynamic, 1>
	  (optimizationVectorSize ());

      }
    private:
      static log4cxx::LoggerPtr logger;
      double framerate_;
      std::vector<InteractionMeshShPtr_t> meshes_;
    };
  } // end of namespace retargeting.
} // end of namespace roboptim.

#endif //! ROBOPTIM_RETARGETING_ANIMATED_INTERACTION_MESH