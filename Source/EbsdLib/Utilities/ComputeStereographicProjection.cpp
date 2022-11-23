/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
#include "ComputeStereographicProjection.h"

#define CSP_DEBUG_OUTPUT 1
#if CSP_DEBUG_OUTPUT
#ifdef EbsdLib_ENABLE_HDF5
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
using namespace H5Support;
#endif
#endif
#include "EbsdLib/Utilities/ModifiedLambertProjection.h"


// -----------------------------------------------------------------------------
ComputeStereographicProjection::ComputeStereographicProjection(EbsdLib::FloatArrayType* xyzCoords, PoleFigureConfiguration_t* config, EbsdLib::DoubleArrayType* intensity)
: m_XYZCoords(xyzCoords)
, m_Config(config)
, m_Intensity(intensity)
{
}

// -----------------------------------------------------------------------------
ComputeStereographicProjection::ComputeStereographicProjection() = default;

// -----------------------------------------------------------------------------
ComputeStereographicProjection::~ComputeStereographicProjection() = default;

// -----------------------------------------------------------------------------
void ComputeStereographicProjection::operator()() const
{
  // Resize the intensity image array to the final output image size
  m_Intensity->resizeTuples(static_cast<size_t>(m_Config->imageDim * m_Config->imageDim));
  m_Intensity->initializeWithZeros();

  if(m_Config->discrete)
  {
    int halfDim = m_Config->imageDim / 2;
    double* intensity = m_Intensity->getPointer(0);
    size_t numCoords = m_XYZCoords->getNumberOfTuples();
    float* xyzPtr = m_XYZCoords->getPointer(0);
    for(size_t i = 0; i < numCoords; i++)
    {
      if(xyzPtr[i * 3 + 2] < 0.0f)
      {
        xyzPtr[i * 3 + 0] *= -1.0f;
        xyzPtr[i * 3 + 1] *= -1.0f;
        xyzPtr[i * 3 + 2] *= -1.0f;
      }
      float x = xyzPtr[i * 3] / (1 + xyzPtr[i * 3 + 2]);
      float y = xyzPtr[i * 3 + 1] / (1 + xyzPtr[i * 3 + 2]);

      int xCoord = static_cast<int>(x * (halfDim - 1)) + halfDim;
      int yCoord = static_cast<int>(y * (halfDim - 1)) + halfDim;

      size_t index = static_cast<size_t>((yCoord * m_Config->imageDim) + xCoord);

      intensity[index]++;
    }
//#if CSP_DEBUG_OUTPUT
//    // This chunk is here for some debugging....
//    int dim = m_Config->imageDim;
//    std::string filename = "/tmp/Discrete-" + m_Intensity->getName() + "-" + dim + ".h5";
//    hid_t file_id = H5Utilities::createFile(filename);
//    hsize_t dims[2];
//    dims[0] = dim;
//    dims[1] = dim;
//    H5Lite::writePointerDataset(file_id, "Discrete", 2, dims, intensity);
//    H5Fclose(file_id);
//#endif
  }
  else
  {
    ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::LambertBallToSquare(m_XYZCoords, m_Config->lambertDim, m_Config->sphereRadius);
#if CSP_DEBUG_OUTPUT
    int dim = lambert->getDimension();
    std::string filename = "/tmp/Lambert-" + m_Config->phaseName + ".h5";

    hid_t file_id = H5Utilities::createFile(filename);

    auto northSquare = lambert->getNorthSquare();
    auto southSquare = lambert->getSouthSquare();
    int32_t rank = 2;
    std::array<hsize_t, 2> dims = {static_cast<hsize_t>(lambert->getDimension()),static_cast<hsize_t>(lambert->getDimension())};
    auto err = H5Lite::writePointerDataset(file_id, northSquare->getName(), rank, dims.data(), northSquare->getPointer(0));
    err = H5Lite::writePointerDataset(file_id, southSquare->getName(), rank, dims.data(), northSquare->getPointer(0));
    H5Fclose(file_id);
#endif
    lambert->normalizeSquaresToMRD();

    lambert->createStereographicProjection(m_Config->imageDim, *m_Intensity);
  }
}
