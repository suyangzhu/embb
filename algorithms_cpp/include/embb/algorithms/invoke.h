/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EMBB_ALGORITHMS_INVOKE_H_
#define EMBB_ALGORITHMS_INVOKE_H_

#include <embb/base/function.h>
#include <embb/algorithms/execution_policy.h>

namespace embb {
namespace algorithms {

/**
 * \defgroup CPP_ALGORITHMS_INVOKE Invoke
 * Parallel invocation of functions.
 * \ingroup CPP_ALGORITHMS
 */


/**
 * Function type used by Invoke.
 * \ingroup CPP_ALGORITHMS_INVOKE
 */
typedef embb::base::Function<void> InvokeFunctionType;

#ifdef DOXYGEN

/**
 * Spawns one to ten function objects at once and runs them in parallel.
 *
 * Blocks until all of them are done.
 *
 * \ingroup CPP_ALGORITHMS_INVOKE
 */
template<typename Function1, ...>
void Invoke(
  Function1 func1,
  /**< [in] First function to invoke */
  ...);

/**
* Spawns one to ten function objects at once and runs them in parallel using the
* given ExecutionPolicy.
*
* Blocks until all of them are done.
*
* \ingroup CPP_ALGORITHMS_INVOKE
*/
template<typename Function1, ...>
void Invoke(
  Function1 func1,
  /**< [in] Function to invoke */
  ...,
  const ExecutionPolicy & policy /**< [in] ExecutionPolicy to use */
  );

#else // DOXYGEN

namespace internal {

/**
 * Spawns an MTAPI task on construction and waits for it on destruction.
 */
template<typename Function>
class TaskWrapper {
 public:
  /**
   * Wraps the function into an mtapi::Action and spawns an mtapi::Task.
   */
  explicit TaskWrapper(
    Function function,
    const ExecutionPolicy& policy)
      : function_(function), task_() {
    mtapi::Action action(embb::base::MakeFunction(*this, &TaskWrapper::Run),
                         policy.GetAffinity());
    task_ = mtapi::Node::GetInstance().Spawn(action, policy.GetPriority());
  }

  /**
   * Waits until the task has finished execution.
   */
  ~TaskWrapper() {
    task_.Wait(MTAPI_INFINITE);
  }

 private:
  Function function_;
  mtapi::Task task_;

  void Run(embb::mtapi::TaskContext&) {
    function_();
  }
};
} // namespace internal

template<typename Function1>
void Invoke(
  Function1 func1) {
  Invoke(func1, ExecutionPolicy());
}

template<typename Function1, typename Function2>
void Invoke(
  Function1 func1,
  Function2 func2) {
  Invoke(func1, func2, ExecutionPolicy());
}
template<typename Function1, typename Function2, typename Function3>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3) {
  Invoke(func1, func2, func3, ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4) {
  Invoke(func1, func2, func3, func4, ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5) {
  Invoke(func1, func2, func3, func4, func5, ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6) {
  Invoke(func1, func2, func3, func4, func5, func6, ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7) {
  Invoke(func1, func2, func3, func4, func5, func6, func7, ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7, typename Function8>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8) {
  Invoke(func1, func2, func3, func4, func5, func6, func7, func8,
         ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7, typename Function8, typename Function9>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  Function9 func9) {
  Invoke(func1, func2, func3, func4, func5, func6, func7, func8, func9,
         ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7, typename Function8, typename Function9,
         typename Function10>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  Function9 func9,
  Function10 func10) {
  Invoke(func1, func2, func3, func4, func5, func6, func7, func8, func9, func10,
         ExecutionPolicy());
}

template<typename Function1>
void Invoke(
  Function1 func1,
  const ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
}

template<typename Function1, typename Function2>
void Invoke(
  Function1 func1,
  Function2 func2,
  const ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
}

template<typename Function1, typename Function2, typename Function3>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  const ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  const ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  const ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  const ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  const ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
  internal::TaskWrapper<Function7> wrap7(func7, policy);
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7, typename Function8>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  const ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
  internal::TaskWrapper<Function7> wrap7(func7, policy);
  internal::TaskWrapper<Function8> wrap8(func8, policy);
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7, typename Function8, typename Function9>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  Function9 func9,
  const ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
  internal::TaskWrapper<Function7> wrap7(func7, policy);
  internal::TaskWrapper<Function8> wrap8(func8, policy);
  internal::TaskWrapper<Function9> wrap9(func9, policy);
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7, typename Function8, typename Function9,
         typename Function10>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  Function9 func9,
  Function10 func10,
  const ExecutionPolicy& policy)  {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
  internal::TaskWrapper<Function7> wrap7(func7, policy);
  internal::TaskWrapper<Function8> wrap8(func8, policy);
  internal::TaskWrapper<Function9> wrap9(func9, policy);
  internal::TaskWrapper<Function10> wrap10(func10, policy);
}

#endif // else DOXYGEN

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INVOKE_H_
