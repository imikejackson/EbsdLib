/* ============================================================================
 * Copyright (c) 2020 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <hdf5.h>

#include <list>
#include <set>
#include <string>
#include <vector>

#include "EbsdLib/Core/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/IO/EbsdReader.h"
#include "EbsdLib/IO/Oxford/H5OinaConstants.h"
#include "EbsdLib/IO/Oxford/OinaPhase.h"
#include "EbsdLib/IO/TSL/AngHeaderEntry.h"

/**
 * @class H5OinaReader H5OinaReader.h EbsdLib/BrukerNano/H5OinaReader.h
 * @brief
 *
 * After reading the HDF5 file one can use the various methods to extract the phases
 * or other header information or all the columnar data.
 */
class EbsdLib_EXPORT H5OinaReader : public EbsdReader
{
public:
  using Self = H5OinaReader;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  EBSD_STATIC_NEW_MACRO(H5OinaReader)
  /**
   * @brief Returns the name of the class for H5OinaReader
   */
  std::string getNameOfClass() const;
  /**
   * @brief Returns the name of the class for H5OinaReader
   */
  static std::string ClassName();

  ~H5OinaReader() override;

  /**
   * @brief The HDF5 path to find the EBSD data
   */
  /**
   * @brief Setter property for HDF5Path
   */
  void setHDF5Path(const std::string& value);
  /**
   * @brief Getter property for HDF5Path
   * @return Value of HDF5Path
   */
  std::string getHDF5Path() const;

  EBSD_INSTANCE_PROPERTY(bool, ReadPatternData)

  EBSD_PTR_INSTANCE_PROPERTY(uint8_t*, PatternData)

  EBSD_INSTANCE_2DVECTOR_PROPERTY(int, PatternDims)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, NumColumns, EbsdLib::H5Oina::XCells)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, NumRows, EbsdLib::H5Oina::YCells)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, XStep, EbsdLib::H5Oina::XStep)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, YStep, EbsdLib::H5Oina::YStep)

  /**
   * @brief These methods allow the developer to set/get the raw pointer for a given array, release ownership of the memory
   * and forcibly release the memory for a given array.
   *
   * The methods will follow the form of:
   * @brief This will return the raw pointer to the data. The Reader class WILL 'free' the memory when it goes out of
   * scope. If you need the memory to persist longer then call the release[NAME]Ownership() method to tell the reader
   * NOT to free the memory.
   * @param releaseOwnerhsip If this is true then the internal pointer is set to nullptr and the Cleanup flag is set to false.
   * [type]* get[NAME]Pointer(bool releaseOwnership = false);
   *
   * @brief This will get the ownership of the raw pointer. If 'true' then this class will 'free' the pointer before
   * each read or when the object goes out of scope.
   * bool get[NAME]Ownership();
   *
   * @brief This method will set the internal pointer to nullptr without calling 'free'. It is now up to the developer
   * to 'free' the memory that was used.
   * void release[NAME]Ownership();
   *
   * @brief This will free the internal pointer as long as it already isn't nullptr.
   * void free[NAME]Pointer();
   *
   */

  EBSD_POINTER_PROPERTY(BandContrast, BandContrast, EbsdLib::H5Oina::BandContrast_t)
  EBSD_POINTER_PROPERTY(BandSlope, BandSlope, EbsdLib::H5Oina::BandSlope_t)
  EBSD_POINTER_PROPERTY(Bands, Bands, EbsdLib::H5Oina::Bands_t)
  EBSD_POINTER_PROPERTY(Error, Error, EbsdLib::H5Oina::Error_t)
  EBSD_POINTER_PROPERTY(Euler, Euler, EbsdLib::H5Oina::Euler_t)
  EBSD_POINTER_PROPERTY(MeanAngularDeviation, MeanAngularDeviation, EbsdLib::H5Oina::MeanAngularDeviation_t)
  EBSD_POINTER_PROPERTY(Phase, Phase, EbsdLib::H5Oina::Phase_t)
  EBSD_POINTER_PROPERTY(X, X, EbsdLib::H5Oina::X_t)
  EBSD_POINTER_PROPERTY(Y, Y, EbsdLib::H5Oina::Y_t)

  /**
   * @brief releaseOwnership
   * @param name
   */
  void releaseOwnership(const std::string& name);

  /**
   * @brief Returns the pointer to the data for a given feature
   * @param featureName The name of the feature to return the pointer to.
   */
  void* getPointerByName(const std::string& featureName) override;

  /**
   * @brief Returns an enumeration value that depicts the numerical
   * primitive type that the data is stored as (Int, Float, etc).
   * @param featureName The name of the feature.
   */
  EbsdLib::NumericTypes::Type getPointerType(const std::string& featureName) override;

  /**
   * @brief Reads the file
   * @return error condition
   */
  int readFile() override;

  /**
   * @brief readScanNames
   * @return
   */
  int readScanNames(std::list<std::string>& names);

  /**
   * @brief Reads the header section of the file
   * @param Valid HDF5 Group ID
   * @return error condition
   */
  int readHeader(hid_t parId);

  /**
   * @brief Reads ONLY the header portion of the Oina HDF5 file
   * @return 1 on success
   */
  int readHeaderOnly() override;

  /**
   * @brief Returns a vector of AngPhase objects corresponding to the phases
   * present in the file
   */
  std::vector<OinaPhase::Pointer> getPhaseVector() const;

  /**
   * @brief Sets the names of the arrays to read out of the file
   * @param names
   */
  void setArraysToRead(const std::set<std::string>& names);

  /**
   * @brief Over rides the setArraysToReads to tell the reader to load ALL the data from the HDF5 file. If the
   * ArrayNames to read is empty and this is true then all arrays will be read.
   * @param b
   */
  void readAllArrays(bool b);

  int getXDimension() override;
  void setXDimension(int xdim) override;
  int getYDimension() override;
  void setYDimension(int ydim) override;

protected:
  H5OinaReader();

  /**
   * @brief Reads the data section of the file
   * @param Valid HDF5 Group ID
   * @return error condition
   */
  int readData(hid_t parId);

  /**
   * @brief sanityCheckForOpening
   * @return
   */
  int sanityCheckForOpening();

private:
  std::string m_HDF5Path = {};

  std::set<std::string> m_ArrayNames;
  bool m_ReadAllArrays = true;

  std::vector<OinaPhase::Pointer> m_Phases;

public:
  H5OinaReader(const H5OinaReader&) = delete;            // Copy Constructor Not Implemented
  H5OinaReader(H5OinaReader&&) = delete;                 // Move Constructor Not Implemented
  H5OinaReader& operator=(const H5OinaReader&) = delete; // Copy Assignment Not Implemented
  H5OinaReader& operator=(H5OinaReader&&) = delete;      // Move Assignment Not Implemented
};
