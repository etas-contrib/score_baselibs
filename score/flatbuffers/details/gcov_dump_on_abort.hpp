/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

// Note: The *_DEATH google test macros fork a child process that dies via SIGABRT.
// abort() bypasses atexit(), so gcov counters are normally never flushed from
// the child.  A SIGABRT handler calls __gcov_dump() to write the child's
// counters before re-raising the signal, ensuring the function-entry line and
// the FLATBUFFERS_ASSERT(false) line are recorded in the coverage data.

// __gcov_dump() writes the current process's gcov counters to the .gcda files
// without resetting them.  Available when compiling with GCC/Clang coverage
// instrumentation (-fprofile-arcs / --coverage).

#ifndef SCORE_LIB_FLATBUFFERS_GCOV_DUMP_ON_ABORT_HPP
#define SCORE_LIB_FLATBUFFERS_GCOV_DUMP_ON_ABORT_HPP

#include <csignal>

#ifdef GCOV
#if defined(__GNUC__) || defined(__clang__)
extern "C" void __gcov_dump();
#define GCOV_DUMP_AVAILABLE 1
#endif
#endif

namespace score
{

namespace flatbuffers
{

namespace unit_test
{

// Installs a SIGABRT handler that flushes gcov counters before the process
// dies. Call this as the first statement inside every *_DEATH block.
inline void InstallGcovDumpOnAbort()
{
#ifdef GCOV_DUMP_AVAILABLE
    std::signal(SIGABRT, [](int) {
        __gcov_dump();
        std::signal(SIGABRT, SIG_DFL);
        std::raise(SIGABRT);
    });
#endif
}

}  // namespace unit_test
}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_LIB_FLATBUFFERS_GCOV_DUMP_ON_ABORT_HPP