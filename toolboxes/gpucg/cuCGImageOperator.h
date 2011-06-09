#pragma once
#include "gadgetron_export.h"
#include "cuCGMatrixOperator.h"
#include "vector_td_utilities.h"
#include "ndarray_vector_td_utilities.h"

#include <cublas_v2.h>
#include <cmath>
#include <boost/smart_ptr.hpp>

template <class REAL, class T> 
class cuCGImageOperator : public cuCGMatrixOperator<REAL,T>
{
 public:

  cuCGImageOperator() {}
  virtual ~cuCGImageOperator() {}

  inline int set_encoding_operator( boost::shared_ptr< cuCGMatrixOperator<REAL,T> > encoding_operator ){
    encoding_operator_ = encoding_operator;
    return 0;
  }
  
  inline int compute( cuNDArray<T> *encoded_image, std::vector<unsigned int> *decoded_image_dimensions, cublasHandle_t handle ) 
  { 
    if( !encoding_operator_.get() ){
      std::cout << std::endl << "cuCGImageOperator::compute: encoding operator not set" << std::endl;
      return -1;
    }
    
    // Reconstruct regularization image
    cuNDArray<T> tmp; tmp.create(decoded_image_dimensions);
    encoding_operator_->mult_MH( encoded_image, &tmp );
    
    // Normalize to an average energy of "one intensity unit per image element"
    REAL sum = cuNDA_asum<REAL,T>( &tmp, handle );
    REAL scale = ( (REAL) tmp.get_number_of_elements()/sum );
    cuNDA_scal<T>( scale*get_one<T>(), &tmp, handle );

    // Square and reciprocalize image
    image_ = cuNDA_norm_squared<REAL,T>(&tmp);     
    cuNDA_reciprocal<REAL>(image_.get()); 
    
    return 0;
  }
    
  virtual int mult_M(cuNDArray<T>* in, cuNDArray<T>* out, bool accumulate = false){
    std::cout << std::endl << "cuCGImageOperator::mult_M not defined." << std::endl;
    return -1;
  }
  
  virtual int mult_MH(cuNDArray<T>* in, cuNDArray<T>* out, bool accumulate = false){
    std::cout << std::endl << "cuCGImageOperator::mult_MH not defined." << std::endl;
    return -1;
  }
  
  virtual int mult_MH_M(cuNDArray<T>* in, cuNDArray<T>* out, bool accumulate = false){

    if( !accumulate ) 
      cuNDA_clear(out);
    
    if( cuNDA_axpy<REAL>( image_.get(), in, out ) )
      return 0;
    else
      return -2;
  }
  
  cuNDArray<REAL>* get() { return image_.get(); }
  
private:
  boost::shared_ptr< cuCGMatrixOperator<REAL,T> > encoding_operator_;
  boost::shared_ptr< cuNDArray<REAL> > image_;
};