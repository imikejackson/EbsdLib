/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "H5OinaReader.h"

#include <algorithm>
#include <iostream>

#include "H5Support/H5Lite.h"
#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/Core/EbsdLibConstants.h"
#include "EbsdLib/Core/EbsdMacros.h"

// -----------------------------------------------------------------------------
H5OinaReader::H5OinaReader()
: m_ReadPatternData(false)
, m_HDF5Path()
{
  m_HeaderMap.clear();
  // Initialize the map of header key to header value
  m_HeaderMap[EbsdLib::H5Oina::AcquisitionDate] = AngStringHeaderEntry::NewEbsdHeaderEntry(EbsdLib::H5Oina::AcquisitionDate);
  m_HeaderMap[EbsdLib::H5Oina::AcquisitionSpeed] = AngHeaderEntry<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::AcquisitionSpeed);
  m_HeaderMap[EbsdLib::H5Oina::AcquisitionTime] = AngHeaderEntry<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::AcquisitionTime);
  m_HeaderMap[EbsdLib::H5Oina::BeamVoltage] = AngHeaderEntry<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::BeamVoltage);
  m_HeaderMap[EbsdLib::H5Oina::DetectorOrientationEuler] = AngHeaderArray<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::DetectorOrientationEuler);
  m_HeaderMap[EbsdLib::H5Oina::Magnification] = AngHeaderEntry<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::Magnification);
  m_HeaderMap[EbsdLib::H5Oina::ProjectFile] = AngStringHeaderEntry::NewEbsdHeaderEntry(EbsdLib::H5Oina::ProjectFile);
  m_HeaderMap[EbsdLib::H5Oina::ProjectLabel] = AngStringHeaderEntry::NewEbsdHeaderEntry(EbsdLib::H5Oina::ProjectLabel);
  m_HeaderMap[EbsdLib::H5Oina::ProjectNotes] = AngStringHeaderEntry::NewEbsdHeaderEntry(EbsdLib::H5Oina::ProjectNotes);
  m_HeaderMap[EbsdLib::H5Oina::ScanningRotationAngle] = AngHeaderEntry<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::ScanningRotationAngle);
  m_HeaderMap[EbsdLib::H5Oina::SiteNotes] = AngStringHeaderEntry::NewEbsdHeaderEntry(EbsdLib::H5Oina::SiteNotes);
  m_HeaderMap[EbsdLib::H5Oina::SpecimenOrientationEuler] = AngHeaderArray<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::SpecimenOrientationEuler);
  m_HeaderMap[EbsdLib::H5Oina::TiltAngle] = AngHeaderEntry<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::TiltAngle);
  m_HeaderMap[EbsdLib::H5Oina::TiltAxis] = AngHeaderEntry<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::TiltAxis);
  m_HeaderMap[EbsdLib::H5Oina::XCells] = AngHeaderEntry<int32_t>::NewEbsdHeaderEntry(EbsdLib::H5Oina::XCells);
  m_HeaderMap[EbsdLib::H5Oina::XStep] = AngHeaderEntry<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::XStep);
  m_HeaderMap[EbsdLib::H5Oina::YCells] = AngHeaderEntry<int32_t>::NewEbsdHeaderEntry(EbsdLib::H5Oina::YCells);
  m_HeaderMap[EbsdLib::H5Oina::YStep] = AngHeaderEntry<float>::NewEbsdHeaderEntry(EbsdLib::H5Oina::YStep);

  m_PatternDims[0] = -1;
  m_PatternDims[1] = -1;
}

// -----------------------------------------------------------------------------
H5OinaReader::~H5OinaReader()
{

  if(m_BandContrastCleanup)
  {
    deallocateArrayData<EbsdLib::H5Oina::BandContrast_t>(m_BandContrast);
    m_BandContrast = nullptr;
  }

  if(m_BandSlopeCleanup)
  {
    deallocateArrayData<EbsdLib::H5Oina::BandSlope_t>(m_BandSlope);
    m_BandSlope = nullptr;
  }

  if(m_BandsCleanup)
  {
    deallocateArrayData<EbsdLib::H5Oina::Bands_t>(m_Bands);
    m_Bands = nullptr;
  }
  if(m_ErrorCleanup)
  {
    deallocateArrayData<EbsdLib::H5Oina::Error_t>(m_Error);
    m_Error = nullptr;
  }
  if(m_EulerCleanup)
  {
    deallocateArrayData<EbsdLib::H5Oina::Euler_t>(m_Euler);
    m_Euler = nullptr;
  }
  if(m_MeanAngularDeviationCleanup)
  {
    deallocateArrayData<EbsdLib::H5Oina::MeanAngularDeviation_t>(m_MeanAngularDeviation);
    m_MeanAngularDeviation = nullptr;
  }
  if(m_PhaseCleanup)
  {
    deallocateArrayData<EbsdLib::H5Oina::Phase_t>(m_Phase);
    m_Phase = nullptr;
  }
  if(m_XCleanup)
  {
    deallocateArrayData<EbsdLib::H5Oina::X_t>(m_X);
    m_X = nullptr;
  }
  if(m_YCleanup)
  {
    deallocateArrayData<EbsdLib::H5Oina::Y_t>(m_Y);
    m_Y = nullptr;
  }
}

// -----------------------------------------------------------------------------
std::vector<OinaPhase::Pointer> H5OinaReader::getPhaseVector() const
{
  return m_Phases;
}

// -----------------------------------------------------------------------------
int H5OinaReader::sanityCheckForOpening()
{
  int err = 0;
  setErrorCode(0);
  setErrorMessage(std::string(""));

  if(m_HDF5Path.empty())
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: HDF5 Path is empty.";
    setErrorCode(-1);
    setErrorMessage(str);
    return err;
  }

  if(getFileName().empty())
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: Input filepath is empty";
    setErrorCode(-387);
    setErrorMessage(str);
    return err;
  }
  return err;
}

// -----------------------------------------------------------------------------
int H5OinaReader::readFile()
{
  int err = sanityCheckForOpening();
  if(getErrorCode() < 0)
  {
    return getErrorCode();
  }

  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if(fileId < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: Could not open HDF5 file '" << getFileName() << "'";
    setErrorCode(-2);
    setErrorMessage(str);
    return err;
  }

  H5ScopedFileSentinel sentinel(&fileId, false);
  hid_t gid = H5Gopen(fileId, m_HDF5Path.c_str(), H5P_DEFAULT);
  if(gid < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: Could not open path '" << m_HDF5Path << "'";
    err = H5Utilities::closeFile(fileId);
    setErrorCode(-90020);
    setErrorMessage(str);
    return getErrorCode();
  }
  sentinel.addGroupId(&gid);

  hid_t ebsdGid = H5Gopen(gid, EbsdLib::H5Oina::EBSD.c_str(), H5P_DEFAULT);
  if(ebsdGid < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: Could not open 'EBSD' Group";
    setErrorCode(-90007);
    setErrorMessage(str);
    return getErrorCode();
  }
  sentinel.addGroupId(&ebsdGid);

  // Read all the header information
  err = readHeader(ebsdGid);
  if(err < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: could not read header";
    return getErrorCode();
  }

  // Read data
  err = readData(ebsdGid);
  if(err < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: could not read data. Internal Error code " << getErrorCode() << " generated.";
    return getErrorCode();
  }

  err = H5Gclose(ebsdGid);
  ebsdGid = -1;
  if(err < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: could not close group id ";
    setErrorCode(-900023);
    setErrorMessage(str);
    return getErrorCode();
  }

  err = H5Gclose(gid);
  gid = -1;
  if(err < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: could not close group id ";
    setErrorCode(-900023);
    setErrorMessage(str);
    return getErrorCode();
  }

  err = H5Utilities::closeFile(fileId);
  fileId = -1;
  if(err < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: could not close file";
    setErrorCode(-900024);
    setErrorMessage(str);
    return getErrorCode();
  }

  return getErrorCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OinaReader::readHeaderOnly()
{
  int err = sanityCheckForOpening();
  if(getErrorCode() < 0)
  {
    return getErrorCode();
  }
  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if(fileId < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: Could not open HDF5 file '" << getFileName() << "'";
    setErrorCode(-2);
    setErrorMessage(str);
    return err;
  }

  H5ScopedFileSentinel sentinel(&fileId, false);
  hid_t gid = H5Gopen(fileId, m_HDF5Path.c_str(), H5P_DEFAULT);
  if(gid < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: Could not open path '" << m_HDF5Path << "'";
    err = H5Utilities::closeFile(fileId);
    setErrorCode(-90020);
    setErrorMessage(str);
    return getErrorCode();
  }
  sentinel.addGroupId(&gid);

  hid_t ebsdGid = H5Gopen(gid, EbsdLib::H5Oina::EBSD.c_str(), H5P_DEFAULT);
  if(ebsdGid < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: Could not open 'EBSD' Group";
    setErrorCode(-90007);
    setErrorMessage(str);
    return getErrorCode();
  }
  sentinel.addGroupId(&ebsdGid);

  // Read all the header information
  err = readHeader(ebsdGid);
  if(err < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: could not read header";
    return getErrorCode();
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OinaReader::readScanNames(std::list<std::string>& names)
{

  // TODO: This needs updating to read from the index data set
  if(getFileName().empty())
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: Input filepath is empty";
    setErrorCode(-387);
    setErrorMessage(str);
    return getErrorCode();
  }

  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if(fileId < 0)
  {
    std::string str;
    std::stringstream ss(str);
    ss << getNameOfClass() << " Error: Could not open HDF5 file '" << getFileName() << "'";
    setErrorCode(-20);
    setErrorMessage(str);
    names.clear();
    return getErrorCode();
  }
  H5ScopedFileSentinel sentinel(&fileId, false);

  int32_t err = H5Utilities::getGroupObjects(fileId, H5Utilities::CustomHDFDataTypes::Group, names);
  setErrorCode(err);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OinaReader::readHeader(hid_t parId)
{
  using AngHeaderFloatType = AngHeaderEntry<float>;
  using AngHeaderIntType = AngHeaderEntry<int32_t>;
  using AngHeaderFloatArrayType = AngHeaderArray<float>;

  m_PatternDims[0] = -1;
  m_PatternDims[1] = -2;

  int err = -1;

  hid_t gid = H5Gopen(parId, EbsdLib::H5Oina::Header.c_str(), H5P_DEFAULT);
  if(gid < 0)
  {
    setErrorCode(-90008);
    setErrorMessage("H5OinaReader Error: Could not open 'Header' Group");
    return -1;
  }
  H5ScopedGroupSentinel sentinel(&gid, false);

  ReadH5EbsdHeaderStringData<H5OinaReader, std::string, AngStringHeaderEntry>(this, EbsdLib::H5Oina::AcquisitionDate, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, float, AngHeaderFloatType>(this, EbsdLib::H5Oina::AcquisitionSpeed, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, float, AngHeaderFloatType>(this, EbsdLib::H5Oina::AcquisitionTime, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, float, AngHeaderFloatType>(this, EbsdLib::H5Oina::BeamVoltage, gid, m_HeaderMap);
  ReadH5EbsdHeaderArray<H5OinaReader, float, AngHeaderFloatArrayType>(this, EbsdLib::H5Oina::DetectorOrientationEuler, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, float, AngHeaderFloatType>(this, EbsdLib::H5Oina::Magnification, gid, m_HeaderMap);
  ReadH5EbsdHeaderStringData<H5OinaReader, std::string, AngStringHeaderEntry>(this, EbsdLib::H5Oina::ProjectFile, gid, m_HeaderMap);
  ReadH5EbsdHeaderStringData<H5OinaReader, std::string, AngStringHeaderEntry>(this, EbsdLib::H5Oina::ProjectLabel, gid, m_HeaderMap);
  ReadH5EbsdHeaderStringData<H5OinaReader, std::string, AngStringHeaderEntry>(this, EbsdLib::H5Oina::ProjectNotes, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, float, AngHeaderFloatType>(this, EbsdLib::H5Oina::ScanningRotationAngle, gid, m_HeaderMap);
  ReadH5EbsdHeaderStringData<H5OinaReader, std::string, AngStringHeaderEntry>(this, EbsdLib::H5Oina::SiteNotes, gid, m_HeaderMap);
  ReadH5EbsdHeaderArray<H5OinaReader, float, AngHeaderFloatArrayType>(this, EbsdLib::H5Oina::SpecimenOrientationEuler, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, float, AngHeaderFloatType>(this, EbsdLib::H5Oina::TiltAngle, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, float, AngHeaderFloatType>(this, EbsdLib::H5Oina::TiltAxis, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, int32_t, AngHeaderIntType>(this, EbsdLib::H5Oina::XCells, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, float, AngHeaderFloatType>(this, EbsdLib::H5Oina::XStep, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, int32_t, AngHeaderIntType>(this, EbsdLib::H5Oina::YCells, gid, m_HeaderMap);
  ReadH5EbsdHeaderData<H5OinaReader, float, AngHeaderFloatType>(this, EbsdLib::H5Oina::YStep, gid, m_HeaderMap);

  HDF_ERROR_HANDLER_OFF
#if 0
  // Read the Pattern Width - This may not exist
  bool patWidthExists = H5Lite::datasetExists(gid, EbsdLib::H5Oina::PatternWidth);
  if(patWidthExists)
  {
    ReadH5EbsdHeaderData<H5OinaReader, int32_t, AngHeaderIntType>(this, EbsdLib::H5Oina::PatternWidth, gid, m_HeaderMap);
    m_PatternDims[1] = getPatternWidth();
  }
  else if(getReadPatternData() && !patWidthExists)
  {
    setErrorCode(-90016);
    setErrorMessage("H5OinaReader Error: Header/PatternWidth data set does not exist and ReadPatternData = true.");
    H5Gclose(gid);
    return getErrorCode();
  }

  // Read the Pattern Height - This may not exist
  bool patHeightExists = H5Lite::datasetExists(gid, EbsdLib::H5Oina::PatternHeight);
  if(patHeightExists)
  {
    ReadH5EbsdHeaderData<H5OinaReader, int32_t, AngHeaderIntType>(this, EbsdLib::H5Oina::PatternHeight, gid, m_HeaderMap);
    m_PatternDims[0] = getPatternHeight();
  }
  else if(getReadPatternData() && !patHeightExists)
  {
    setErrorCode(-90017);
    setErrorMessage("H5OinaReader Error: Header/PatternHeight data set does not exist and ReadPatternData = true.");
    H5Gclose(gid);
    return getErrorCode();
  }

  bool patternDataExists = H5Lite::datasetExists(parId, EbsdLib::H5Oina::Data + "/" + EbsdLib::H5Oina::RawPatterns);
  if(getReadPatternData() && !patternDataExists)
  {
    setErrorCode(-90018);
    setErrorMessage("H5OinaReader Error: Data/RawPatterns data set does not exist and ReadPatternData = true.");
    H5Gclose(gid);
    return getErrorCode();
  }
  HDF_ERROR_HANDLER_ON
#endif

  hid_t phasesGid = H5Gopen(gid, EbsdLib::H5Oina::Phases.c_str(), H5P_DEFAULT);
  if(phasesGid < 0)
  {
    setErrorCode(-90007);
    setErrorMessage("H5OinaReader Error: Could not open Header/Error HDF Group.");
    H5Gclose(gid);
    return getErrorCode();
  }
  sentinel.addGroupId(&phasesGid);

  std::list<std::string> names;
  err = H5Utilities::getGroupObjects(phasesGid, H5Utilities::CustomHDFDataTypes::Group, names);
  if(err < 0 || names.empty())
  {
    setErrorCode(-90009);
    setErrorMessage("H5OinaReader Error: There were no Error groups present in the HDF5 file");
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return getErrorCode();
  }
  // m_Phases.clear();
  std::vector<OinaPhase::Pointer> PhaseVector;

  for(const auto& PhaseGroupName : names)
  {
    hid_t pid = H5Gopen(phasesGid, PhaseGroupName.c_str(), H5P_DEFAULT);

    OinaPhase::Pointer currentPhase = OinaPhase::New();
    currentPhase->setPhaseIndex(std::stoi(PhaseGroupName));
    READ_PHASE_HEADER_ARRAY("H5OinaReader", pid, uint8_t, EbsdLib::H5Oina::Color, Color, currentPhase)
    READ_PHASE_HEADER_ARRAY("H5OinaReader", pid, float, EbsdLib::H5Oina::LatticeAngles, LatticeAngles, currentPhase);
    READ_PHASE_HEADER_ARRAY("H5OinaReader", pid, float, EbsdLib::H5Oina::LatticeDimensions, LatticeDimensions, currentPhase);
    READ_PHASE_HEADER_DATA("H5OinaReader", pid, int32_t, EbsdLib::H5Oina::LaueGroup, LaueGroup, currentPhase)
    READ_PHASE_HEADER_DATA("H5OinaReader", pid, int32_t, EbsdLib::H5Oina::NumberReflectors, NumberReflectors, currentPhase)
    //    READ_PHASE_STRING_DATA("H5OinaReader", pid, EbsdLib::H5Oina::PhaseName, PhaseName, currentPhase)
    //    READ_PHASE_STRING_DATA("H5OinaReader", pid, EbsdLib::H5Oina::Reference, Reference, currentPhase)
    READ_PHASE_HEADER_DATA("H5OinaReader", pid, int32_t, EbsdLib::H5Oina::SpaceGroup, SpaceGroup, currentPhase)

    {
      std::string t;
      err = H5Lite::readStringDataset(pid, EbsdLib::H5Oina::PhaseName, t);
      if(err < 0)
      {
        std::stringstream ss;
        ss << "H5OinaReader"
           << ": The header value for '" << EbsdLib::H5Oina::PhaseName << "' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?";
        setErrorCode(-90003);
        setErrorMessage(ss.str());
        err = H5Gclose(pid);
        H5Gclose(phasesGid);
        H5Gclose(gid);
        return -1;
      }
      currentPhase->setPhaseName(t);
    }

    PhaseVector.push_back(currentPhase);
    err = H5Gclose(pid);
  }

  m_Phases = PhaseVector;

  std::string completeHeader;
  setOriginalHeader(completeHeader);

  err = H5Gclose(phasesGid);
  phasesGid = -1;
  err = H5Gclose(gid);
  gid = -1;

  return getErrorCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OinaReader::readData(hid_t parId)
{
  int err = -1;

  // Initialize new pointers
  size_t totalDataRows = 0;

  size_t nColumns = getNumColumns();
  size_t nRows = getNumRows();

  if(nRows < 1)
  {
    err = -200;
    setErrorMessage("H5OinaReader Error: The number of Rows was < 1.");
    setErrorCode(err);
    return err;
  }

  if(totalDataRows == 0)
  {
    setErrorCode(-90301);
    setErrorMessage("There is no data to read. NumRows or NumColumns is Zero (0)");
    return -301;
  }

  hid_t gid = H5Gopen(parId, EbsdLib::H5Oina::Data.c_str(), H5P_DEFAULT);
  if(gid < 0)
  {
    setErrorMessage("H5OIMReader Error: Could not open 'Data' Group");
    setErrorCode(-90012);
    return -90012;
  }
  setNumberOfElements(totalDataRows);
  size_t numBytes = totalDataRows * sizeof(float);
  std::string sBuf;
  std::stringstream ss(sBuf);

  if(m_ArrayNames.empty() && !m_ReadAllArrays)
  {
    err = H5Gclose(gid);
    err = -90013;
    setErrorMessage("H5OIMReader Error: ReadAllArrays was FALSE and no other arrays were requested to be read.");
    setErrorCode(err);
    return err;
  }

  ANG_READER_ALLOCATE_AND_READ(BandContrast, EbsdLib::H5Oina::BandContrast, EbsdLib::H5Oina::BandContrast_t);
  ANG_READER_ALLOCATE_AND_READ(BandSlope, EbsdLib::H5Oina::BandSlope, EbsdLib::H5Oina::BandSlope_t);
  ANG_READER_ALLOCATE_AND_READ(Bands, EbsdLib::H5Oina::Bands, EbsdLib::H5Oina::Bands_t);
  ANG_READER_ALLOCATE_AND_READ(Error, EbsdLib::H5Oina::Error, EbsdLib::H5Oina::Error_t);
  ANG_READER_ALLOCATE_AND_READ(Euler, EbsdLib::H5Oina::Euler, EbsdLib::H5Oina::Euler_t);
  ANG_READER_ALLOCATE_AND_READ(MeanAngularDeviation, EbsdLib::H5Oina::MeanAngularDeviation, EbsdLib::H5Oina::MeanAngularDeviation_t);
  ANG_READER_ALLOCATE_AND_READ(Phase, EbsdLib::H5Oina::Phase, EbsdLib::H5Oina::Phase_t);
  ANG_READER_ALLOCATE_AND_READ(X, EbsdLib::H5Oina::X, EbsdLib::H5Oina::X_t);
  ANG_READER_ALLOCATE_AND_READ(Y, EbsdLib::H5Oina::Y, EbsdLib::H5Oina::Y_t);

  if(m_ReadPatternData)
  {
#if 0
    H5T_class_t type_class;
    std::vector<hsize_t> dims;
    size_t type_size = 0;
    err = H5Lite::getDatasetInfo(gid, EbsdLib::H5Oina::RawPatterns, dims, type_class, type_size);
    if(err >= 0) // Only read the pattern data if the pattern data is available.
    {
      totalDataRows = std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<size_t>());

      // Set the pattern dimensions
      m_PatternDims[0] = dims[1];
      m_PatternDims[1] = dims[2];

      m_PatternData = this->allocateArray<uint8_t>(totalDataRows);
      err = H5Lite::readPointerDataset(gid, EbsdLib::H5Oina::RawPatterns, m_PatternData);
    }
#endif
  }
  err = H5Gclose(gid);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5OinaReader::setArraysToRead(const std::set<std::string>& names)
{
  m_ArrayNames = names;
  m_ReadAllArrays = m_ArrayNames.empty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5OinaReader::readAllArrays(bool b)
{
  m_ReadAllArrays = b;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OinaReader::getXDimension()
{
  return getNumColumns();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5OinaReader::setXDimension(int xdim)
{
  setNumColumns(xdim);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OinaReader::getYDimension()
{
  return getNumRows();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5OinaReader::setYDimension(int ydim)
{
  setNumRows(ydim);
}

// -----------------------------------------------------------------------------
void H5OinaReader::releaseOwnership(const std::string& name)
{
  if(name == EbsdLib::H5Oina::BandContrast)
  {
    m_BandContrast = nullptr;
    m_BandContrastCleanup = false;
  }
  if(name == EbsdLib::H5Oina::BandSlope)
  {
    m_BandSlope = nullptr;
    m_BandSlopeCleanup = false;
  }
  if(name == EbsdLib::H5Oina::Bands)
  {
    m_Bands = nullptr;
    m_BandsCleanup = false;
  }
  if(name == EbsdLib::H5Oina::Error)
  {
    m_Error = nullptr;
    m_ErrorCleanup = false;
  }
  if(name == EbsdLib::H5Oina::Euler)
  {
    m_Euler = nullptr;
    m_EulerCleanup = false;
  }
  if(name == EbsdLib::H5Oina::MeanAngularDeviation)
  {
    m_MeanAngularDeviation = nullptr;
    m_MeanAngularDeviationCleanup = false;
  }
#if 0
  if(name == EbsdLib::H5Oina::RawPatterns)
  {
    m_PatternData = nullptr;
    m_PatternDataCleanup = false;
  }
#endif
  if(name == EbsdLib::H5Oina::Phase)
  {
    m_Phase = nullptr;
    m_PhaseCleanup = false;
  }
  if(name == EbsdLib::H5Oina::X)
  {
    m_X = nullptr;
    m_XCleanup = false;
  }
  if(name == EbsdLib::H5Oina::Y)
  {
    m_Y = nullptr;
    m_YCleanup = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* H5OinaReader::getPointerByName(const std::string& featureName)
{
  if(featureName == EbsdLib::H5Oina::BandContrast)
  {
    return static_cast<void*>(m_BandContrast);
  }
  if(featureName == EbsdLib::H5Oina::BandSlope)
  {
    return static_cast<void*>(m_BandSlope);
  }
  if(featureName == EbsdLib::H5Oina::Bands)
  {
    return static_cast<void*>(m_Bands);
  }
  if(featureName == EbsdLib::H5Oina::Error)
  {
    return static_cast<void*>(m_Error);
  }
  if(featureName == EbsdLib::H5Oina::Euler)
  {
    return static_cast<void*>(m_Euler);
  }
  if(featureName == EbsdLib::H5Oina::MeanAngularDeviation)
  {
    return static_cast<void*>(m_MeanAngularDeviation);
  }
#if 0
  if(featureName == EbsdLib::H5Oina::RawPatterns)
  {
    return static_cast<void*>(m_PatternData);
  }
#endif
  if(featureName == EbsdLib::H5Oina::Phase)
  {
    return static_cast<void*>(m_Phase);
  }
  if(featureName == EbsdLib::H5Oina::X)
  {
    return static_cast<void*>(m_X);
  }
  if(featureName == EbsdLib::H5Oina::Y)
  {
    return static_cast<void*>(m_Y);
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdLib::NumericTypes::Type H5OinaReader::getPointerType(const std::string& featureName)
{

  if(featureName == EbsdLib::H5Oina::BandContrast)
  {
    return EbsdLib::NumericTypes::Type::UInt8;
  }

  if(featureName == EbsdLib::H5Oina::BandSlope)
  {
    return EbsdLib::NumericTypes::Type::UInt8;
  }
  if(featureName == EbsdLib::H5Oina::Bands)
  {
    return EbsdLib::NumericTypes::Type::UInt8;
  }
  if(featureName == EbsdLib::H5Oina::Error)
  {
    return EbsdLib::NumericTypes::Type::UInt8;
  }
  if(featureName == EbsdLib::H5Oina::Euler)
  {
    return EbsdLib::NumericTypes::Type::Float;
  }
  if(featureName == EbsdLib::H5Oina::MeanAngularDeviation)
  {
    return EbsdLib::NumericTypes::Type::Float;
  }
  if(featureName == EbsdLib::H5Oina::Phase)
  {
    return EbsdLib::NumericTypes::Type::UInt8;
  }
  if(featureName == EbsdLib::H5Oina::X)
  {
    return EbsdLib::NumericTypes::Type::Float;
  }
  if(featureName == EbsdLib::H5Oina::Y)
  {
    return EbsdLib::NumericTypes::Type::Float;
  }
#if 0
  if(featureName == EbsdLib::H5Oina::RawPatterns)
  {
    return EbsdLib::NumericTypes::Type::UInt8;
  }
#endif
  return EbsdLib::NumericTypes::Type::UnknownNumType;
}

// -----------------------------------------------------------------------------
H5OinaReader::Pointer H5OinaReader::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
void H5OinaReader::setHDF5Path(const std::string& value)
{
  m_HDF5Path = value;
}

// -----------------------------------------------------------------------------
std::string H5OinaReader::getHDF5Path() const
{
  return m_HDF5Path;
}

// -----------------------------------------------------------------------------
std::string H5OinaReader::getNameOfClass() const
{
  return std::string("H5OinaReader");
}

// -----------------------------------------------------------------------------
std::string H5OinaReader::ClassName()
{
  return std::string("H5OinaReader");
}
