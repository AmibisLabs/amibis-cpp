/**
 * @file MultipleReferencedData.h
 * @brief Definition of MultipleReferencedData class
 */

#ifndef __MULTIPLE_REFERENCED_DATA_H__
#define __MULTIPLE_REFERENCED_DATA_H__

#include <System/Config.h>
#include <System/AtomicCounter.h>

namespace Omiscid {

/**
 * @class MultipleReferencedData MultipleReferencedData.h System/MultipleReferencedData.h
 * @brief Container for data with multiple references
 *
 * This container manages a system of reference.
 * It is released when there is no more reference on it.
 * After use, the users release the object by calling ReleaseMultimediaBuffer
 */
class MultipleReferencedData
{
 public:
  typedef void (FUNCTION_CALL_TYPE *MethodForRelease)(MultipleReferencedData*);

 public: 
  /** @brief Constructor 
   * @see Init */
  MultipleReferencedData(MethodForRelease method_for_release = NULL);

  /** @brief Destructor */
  virtual ~MultipleReferencedData();
 
  /** @brief Initialize the object
   *
   * Set the reference counter to 0.
   */
  virtual void Init();

  /** @brief Add a reference on the object */
  void AddRef();

  /** @brief Remove a reference on the object */
  void RemoveRef();


  /** @brief Test if the object is still referenced 
   * \return true if the number of reference is greater than 0 */
  bool IsStillUseful() const;

  /** @brief Release a MultipleReferencedData object
   *
   * Remove a reference on the object.
   * If the number of reference on the object fall to 0,
   * the object is released by using the method associated to the object. 
   * @param multiple_referenced_data the MultipleReferencedData object to release
   */
  static void ReleaseData(MultipleReferencedData* multiple_referenced_data);

 private:  

  AtomicCounter NbCurrentRef;  /*!< number of reference on the object */

  MethodForRelease methodForRelease; /*!< method used to release the object (if null objects are deleted)*/
};


} // namespace Omiscid

#endif // __MULTIPLE_REFERENCED_DATA_H__
