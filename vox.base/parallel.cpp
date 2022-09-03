// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.base/parallel.h"

#include <thread>

#if defined(JET_TASKING_TBB)
#include <tbb/task_arena.h>
#include <tbb/task_scheduler_init.h>
#elif defined(JET_TASKING_OPENMP)
#include <omp.h>
#endif

static unsigned int sMaxNumberOfThreads = std::thread::hardware_concurrency();

namespace vox {

void setMaxNumberOfThreads(unsigned int numThreads) {
#if defined(JET_TASKING_TBB)
    static std::unique_ptr<tbb::task_scheduler_init> tbbInit;
    if (!tbbInit.get())
        tbbInit.reset(new tbb::task_scheduler_init(numThreads));
    else {
        tbbInit->terminate();
        tbbInit->initialize(numThreads);
    }
#elif defined(JET_TASKING_OPENMP)
    omp_set_num_threads(numThreads);
#endif
    sMaxNumberOfThreads = std::max(numThreads, 1u);
}

unsigned int maxNumberOfThreads() { return sMaxNumberOfThreads; }

}  // namespace vox
