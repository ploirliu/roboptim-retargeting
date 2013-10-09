#include <boost/make_shared.hpp>

#include <roboptim/retargeting/function/minimum-jerk-trajectory.hh>
#include <roboptim/retargeting/function/zmp/choreonoid.hh>
#include <roboptim/trajectory/vector-interpolation.hh>

#include <cnoid/BodyLoader>

#define BOOST_TEST_MODULE zmp_chorenoid

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include "tests-config.h"

using boost::test_tools::output_test_stream;

using namespace roboptim;
using namespace roboptim::retargeting;

//FIXME: we should embed the copy.
std::string modelFilePath
("/home/moulard/HRP4C-release/HRP4Cg2.yaml");


BOOST_AUTO_TEST_CASE (rnd)
{
  configureLog4cxx ();

  // Loading robot.
  cnoid::BodyLoader loader;
  cnoid::BodyPtr robot = loader.load (modelFilePath);
  if (!robot)
    throw std::runtime_error ("failed to load model");

  typedef ZMPChoreonoid<EigenMatrixDense>::vector_t vector_t;
  ZMPChoreonoid<EigenMatrixDense> zmp (robot);

  vector_t x ((6 + robot->numJoints ()) * 3);
  x.setZero ();

  vector_t res;

  // check that the center of mass and the ZMP are at the same
  // position if velocity and acceleration is null.
  for (int i = 0; i < 100; ++i)
    {
      zmp.q (x) = vector_t::Random (6 + robot->numJoints ());
      res = zmp (x);

      std::cout << "----------" << iendl
		<< "X:" << incindent << iendl
		<< x << decindent << iendl
		<< "ZMP(X): " << incindent << iendl
		<< res << decindent << iendl;
      zmp.printQuantities (std::cerr);
      std::cerr << iendl;

      robot->calcCenterOfMass ();
      BOOST_CHECK_EQUAL (res[0], robot->centerOfMass ()[0]);
      BOOST_CHECK_EQUAL (res[1], robot->centerOfMass ()[1]);
    }

  std::cout << "==========" << iendl;

  // if the velocity/acceleration is not null, check that the ZMP and
  // COM are not the same.
  for (int i = 0; i < 100; ++i)
    {
      x = vector_t::Random (3 * (6 + robot->numJoints ()));
      res = zmp (x);

      std::cout << "----------" << iendl
		<< "X:" << incindent << iendl
		<< x << decindent << iendl
		<< "ZMP(X): " << incindent << iendl
		<< res << decindent << iendl;
      zmp.printQuantities (std::cerr);
      std::cerr << iendl;

      robot->calcCenterOfMass ();
      BOOST_CHECK_GE (std::abs (res[0] - robot->centerOfMass ()[0]), 1e-5);
      BOOST_CHECK_GE (std::abs (res[1] - robot->centerOfMass ()[1]), 1e-5);
    }

  std::cout << "==========" << iendl;

  zmp.q (x).setConstant (0.);
  zmp.dq (x).setConstant (0.);
  zmp.ddq (x).setConstant (0.);
  res = zmp (x);

  std::cout << "X:" << incindent << iendl
	    << x << decindent << iendl
	    << "ZMP(X): " << incindent << iendl
	    << res << decindent << iendl;
  zmp.printQuantities (std::cerr);
  std::cerr << iendl;

  std::cout << "==========" << iendl;

  zmp.q (x).setConstant (0.);
  zmp.dq (x).setConstant (0.);
  zmp.ddq (x).setConstant (1.);
  res = zmp (x);

  std::cout << "X:" << incindent << iendl
	    << x << decindent << iendl
	    << "ZMP(X): " << incindent << iendl
	    << res << decindent << iendl;
  zmp.printQuantities (std::cerr);
  std::cerr << iendl;

  std::cout << "==========" << iendl;

  zmp.q (x).setConstant (0.);
  zmp.dq (x).setConstant (1.);
  zmp.ddq (x).setConstant (0.);
  res = zmp (x);

  std::cout << "X:" << incindent << iendl
	    << x << decindent << iendl
	    << "ZMP(X): " << incindent << iendl
	    << res << decindent << iendl;
  zmp.printQuantities (std::cerr);
  std::cerr << iendl;
}

BOOST_AUTO_TEST_CASE (minimum_jerk)
{
  // Forward typedefs.
  typedef MinimumJerkTrajectory<EigenMatrixDense>::vector_t vector_t;
  typedef MinimumJerkTrajectory<EigenMatrixDense>::interval_t interval_t;
  typedef MinimumJerkTrajectory<EigenMatrixDense>::value_type value_type;
  typedef MinimumJerkTrajectory<EigenMatrixDense>::size_type size_type;

  configureLog4cxx ();

  // Loading robot.
  cnoid::BodyLoader loader;
  cnoid::BodyPtr robot = loader.load (modelFilePath);
  if (!robot)
    throw std::runtime_error ("failed to load model");

  ZMPChoreonoid<EigenMatrixDense> zmp (robot);

  size_type dofId = 6;
  value_type dt = 0.05;
  value_type tmin = 0.;
  value_type tmax = 1.;
  value_type nFrames = (tmax - tmin) / dt;

  // Compute the initial trajectory (whole body)
  vector_t initialTrajectory (nFrames * (6 + robot->numJoints ()));
  initialTrajectory.setZero ();

  vector_t x (4);
  x << 0., 1., 0., 0.;

  boost::shared_ptr<MinimumJerkTrajectory<EigenMatrixDense> >
    minimumJerkTrajectory =
    boost::make_shared<MinimumJerkTrajectory<EigenMatrixDense> >
    ();
  minimumJerkTrajectory->setParameters (x);

  // Build the starting point
  for (std::size_t frameId = 0; frameId < nFrames; ++frameId)
    initialTrajectory[frameId * (6 + robot->numJoints ()) + dofId] =
      (*minimumJerkTrajectory)
      (static_cast<value_type> (frameId) * dt)[0];

  // Build a trajectory over the starting point for display
  MinimumJerkTrajectory<EigenMatrixDense>::discreteInterval_t
    intervalS (tmin, tmax, 0.01);

  boost::shared_ptr<VectorInterpolation >
    trajectory =
    vectorInterpolation
    (initialTrajectory,
     static_cast<size_type> (6 + robot->numJoints ()), dt);

  for (value_type t = tmin; (t + dt) < tmax; t += dt)
    {
      std::cout << "t = " << t << incindent << iendl;

      vector_t state = trajectory->state (t, 2);
      vector_t result = zmp (state);

      std::cout
	<< "State (q, dq, ddq):" << incindent << iendl
	<< state << decindent << iendl
	<< "Result:" << incindent << iendl
	<< result << decindent << iendl
	<< decindent << iendl;

      zmp.printQuantities (std::cerr);
      std::cerr << iendl;
    }
}