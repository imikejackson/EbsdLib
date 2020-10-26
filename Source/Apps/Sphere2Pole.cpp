

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/Core/EbsdDataArray.hpp"
#include "EbsdLib/Core/Quaternion.hpp"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/LaueOps/HexagonalOps.h"
#include "EbsdLib/Utilities/ComputeStereographicProjection.h"
#include "EbsdLib/Utilities/EbsdStringUtils.hpp"
#include "EbsdLib/Utilities/ModifiedLambertProjection.h"
#include "EbsdLib/Utilities/PoleFigureUtilities.h"
#include "EbsdLib/Utilities/TiffWriter.h"

class Sphere2PoleFigure
{
public:
  Sphere2PoleFigure(const std::string& filePath)
  : m_FilePath(filePath)
  {
  }
  ~Sphere2PoleFigure() = default;

  Sphere2PoleFigure(const Sphere2PoleFigure&) = delete;            // Copy Constructor Not Implemented
  Sphere2PoleFigure(Sphere2PoleFigure&&) = delete;                 // Move Constructor Not Implemented
  Sphere2PoleFigure& operator=(const Sphere2PoleFigure&) = delete; // Copy Assignment Not Implemented
  Sphere2PoleFigure& operator=(Sphere2PoleFigure&&) = delete;      // Move Assignment Not Implemented

  int32_t convert()
  {

    std::ifstream in(m_FilePath, std::ios_base::in);
    if(!in.is_open())
    {
      std::string msg = std::string("Ang file could not be opened: ") + getFileName();
      setErrorCode(-100);
      setErrorMessage(msg);
      return -100;
    }
    std::string buf;
    size_t numTuples = 0;
    // Count the number of lines
    while(!in.eof())
    {
      std::getline(in, buf);
      numTuples++;
    }
    in.close();

    std::vector<size_t> compDims = {3};
    EbsdLib::FloatArrayType xyzCoords(numTuples, "XYZ Coords", compDims, 0.0f);
    in.open(m_FilePath, std::ios_base::in);
    size_t tupleIndex = 0;
    std::array<float, 3> data;
    while(!in.eof())
    {
      std::getline(in, buf);
      EbsdStringUtils::StringTokenType tokens = EbsdStringUtils::split(buf, ',');

      data[0] = std::stof(tokens[0]);
      data[1] = std::stof(tokens[1]);
      data[2] = std::stof(tokens[2]);

      xyzCoords.setTuple(tupleIndex, data.data());
      tupleIndex++;
    }
    in.close();

    PoleFigureConfiguration_t config;
    config.imageDim = 512;
    config.lambertDim = 32;
    config.numColors = 32;
    config.minScale = 0.0;
    config.maxScale = 1001.0;
    config.sphereRadius = 1.0F;
    config.discrete = true;
    config.discreteHeatMap = false;
    config.labels = {"First", "Second", "Third"};
    config.order = {0, 1, 2};
    config.phaseName = "Simulation Data";

    // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
    ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::LambertBallToSquare(&xyzCoords, config.lambertDim, config.sphereRadius);
    // Now create the intensity image that will become the actual Pole figure image
    EbsdLib::DoubleArrayType::Pointer poleFigurePtr = lambert->createStereographicProjection(config.imageDim);

    hid_t fid = H5Utilities::createFile("/tmp/PoleFigureData.h5");
    poleFigurePtr->setName(config.phaseName);

    hid_t gid = H5Utilities::createGroup(fid, "PoleFigure");
    // size_t size = poleFigurePtr->size();
    std::vector<hsize_t> dims = {static_cast<hsize_t>(config.imageDim), static_cast<hsize_t>(config.imageDim)};
    int32_t err = H5Lite::writePointerDataset(gid, "Intensity", dims.size(), dims.data(), poleFigurePtr->getPointer(0));

    //    {
    //      std::ofstream out("/tmp/lambert_intensities.txt", std::ios_base::out);
    //      for(int32_t y = 0; y < config.imageDim; y++)
    //      {
    //        for(int32_t x = 0; x < config.imageDim; x++)
    //        {
    //          size_t idx = (y * config.imageDim) + x;
    //          out << (*poleFigurePtr)[idx] << "  ";
    //        }
    //        out << std::endl;
    //      }
    //    }

    // Find the Max and Min values so we can color scale
    double max = std::numeric_limits<double>::min();
    double min = std::numeric_limits<double>::max();

    double* dPtr = poleFigurePtr->getPointer(0);
    size_t count = poleFigurePtr->getNumberOfTuples();
    for(size_t i = 0; i < count; ++i)
    {
      if(dPtr[i] > max)
      {
        max = dPtr[i];
      }
      if(dPtr[i] < min)
      {
        min = dPtr[i];
      }
    }

    config.minScale = min;
    config.maxScale = max;
    config.discrete = false;
    std::string label0("Simulation Data");
    std::vector<size_t> idims = {4};
    EbsdLib::UInt8ArrayType::Pointer image001 = EbsdLib::UInt8ArrayType::CreateArray(static_cast<size_t>(config.imageDim * config.imageDim), idims, label0, true);

    GeneratePoleFigureRgbaImageImpl rgbaPoleFigure(poleFigurePtr.get(), &config, image001.get());
    rgbaPoleFigure();

    idims = {3};
    EbsdLib::UInt8ArrayType rgb(static_cast<size_t>(config.imageDim * config.imageDim), label0, idims, 0);
    for(size_t i = 0; i < image001->getNumberOfTuples(); i++)
    {
      rgb.setTuple(i, image001->getTuplePointer(i));
    }

    dims = {static_cast<hsize_t>(config.imageDim), static_cast<hsize_t>(config.imageDim), 3};
    err = H5Lite::writePointerDataset(gid, "RGB Pole Figure", dims.size(), dims.data(), rgb.getPointer(0));

    std::pair<int32_t, std::string> error = TiffWriter::WriteColorImage("/tmp/PoleFigure.tif", config.imageDim, config.imageDim, 3, rgb.getPointer(0));

    //    {
    //      std::ofstream out("/tmp/lambert_intensities.txt", std::ios_base::out);
    //      for(int32_t y = 0; y < config.imageDim; y++)
    //      {
    //        for(int32_t x = 0; x < config.imageDim; x++)
    //        {
    //          size_t idx = (y * config.imageDim) + x;
    //          out << (*poleFigurePtr)[idx] << "  ";
    //        }
    //        out << std::endl;
    //      }
    //    }

    config.discrete = true;
    ComputeStereographicProjection stereoGraphicProj(&xyzCoords, &config, poleFigurePtr.get());
    stereoGraphicProj();

    idims = {1};
    EbsdLib::UInt8ArrayType discrete(static_cast<size_t>(config.imageDim * config.imageDim), label0, idims, 0);
    for(size_t i = 0; i < poleFigurePtr->getNumberOfTuples(); i++)
    {
      if(poleFigurePtr->getValue(i) > 0.0)
      {
        discrete[i] = 1;
      }
    }

    dims = {static_cast<hsize_t>(config.imageDim), static_cast<hsize_t>(config.imageDim)};
    err = H5Lite::writePointerDataset(gid, "Discrete Pole Figure (Uint8)", dims.size(), dims.data(), discrete.getPointer(0));

    dims = {static_cast<hsize_t>(config.imageDim), static_cast<hsize_t>(config.imageDim)};
    err = H5Lite::writePointerDataset(gid, "Discrete Pole Figure (Double)", dims.size(), dims.data(), poleFigurePtr->getPointer(0));

    //    {
    //      std::ofstream out("/tmp/discrete.txt", std::ios_base::out);
    //      for(int32_t y = 0; y < config.imageDim; y++)
    //      {
    //        for(int32_t x = 0; x < config.imageDim; x++)
    //        {
    //          size_t idx = (y * config.imageDim) + x;
    //          out << (*poleFigurePtr)[idx] << "  ";
    //        }
    //        out << std::endl;
    //      }
    //    }

    H5Utilities::closeHDF5Object(gid);
    H5Utilities::closeFile(fid);
    return 0;
  }

  void setErrorCode(int32_t err)
  {
    m_ErrorCode = err;
  }
  void setErrorMessage(const std::string& msg)
  {
    m_ErrorMessage = msg;
  }
  std::string getFileName() const
  {
    return m_FilePath;
  }

private:
  std::string m_FilePath;
  int32_t m_ErrorCode = 0;
  std::string m_ErrorMessage;
};

// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  std::cout << "QuatID,distance,q0,q1,q2,q3,x,y,z" << std::endl;
  QuatD ref = {-0.700992, -0.0601976, -0.0557426, 0.708435};
  std::cout << "1000,0" << ',' << ref[0] << ',' << ref[1] << ',' << ref[2] << ',' << ref[3] << ',' << ref[0] / (1.0 + ref[3]) << ',' << ref[1] / (1.0 + ref[3]) << ',' << ref[2] / (1.0 + ref[3])
            << std::endl;

  QuatD q2 = {0.705249, -0.0377035, 0.055034, 0.705814};
  //  std::cout << "100,0" << ',' << q2[0] << ',' << q2[1] << ',' << q2[2] << ',' << q2[3] << ',' << q2[0] / (1.0 + q2[3]) << ',' << q2[1] / (1.0 + q2[3]) << ',' << q2[2] / (1.0 + q2[3]) << std::endl;

  HexagonalOps ops;
  //  QuatD fzQuat = ops.getFZQuat(qr);
  double dist = 0.0;
  double smallestdist = 1000000.0f;

  size_t nearestIndex = 0;

  QuatD qmax;
  size_t numsym = 12;
  for(size_t i = 0; i < numsym; i++)
  {
    QuatD qc = (ops.getQuatSymOp(i) * q2).unitQuaternion();

    if(qc.w() < 0)
    {
      qc.negate();
    }
    dist = static_cast<double>(1 - std::fabs(qc.w() * ref.w() + qc.x() * ref.x() + qc.y() * ref.y() + qc.z() * ref.z()));

    if(dist < smallestdist)
    {
      smallestdist = dist;
      qmax = qc;
      nearestIndex = i;
    }
    std::cout << i << ',' << dist << ',' << qc[0] << ',' << qc[1] << ',' << qc[2] << ',' << qc[3] << ',' << qc[0] / (1.0 + qc[3]) << ',' << qc[1] / (1.0 + qc[3]) << ',' << qc[2] / (1.0 + qc[3])
              << std::endl;
  }

  std::cout << "nearestIndex: " << nearestIndex << std::endl;

  //  std::string filePath(argv[1]);
  //  Sphere2PoleFigure s2f(filePath);
  //  int32_t err = s2f.convert();

  return 0;
}
