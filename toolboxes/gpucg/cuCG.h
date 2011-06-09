#pragma once
#include "gadgetron_export.h"
#include "cuNDArray.h"
#include "cuCGMatrixOperator.h"
#include "cuCGPreconditioner.h"

#include <vector>
#include <iostream>

#include <cublas_v2.h>
#include <boost/smart_ptr.hpp>

template<class REAL, class T> class EXPORTGPUCG cuCG
{
 public:

  enum cuCGOutputModes { OUTPUT_SILENT = 0, OUTPUT_WARNINGS = 1, OUTPUT_VERBOSE = 2, OUTPUT_MAX = 3 };

  cuCG() : iterations_(10), limit_(1e-3), output_mode_(OUTPUT_SILENT) {
    operators_ = boost::shared_ptr< std::vector< boost::shared_ptr< cuCGMatrixOperator<REAL,T> > > >(new std::vector< boost::shared_ptr< cuCGMatrixOperator<REAL,T> > >);
    if (cublasCreate(&cublas_handle_) != CUBLAS_STATUS_SUCCESS) {
      std::cerr << "cuCG unable to create cublas handle" << std::endl;
    }
  }

  virtual ~cuCG() {
    if (cublasDestroy(cublas_handle_) != CUBLAS_STATUS_SUCCESS) {
      std::cerr << "cuCG unable to create cublas handle" << std::endl;
    }
  }
  
  int add_matrix_operator( boost::shared_ptr< cuCGMatrixOperator<REAL,T> > op ) {
    operators_->push_back(op);
    return 0;
  }

  int set_preconditioner( boost::shared_ptr< cuCGPreconditioner<T> > precond ) {
    precond_ = precond;
    return 0;
  }

  void set_output_mode(int output_mode) {
    if (!(output_mode >= OUTPUT_MAX || output_mode < 0)) {
      output_mode_ = output_mode;
    }
  }

  void set_limit(REAL limit) {
    limit_ = limit;
  }

  void set_iterations(unsigned int iterations) {
    iterations_ = iterations;
  }
  
  cublasHandle_t get_cublas_handle() {
    return cublas_handle_; 
  }

  boost::shared_ptr< cuNDArray<T> > solve(cuNDArray<T> *rhs);

  void* operator new (size_t bytes) { return ::new char[bytes]; }
  void operator delete (void *ptr) { delete [] static_cast <char *> (ptr); } 
  void * operator new(size_t s, void * p) { return p; }

protected:
  boost::shared_ptr< std::vector< boost::shared_ptr< cuCGMatrixOperator<REAL,T> > > > operators_;
  boost::shared_ptr< cuCGPreconditioner<T> > precond_;
  cublasHandle_t cublas_handle_;
  unsigned int iterations_;
  REAL limit_;
  int output_mode_;
};