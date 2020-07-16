
#include <fstream>
#include <string>

#include "H5Support/H5Lite.h"
#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/Core/EbsdLibConstants.h"
#include "EbsdLib/IO/Oxford/H5OinaConstants.h"
#include "EbsdLib/IO/Oxford/H5OinaReader.h"
#include "EbsdLib/Test/EbsdLibTestFileLocations.h"

#include "UnitTestSupport.hpp"

class H5OinaReaderTest
{
  const std::string k_HDF5Path = std::string("1");
  const std::string k_InputFilePath = UnitTest::DataDir + "Alumin.h5oina";

public:
  H5OinaReaderTest() = default;
  ~H5OinaReaderTest() = default;

  EBSD_GET_NAME_OF_CLASS_DECL(H5OinaReaderTest)

  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    //   fs::remove(UnitTest::H5OinaReaderTest::OutputFile);
#endif
  }

  // -----------------------------------------------------------------------------
  void TestH5OinaReader()
  {
    H5OinaReader::Pointer reader = H5OinaReader::New();

    reader->readHeaderOnly();
    int32_t err = reader->getErrorCode();
    DREAM3D_REQUIRED(err, ==, -1)

    reader->setHDF5Path(k_HDF5Path);
    reader->readHeaderOnly();
    err = reader->getErrorCode();
    DREAM3D_REQUIRED(err, ==, -387)

    reader->setFileName("Some Random Path");
    reader->readHeaderOnly();
    err = reader->getErrorCode();
    DREAM3D_REQUIRED(err, ==, -2)

    reader->setFileName(k_InputFilePath);
    reader->readHeaderOnly();
    err = reader->getErrorCode();
    if(err < 0)
    {
      std::cout << reader->getErrorMessage();
    }

    int32_t ncols = reader->getNumColumns();
    DREAM3D_REQUIRED(ncols, ==, 600)

    int32_t nrows = reader->getNumRows();
    DREAM3D_REQUIRED(nrows, ==, 600)

    float xstep = reader->getXStep();
    DREAM3D_REQUIRED(xstep, ==, 0.0)
    float ystep = reader->getYStep();
    DREAM3D_REQUIRED(ystep, ==, 0.0)

    int32_t xDim = reader->getXDimension();
    DREAM3D_REQUIRED(xDim, ==, 600)

    int32_t yDim = reader->getYDimension();
    DREAM3D_REQUIRED(yDim, ==, 600)

    std::list<std::string> scanNames;
    err = reader->readScanNames(scanNames);
    DREAM3D_REQUIRED(scanNames.size(), ==, 1)
    DREAM3D_REQUIRED(scanNames.front(), ==, k_HDF5Path)

    err = reader->readFile();

    void* ptr = reader->getPointerByName(EbsdLib::H5Oina::BandContrast);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);

    ptr = reader->getPointerByName(EbsdLib::H5Oina::BandSlope);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);

    ptr = reader->getPointerByName(EbsdLib::H5Oina::Bands);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(EbsdLib::H5Oina::Error);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(EbsdLib::H5Oina::Euler);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(EbsdLib::H5Oina::MeanAngularDeviation);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(EbsdLib::H5Oina::Phase);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(EbsdLib::H5Oina::X);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);

    ptr = reader->getPointerByName(EbsdLib::H5Oina::Y);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);

    EbsdLib::NumericTypes::Type numType = reader->getPointerType(EbsdLib::H5Oina::BandContrast);
    DREAM3D_REQUIRE(numType == EbsdLib::NumericTypes::Type::UInt8)

    numType = reader->getPointerType(EbsdLib::H5Oina::BandSlope);
    DREAM3D_REQUIRE(numType == EbsdLib::NumericTypes::Type::UInt8)

    numType = reader->getPointerType(EbsdLib::H5Oina::Bands);
    DREAM3D_REQUIRE(numType == EbsdLib::NumericTypes::Type::UInt8)
    numType = reader->getPointerType(EbsdLib::H5Oina::Error);
    DREAM3D_REQUIRE(numType == EbsdLib::NumericTypes::Type::UInt8)
    numType = reader->getPointerType(EbsdLib::H5Oina::Euler);
    DREAM3D_REQUIRE(numType == EbsdLib::NumericTypes::Type::Float)
    numType = reader->getPointerType(EbsdLib::H5Oina::MeanAngularDeviation);
    DREAM3D_REQUIRE(numType == EbsdLib::NumericTypes::Type::Float)
    numType = reader->getPointerType(EbsdLib::H5Oina::Phase);
    DREAM3D_REQUIRE(numType == EbsdLib::NumericTypes::Type::UInt8)
    numType = reader->getPointerType(EbsdLib::H5Oina::X);
    DREAM3D_REQUIRE(numType == EbsdLib::NumericTypes::Type::Float)

    numType = reader->getPointerType(EbsdLib::H5Oina::Y);
    DREAM3D_REQUIRE(numType == EbsdLib::NumericTypes::Type::Float)

    //    float* xsamplePtr = reader->getXSAMPLEPointer();
    //    DREAM3D_REQUIRE_VALID_POINTER(xsamplePtr);
    //    reader->releaseXSAMPLEOwnership();
    //    ptr = reader->getPointerByName(EbsdLib::H5Oina::XSAMPLE);
    //    DREAM3D_REQUIRE_NULL_POINTER(ptr);
    //    reader->deallocateArrayData<float>(xsamplePtr);

    //    float* ysamplePtr = reader->getYSAMPLEPointer();
    //    DREAM3D_REQUIRE_VALID_POINTER(ysamplePtr);
    //    reader->releaseYSAMPLEOwnership();
    //    ptr = reader->getPointerByName(EbsdLib::H5Oina::YSAMPLE);
    //    DREAM3D_REQUIRE_NULL_POINTER(ptr);
    //    reader->deallocateArrayData<float>(ysamplePtr);

    // std::cout << "=============================================================" << std::endl;
    std::set<std::string> arraysToRead;
    arraysToRead.insert(EbsdLib::H5Oina::Euler);
    reader->setArraysToRead(arraysToRead);
    err = reader->readFile();

    ptr = reader->getPointerByName(EbsdLib::H5Oina::Euler);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);

    //    ptr = reader->getPointerByName(EbsdLib::H5Oina::XSAMPLE);
    //    DREAM3D_REQUIRE_NULL_POINTER(ptr);
    //    ptr = reader->getPointerByName(EbsdLib::H5Oina::YSAMPLE);
    //    DREAM3D_REQUIRE_NULL_POINTER(ptr);

#if 0
      reader->setReadPatternData(true);
      err = reader->readFile();
      err = reader->getErrorCode();
      if(err < 0)
      {
        std::cout << reader->getErrorMessage();
      }
      DREAM3D_REQUIRED(err, >=, 0)
#endif
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "<===== Start " << getNameOfClass() << std::endl;

    DREAM3D_REGISTER_TEST(TestH5OinaReader())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

public:
  H5OinaReaderTest(const H5OinaReaderTest&) = delete;            // Copy Constructor Not Implemented
  H5OinaReaderTest(H5OinaReaderTest&&) = delete;                 // Move Constructor Not Implemented
  H5OinaReaderTest& operator=(const H5OinaReaderTest&) = delete; // Copy Assignment Not Implemented
  H5OinaReaderTest& operator=(H5OinaReaderTest&&) = delete;      // Move Assignment Not Implemented
};
