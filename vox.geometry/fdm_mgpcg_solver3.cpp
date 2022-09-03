// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/fdm_mgpcg_solver3.h"

#include <utility>

#include "vox.base/logging.h"
#include "vox.geometry/cg.h"
#include "vox.geometry/mg.h"

using namespace vox;

void FdmMgpcgSolver3::Preconditioner::build(FdmMgLinearSystem3 *system_, MgParameters<FdmBlas3> mgParams_) {
    system = system_;
    mgParams = std::move(mgParams_);
}

void FdmMgpcgSolver3::Preconditioner::solve(const FdmVector3 &b, FdmVector3 *x) const {
    // Copy dimension
    FdmMgVector3 mgX = system->x;
    FdmMgVector3 mgB = system->x;
    FdmMgVector3 mgBuffer = system->x;

    // Copy input to the top
    mgX.levels.front().set(*x);
    mgB.levels.front().set(b);

    mgVCycle(system->A, mgParams, &mgX, &mgB, &mgBuffer);

    // Copy result to the output
    x->set(mgX.levels.front());
}

//

FdmMgpcgSolver3::FdmMgpcgSolver3(unsigned int numberOfCgIter,
                                 size_t maxNumberOfLevels,
                                 unsigned int numberOfRestrictionIter,
                                 unsigned int numberOfCorrectionIter,
                                 unsigned int numberOfCoarsestIter,
                                 unsigned int numberOfFinalIter,
                                 double maxTolerance,
                                 double sorFactor,
                                 bool useRedBlackOrdering)
    : FdmMgSolver3(maxNumberOfLevels,
                   numberOfRestrictionIter,
                   numberOfCorrectionIter,
                   numberOfCoarsestIter,
                   numberOfFinalIter,
                   maxTolerance,
                   sorFactor,
                   useRedBlackOrdering),
      _maxNumberOfIterations(numberOfCgIter),
      _lastNumberOfIterations(0),
      _tolerance(maxTolerance),
      _lastResidualNorm(kMaxD) {}

bool FdmMgpcgSolver3::solve(FdmMgLinearSystem3 *system) {
    Size3 size = system->A.levels.front().size();
    _r.resize(size);
    _d.resize(size);
    _q.resize(size);
    _s.resize(size);

    system->x.levels.front().set(0.0);
    _r.set(0.0);
    _d.set(0.0);
    _q.set(0.0);
    _s.set(0.0);

    _precond.build(system, params());

    pcg<FdmBlas3, Preconditioner>(system->A.levels.front(), system->b.levels.front(), _maxNumberOfIterations,
                                  _tolerance, &_precond, &system->x.levels.front(), &_r, &_d, &_q, &_s,
                                  &_lastNumberOfIterations, &_lastResidualNorm);

    LOGI("Residual after solving MGPCG: {} Number of MGPCG iterations: {}", _lastResidualNorm, _lastNumberOfIterations)

    return _lastResidualNorm <= _tolerance || _lastNumberOfIterations < _maxNumberOfIterations;
}

unsigned int FdmMgpcgSolver3::maxNumberOfIterations() const { return _maxNumberOfIterations; }

unsigned int FdmMgpcgSolver3::lastNumberOfIterations() const { return _lastNumberOfIterations; }

double FdmMgpcgSolver3::tolerance() const { return _tolerance; }

double FdmMgpcgSolver3::lastResidual() const { return _lastResidualNorm; }
