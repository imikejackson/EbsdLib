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

#pragma once

#include <array>
#include <sstream>
#include <string>
#include <vector>

#include "EbsdLib/Core/EbsdLibConstants.h"
#include "EbsdLib/Core/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"

class EbsdLib_EXPORT OinaPhase
{
public:
  using Self = OinaPhase;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  EBSD_STATIC_NEW_MACRO(OinaPhase)
  /**
   * @brief Returns the name of the class for OinaPhase
   */
  std::string getNameOfClass() const;
  /**
   * @brief Returns the name of the class for OinaPhase
   */
  static std::string ClassName();

  virtual ~OinaPhase();

  EBSD_INSTANCE_PROPERTY(int, PhaseIndex)
  EBSD_INSTANCE_PROPERTY(std::vector<uint8_t>, Color)
  EBSD_INSTANCE_PROPERTY(std::vector<float>, LatticeAngles)
  EBSD_INSTANCE_PROPERTY(std::vector<float>, LatticeDimensions)
  EBSD_INSTANCE_PROPERTY(int32_t, LaueGroup)
  EBSD_INSTANCE_PROPERTY(float, NumberReflectors)
  EBSD_INSTANCE_STRING_PROPERTY(PhaseName)
  EBSD_INSTANCE_STRING_PROPERTY(Reference)
  EBSD_INSTANCE_PROPERTY(int32_t, SpaceGroup)

  void printSelf(std::stringstream& stream);

protected:
  OinaPhase();

private:
public:
  OinaPhase(const OinaPhase&) = delete;            // Copy Constructor Not Implemented
  OinaPhase(OinaPhase&&) = delete;                 // Move Constructor Not Implemented
  OinaPhase& operator=(const OinaPhase&) = delete; // Copy Assignment Not Implemented
  OinaPhase& operator=(OinaPhase&&) = delete;      // Move Assignment Not Implemented
};
